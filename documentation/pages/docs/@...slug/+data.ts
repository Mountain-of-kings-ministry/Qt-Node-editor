import fs from "node:fs";
import path from "node:path";
import matter from "gray-matter";
import type { PageContextServer } from "vike/types";

export type Data = {
  content: string;
  title: string;
  slug: string;
  allDocs: DocGroup[];
};

type DocGroup = {
  group: string;
  items: { title: string; slug: string }[];
};

export async function data(pageContext: PageContextServer): Promise<Data> {
  // slug is an array or string for catch-all routes in Vike
  const rawSlug = (pageContext.routeParams as any)["...slug"];
  const slug = Array.isArray(rawSlug) ? rawSlug.join("/") : (rawSlug || "");
  const docsDir = path.join(process.cwd(), "docs");
  
  // Recursive function to get all md files
  function getFiles(dir: string, baseDir: string): { title: string; slug: string; group: string }[] {
    const entries = fs.readdirSync(dir, { withFileTypes: true });
    let files: any[] = [];
    
    for (const entry of entries) {
      const fullPath = path.join(dir, entry.name);
      if (entry.isDirectory()) {
        files = [...files, ...getFiles(fullPath, baseDir)];
      } else if (entry.name.endsWith(".md")) {
        const relativePath = path.relative(baseDir, fullPath);
        const name = relativePath.replace(".md", "");
        const parts = name.split("/");
        const group = parts.length > 1 ? parts[0] : "General";
        const title = parts[parts.length - 1]
          .split("-")
          .map(word => word.charAt(0).toUpperCase() + word.slice(1))
          .join(" ");
        files.push({ title, slug: name, group });
      }
    }
    return files;
  }

  const allFiles = getFiles(docsDir, docsDir);
  
  // Group files
  const groupedDocs: Record<string, { title: string; slug: string }[]> = {};
  allFiles.forEach(f => {
    if (!groupedDocs[f.group]) groupedDocs[f.group] = [];
    groupedDocs[f.group].push({ title: f.title, slug: f.slug });
  });

  const allDocs: DocGroup[] = Object.entries(groupedDocs).map(([group, items]) => ({
    group: group === "General" ? "Introduction" : group.charAt(0).toUpperCase() + group.slice(1),
    items
  }));

  const filePath = path.join(docsDir, `${slug}.md`);
  
  if (!fs.existsSync(filePath)) {
    return {
      content: "# 404 - Not Found\n\nThe requested documentation page could not be found.",
      title: "Not Found",
      slug,
      allDocs
    };
  }

  const fileContent = fs.readFileSync(filePath, "utf-8");
  const { data: frontmatter, content } = matter(fileContent);

  const title = frontmatter.title || slug
    .split("/")
    .pop()!
    .split("-")
    .map((word: string) => word.charAt(0).toUpperCase() + word.slice(1))
    .join(" ");

  return {
    content,
    title,
    slug,
    allDocs
  };
}
