// @ts-check
import { defineConfig } from 'astro/config';
import starlight from '@astrojs/starlight';

// https://astro.build/config
export default defineConfig({
    site: "https://riotguide.annsann.eu",
    integrations: [
      starlight({
        title: "Riot Documentation",
        social: [
          { icon: 'github', label: 'GitHub', href: 'https://github.com/riot-os/riot' },
          { icon: 'mastodon', label: 'Mastodon', href: 'https://fosstodon.org/@RIOT_OS' },
          { icon: 'matrix', label: 'Matrix', href: 'https://matrix.to/#/#riot-os:matrix.org' },
        ],
        sidebar: [
          {
            label: "RIOT in a Nutshell",
            items: [
              { label: "Introduction", slug: "index" },
              { label: "RIOT Vision", slug: "general/vision" },
              "general/community_process",
              "general/structure",
            ],
          },
          {
            label: "Build System",
            autogenerate: { directory: "build-system" },
          },
        ],
        customCss: [],
        logo: {
          src: "./src/assets/riot-logo.svg",
          replacesTitle: true,
        },
        plugins: [],
        editLink: {
          baseUrl:
            "https://github.com/AnnsAnns/RIOT/tree/newguides/doc/guides",
        },
      }),
    ],
  });
  