import React from "react";
import { useData } from "vike-react/useData";
import { Link } from "../../components/Link";
import type { Data } from "./@slug/+data";

export default function Layout({ children }: { children: React.ReactNode }) {
  const data = useData<Data>();
  const allDocs = data?.allDocs || [];

  return (
    <div className="flex flex-col md:flex-row gap-8 min-h-[calc(100vh-10rem)]">
      {/* Sidebar */}
      <aside className="w-full md:w-64 flex-shrink-0 border-b md:border-b-0 md:border-r border-slate-800 pb-8 md:pb-0 md:pr-8">
        <div className="sticky top-24 max-h-[calc(100vh-8rem)] overflow-y-auto pr-2 custom-scrollbar">
          {allDocs.map((group) => (
            <div key={group.group} className="mb-8">
              <h5 className="mb-3 text-sm font-bold uppercase tracking-wider text-white">{group.group}</h5>
              <ul className="space-y-1">
                {group.items.map((doc) => (
                  <li key={doc.slug}>
                    <Link 
                      href={`/docs/${doc.slug}`} 
                      className="block py-1.5 text-sm text-slate-400 hover:text-blue-400 transition-colors"
                    >
                      {doc.title}
                    </Link>
                  </li>
                ))}
              </ul>
            </div>
          ))}
        </div>
      </aside>

      {/* Content */}
      <div className="flex-1 min-w-0">
        {children}
      </div>
    </div>
  );
}
