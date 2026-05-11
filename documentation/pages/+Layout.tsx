import React from "react";
import "./Layout.css";
import logoUrl from "../assets/logo.svg";
import { Link } from "../components/Link";
import { HiMenu, HiX } from "react-icons/hi";
import { FaGithub } from "react-icons/fa";

export default function Layout({ children }: { children: React.ReactNode }) {
  const [isMobileMenuOpen, setIsMobileMenuOpen] = React.useState(false);

  return (
    <div className="min-h-screen bg-slate-950 text-slate-200 font-sans selection:bg-blue-500/30">
      {/* Navigation */}
      <nav className="sticky top-0 z-50 w-full border-b border-slate-800 bg-slate-950/80 backdrop-blur-md">
        <div className="mx-auto flex h-16 max-w-7xl items-center justify-between px-4 sm:px-6 lg:px-8">
          <div className="flex items-center gap-8">
            <a href="/" className="flex items-center gap-2 group">
              <img src={logoUrl} className="h-8 w-8 transition-transform group-hover:scale-110" alt="logo" />
              <span className="text-xl font-bold text-white tracking-tight">NodeEditor</span>
            </a>
            
            <div className="hidden md:flex items-center gap-6">
              <Link href="/docs/getting-started" className="text-sm font-medium hover:text-blue-400 transition-colors">Documentation</Link>
              <Link href="/docs/api-reference" className="text-sm font-medium hover:text-blue-400 transition-colors">API</Link>
              <Link href="/todo" className="text-sm font-medium hover:text-blue-400 transition-colors">Examples</Link>
            </div>
          </div>

          <div className="flex items-center gap-4">
            <div className="hidden sm:flex items-center gap-2 pr-4 border-r border-slate-800">
              <a href="https://github.com" target="_blank" rel="noreferrer" className="p-2 text-slate-400 hover:text-white transition-colors">
                <FaGithub size={20} />
              </a>
            </div>
            
            <button 
              className="md:hidden p-2 text-slate-400"
              onClick={() => setIsMobileMenuOpen(!isMobileMenuOpen)}
            >
              {isMobileMenuOpen ? <HiX size={24} /> : <HiMenu size={24} />}
            </button>
          </div>
        </div>

        {/* Mobile Menu */}
        {isMobileMenuOpen && (
          <div className="md:hidden border-b border-slate-800 bg-slate-950 px-4 py-4 space-y-4">
            <Link href="/docs/getting-started" className="block text-base font-medium">Documentation</Link>
            <Link href="/docs/api-reference" className="block text-base font-medium">API</Link>
            <Link href="/todo" className="block text-base font-medium">Examples</Link>
          </div>
        )}
      </nav>

      <main className="mx-auto max-w-7xl px-4 sm:px-6 lg:px-8 py-8">
        {children}
      </main>
      
      <footer className="mt-auto border-t border-slate-800 py-8 text-center text-sm text-slate-500">
        <p>© 2026 NodeEditor Project. Built with Vike and Tailwind CSS.</p>
      </footer>
    </div>
  );
}
