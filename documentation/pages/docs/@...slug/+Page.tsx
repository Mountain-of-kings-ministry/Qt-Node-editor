import React from "react";
import ReactMarkdown from "react-markdown";
import remarkGfm from "remark-gfm";
import { useData } from "vike-react/useData";
import type { Data } from "./+data";

export default function Page() {
  const { content, title } = useData<Data>();

  return (
    <div className="mx-auto max-w-4xl">
      <h1 className="mb-8 text-4xl font-bold text-white tracking-tight">{title}</h1>
      <div className="prose prose-invert prose-slate max-w-none prose-headings:scroll-mt-20 prose-a:text-blue-400 hover:prose-a:text-blue-300">
        <ReactMarkdown remarkPlugins={[remarkGfm]}>
          {content}
        </ReactMarkdown>
      </div>
    </div>
  );
}
