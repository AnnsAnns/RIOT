function cleanDoxygenText(value: string): string {
	return value
		.replace(/^\*+\s*/, "")
		.replace(/\*\/\s*$/, "")
		.replace(/^"|"$/g, "")
		.trim();
}

function replaceInlineDoxygenCommands(line: string): string {
	return line
		.replace(/[@\\]c\s+([A-Za-z0-9_./:-]+)/g, "`$1`")
		.replace(/[@\\]ref\s+([A-Za-z0-9_./:-]+)/g, "$1")
		.replace(/[@\\]p\s+([A-Za-z0-9_./:-]+)/g, "`$1`");
}

function stripDoxygenAnchors(line: string): string {
	// Doxygen markdown headings often append anchors like `{#TITLE}`.
	// Remove these anchor fragments from rendered output.
	return line.replace(/\s*\{#[^}]+\}\s*/g, " ").trimEnd();
}

type CalloutKind = "note" | "tip" | "caution";

function getCalloutConfig(command: string): { kind: CalloutKind; title: string } | null {
	const key = command.toLowerCase();
	if (key === "warning" || key === "attention" || key === "important") {
		return { kind: "caution", title: "Warning" };
	}
	if (key === "note" || key === "remark") {
		return { kind: "note", title: "Note" };
	}
	if (key === "tip" || key === "hint") {
		return { kind: "tip", title: "Tip" };
	}
	return null;
}

function formatCallout(kind: CalloutKind, title: string, text: string): string[] {
	// Loader-rendered markdown does not always apply directive transforms,
	// so use a markdown blockquote that always renders visibly.
	return ["", "> **" + title + ":** " + text, ""];
}

export function extractBoardTitleFromDoxygen(content: string, fallback: string): string {
	const lines = content.split("\n");
	const defgroupPattern = /@defgroup\s+boards_[A-Za-z0-9._-]+\s+(.+)$/;

	for (let i = 0; i < lines.length; i++) {
		const match = lines[i].match(defgroupPattern);
		if (!match) {
			continue;
		}
		return cleanDoxygenText(match[1]);
	}

	return fallback;
}

/**
 * Convert common Doxygen directives to Markdown/Starlight friendly syntax.
 */
export function filterDoxygenMarkdown(content: string): string {
	const sourceLines = content.split("\n");
	const out: string[] = [];

	for (let i = 0; i < sourceLines.length; i++) {
		const rawLine = sourceLines[i];
		const line = rawLine.trim();

		// Ignore grouping directives that are metadata only.
		if (/^@(?:defgroup|ingroup|addtogroup|name)\b/.test(line)) {
			continue;
		}

		const brief = line.match(/^[@\\]brief\s+(.+)$/);
		if (brief) {
			out.push(cleanDoxygenText(brief[1]));
			continue;
		}

		const calloutMatch = line.match(/^[@\\](warning|attention|important|note|remark|tip|hint)\s+(.+)$/i);
		if (calloutMatch) {
			const config = getCalloutConfig(calloutMatch[1]);
			if (config) {
				out.push(...formatCallout(config.kind, config.title, cleanDoxygenText(calloutMatch[2])));
			}
			continue;
		}

		const image = line.match(/^[@\\]image\s+\w+\s+(\S+)(?:\s+(.+))?$/);
		if (image) {
			const imagePath = image[1];
			const imageArgs = image[2] || "";
			const quotedCaption = imageArgs.match(/"([^"]+)"/);
			const optionless = imageArgs.replace(/\s+[A-Za-z0-9_-]+\s*=\s*[^\s]+/g, "").trim();
			const caption = cleanDoxygenText(quotedCaption?.[1] || optionless || "Image");
			out.push("![" + caption + "](" + imagePath + ")");
			continue;
		}

		// Skip other standalone doxygen commands we do not map yet.
		if (/^[@\\][A-Za-z_]+\b/.test(line)) {
			continue;
		}

		out.push(stripDoxygenAnchors(replaceInlineDoxygenCommands(rawLine)));
	}

	return out.join("\n");
}
