import React from "react";
import { HiArrowRight, HiLightningBolt, HiShieldCheck, HiDeviceMobile } from "react-icons/hi";

export default function Page() {
  return (
    <div className="relative isolate pt-14">
      <div className="mx-auto max-w-7xl px-6 py-24 sm:py-32 lg:flex lg:items-center lg:gap-x-10 lg:px-8 lg:py-40">
        <div className="mx-auto max-w-2xl lg:mx-0 lg:flex-auto">
          <div className="flex">
            <div className="relative rounded-full px-3 py-1 text-sm leading-6 text-slate-400 ring-1 ring-slate-800 hover:ring-slate-700 transition-all">
              Latest release v1.0.0.{" "}
              <a href="/docs/installation" className="font-semibold text-blue-400">
                <span className="absolute inset-0" aria-hidden="true" />
                Read more <span aria-hidden="true">&rarr;</span>
              </a>
            </div>
          </div>
          <h1 className="mt-10 text-4xl font-bold tracking-tight text-white sm:text-6xl bg-gradient-to-br from-white to-slate-500 bg-clip-text text-transparent">
            Build Professional Node-Based Editors
          </h1>
          <p className="mt-6 text-lg leading-8 text-slate-300">
            A high-performance, reusable library for creating professional node editors like Unreal Engine or Blender.
            Dark themed, automatic data flow, and smooth interactions out of the box.
          </p>
          <div className="mt-10 flex items-center gap-x-6">
            <a
              href="/docs/getting-started"
              className="rounded-md bg-blue-600 px-3.5 py-2.5 text-sm font-semibold text-white shadow-sm hover:bg-blue-500 focus-visible:outline focus-visible:outline-2 focus-visible:outline-offset-2 focus-visible:outline-blue-400 transition-all flex items-center gap-2"
            >
              Get Started <HiArrowRight size={18} />
            </a>
            <a href="/docs/api-reference" className="text-sm font-semibold leading-6 text-white hover:text-blue-400 transition-colors">
              View API Reference <span aria-hidden="true">→</span>
            </a>
          </div>
        </div>
      </div>

      <div className="mx-auto max-w-7xl px-6 lg:px-8 pb-24">
        <div className="mx-auto grid max-w-2xl grid-cols-1 gap-x-8 gap-y-16 sm:gap-y-20 lg:mx-0 lg:max-w-none lg:grid-cols-3">
          <Feature 
            icon={<HiLightningBolt className="text-blue-400" />}
            title="Fast Data Flow"
            description="Automatic propagation of data through the graph using optimized topological sorting."
          />
          <Feature 
            icon={<HiShieldCheck className="text-blue-400" />}
            title="Type Safe"
            description="Built-in support for multiple data types with visual color coding and validation."
          />
          <Feature 
            icon={<HiDeviceMobile className="text-blue-400" />}
            title="Professional UI"
            description="Unreal Engine and Blender inspired dark theme with smooth zooming and panning."
          />
        </div>
      </div>
    </div>
  );
}

function Feature({ icon, title, description }: { icon: React.ReactNode, title: string, description: string }) {
  return (
    <div className="relative pl-16">
      <dt className="text-base font-semibold leading-7 text-white">
        <div className="absolute left-0 top-0 flex h-10 w-10 items-center justify-center rounded-lg bg-slate-900 border border-slate-800">
          {icon}
        </div>
        {title}
      </dt>
      <dd className="mt-2 text-base leading-7 text-slate-400">{description}</dd>
    </div>
  );
}
