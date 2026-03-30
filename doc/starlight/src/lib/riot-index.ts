import fs from "node:fs";
import path from "node:path";
import { fileURLToPath } from "node:url";

export type RiotItem = {
  name: string;
  group: string;
  module: string,
  file: string;
  href: string;
};

const TEXT_EXTENSIONS = [".c", ".cpp", ".h", ".hpp", ".inl", ".md", ".txt"];
const DEF_GROUP_PATTERN = /@defgroup\s+(boards|cpu|drivers)_([A-Za-z0-9._-]+)\s+(.+)$/;

function getRiotBase(): string {
  const moduleDir = path.dirname(fileURLToPath(import.meta.url));
  const defaultBase = path.resolve(moduleDir, "../../../../");
  return process.env.RIOTBASE ? process.env.RIOTBASE : defaultBase;
}

function toApiUrl(kind: "boards" | "cpu" | "drivers", group: string): string {
  const base = process.env.RIOT_DOC_BASE_URL
    ? process.env.RIOT_DOC_BASE_URL
    : "https://api.riot-os.org";
  return base + "/group__" + kind + "__" + group + ".html";
}

function hasTextExtension(fileName: string): boolean {
  return TEXT_EXTENSIONS.indexOf(path.extname(fileName)) !== -1;
}

function walkFiles(directoryPath: string, out: string[] = []): string[] {
  const results = out;
  const entries = fs.readdirSync(directoryPath, { withFileTypes: true });

  for (let i = 0; i < entries.length; i++) {
    const entry = entries[i];
    if (entry.name.indexOf(".") === 0) {
      continue;
    }

    const fullPath = path.join(directoryPath, entry.name);
    if (entry.isDirectory()) {
      walkFiles(fullPath, results);
      continue;
    }

    if (hasTextExtension(entry.name)) {
      results.push(fullPath);
    }
  }

  return results;
}

function cleanName(raw: string): string {
  return raw.replace(/\*\/\s*$/, "").replace(/^\*+\s*/, "").trim();
}

function parseDefGroup(
  filePath: string,
  prefix: "boards" | "cpu" | "drivers",
): { group: string; name: string } | null {
  const lines = fs.readFileSync(filePath, "utf8").split("\n");

  for (let i = 0; i < lines.length; i++) {
    const line = lines[i];
    const match = line.match(DEF_GROUP_PATTERN);
    if (!match) {
      continue;
    }

    if (match[1] !== prefix) {
      continue;
    }

    return {
      group: match[2].split("_").join("__"),
      name: cleanName(match[3]),
    };
  }

  return null;
}

function listSubdirs(directoryPath: string): string[] {
  if (!fs.existsSync(directoryPath)) {
    return [];
  }

  return fs
    .readdirSync(directoryPath, { withFileTypes: true })
    .filter(function (entry) {
      return entry.isDirectory() && entry.name.indexOf(".") !== 0;
    })
    .map(function (entry) {
      return entry.name;
    })
    .sort();
}

function upsertByGroup(store: Record<string, RiotItem>, item: RiotItem): void {
  if (!store[item.group]) {
    store[item.group] = item;
  }
}

function sortedValues(store: Record<string, RiotItem>): RiotItem[] {
  return Object.keys(store)
    .map(function (key) {
      return store[key];
    })
    .sort(function (a, b) {
      return a.name.localeCompare(b.name);
    });
}

function getBoards(): RiotItem[] {
  const riotBase = getRiotBase();
  const boardsRoot = path.join(riotBase, "boards");
  if (!fs.existsSync(boardsRoot)) {
    return [];
  }

  const files = walkFiles(boardsRoot, []);
  const byGroup: Record<string, RiotItem> = {};

  for (let i = 0; i < files.length; i++) {
    const filePath = files[i];
    const parsed = parseDefGroup(filePath, "boards");
    if (!parsed) {
      continue;
    }

    if (parsed.group.indexOf("common") !== -1) {
      continue;
    }

    const repoRelative = path.relative(riotBase, filePath);
    const module = repoRelative.split(path.sep)[1];
    upsertByGroup(byGroup, {
      name: parsed.name,
      group: parsed.group,
      file: repoRelative,
      module: module,
      href: toApiUrl("boards", parsed.group),
    });
  }

  return sortedValues(byGroup);
}

function getCpus(): RiotItem[] {
  const riotBase = getRiotBase();
  const cpuRoot = path.join(riotBase, "cpu");
  if (!fs.existsSync(cpuRoot)) {
    return [];
  }

  const candidates = [
    "doc.md",
    path.join("include", "cpu.h"),
    path.join("include", "cpu_conf.h"),
    path.join("include", "periph_cpu.h"),
  ];

  const byGroup: Record<string, RiotItem> = {};
  const cpuDirs = listSubdirs(cpuRoot).filter(function (name) {
    return name !== "common";
  });

  for (let i = 0; i < cpuDirs.length; i++) {
    const cpuName = cpuDirs[i];
    for (let j = 0; j < candidates.length; j++) {
      const relativeCandidatePath = candidates[j];
      const fullPath = path.join(cpuRoot, cpuName, relativeCandidatePath);
      if (!fs.existsSync(fullPath)) {
        continue;
      }

      const parsed = parseDefGroup(fullPath, "cpu");
      if (!parsed) {
        continue;
      }

      if (parsed.group.slice(-4) === "conf") {
        continue;
      }

      if (parsed.group.indexOf("common") !== -1) {
        continue;
      }

      if (parsed.name.toLowerCase().indexOf("common") !== -1) {
        continue;
      }

      const repoRelative = path.relative(riotBase, fullPath);
      const module = repoRelative.split(path.sep)[0];
      upsertByGroup(byGroup, {
        name: parsed.name,
        group: parsed.group,
        file: repoRelative,
        module: module,
        href: toApiUrl("cpu", parsed.group),
      });
    }
  }

  return sortedValues(byGroup);
}

function getDrivers(): RiotItem[] {
  const riotBase = getRiotBase();
  const driversRoot = path.join(riotBase, "drivers");
  if (!fs.existsSync(driversRoot)) {
    return [];
  }

  const files = walkFiles(driversRoot, []);
  const byGroup: Record<string, RiotItem> = {};

  for (let i = 0; i < files.length; i++) {
    const filePath = files[i];
    const parsed = parseDefGroup(filePath, "drivers");
    if (!parsed) {
      continue;
    }

    if (parsed.group.slice(-6) === "config") {
      continue;
    }

    const repoRelative = path.relative(riotBase, filePath);
    const module = repoRelative.split(path.sep)[0];

    upsertByGroup(byGroup, {
      name: parsed.name,
      group: parsed.group,
      file: repoRelative,
      module: module,
      href: toApiUrl("drivers", parsed.group),
    });
  }

  return sortedValues(byGroup);
}

function getRiotIndex(): {
  boards: RiotItem[];
  cpus: RiotItem[];
  drivers: RiotItem[];
} {
  return {
    boards: getBoards(),
    cpus: getCpus(),
    drivers: getDrivers(),
  };
}

export {
  getBoards,
  getCpus,
  getDrivers,
  getRiotIndex,
};
