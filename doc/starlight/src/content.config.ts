import { defineCollection, z } from "astro:content";
import { glob, type Loader, type LoaderContext } from "astro/loaders";
import { docsSchema } from "@astrojs/starlight/schema";
import { docsLoader } from "@astrojs/starlight/loaders";
import type { MarkdownInstance } from "astro";

export function myLoader(): Loader {
  // Return a loader object
  return {
    name: "boards",
    // Called when updating the collection.
    load: async (context: LoaderContext): Promise<void> => {
      //const boards = import.meta.glob({ pattern: "**/*.(md|mdx)", base: "../../boards" }).load(context)

      const boards = Object.values(
        import.meta.glob<MarkdownInstance<Record<any, any>>>(
          "../../../boards/rpi-pico-2/doc.md",
          { eager: true }
        )
      );

      context.store.clear();

      for (const board of boards) {
        // Extract board name from path (folder-name)
        const boardNameParts = board.file.split("/");
        const boardName = boardNameParts[boardNameParts.length - 2];

        // Create a proper entry ID using the board name
        const entryId = `boards/${boardName}`;

        console.log(entryId);

        // Set the entry in the store with proper metadata formatting
        context.store.set({
          id: entryId,
          body: board.rawContent(),
          data: {
            title: board.frontmatter?.title || boardName,
            description: board.frontmatter?.description || "",
            ...board.frontmatter,
          },
        });
      }
    },
  };
}

const docs = defineCollection({
  loader: glob({ pattern: "**/*.(md|mdx)", base: "../guides" }),
  schema: docsSchema(),
});

const boards = defineCollection({
    loader: myLoader(),
    schema: docsSchema(),
})

export const collections = { docs, boards };
