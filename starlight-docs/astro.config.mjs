// @ts-check
import { defineConfig } from 'astro/config';
import starlight from '@astrojs/starlight';

// https://astro.build/config
export default defineConfig({
	integrations: [
		starlight({
			title: 'Riot Guides',
			social: {
				github: 'https://github.com/riot-os/riot',
			},
			sidebar: [
                {
                    label: 'Home',
                    items: [
                        { label: 'Welcome to RIOT', slug: 'index' },
                    ]
                },
				{
					label: 'Guides',
					autogenerate: { directory: 'guides' },  
				},
			],
		}),
	],
});
