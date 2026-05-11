import react from "@vitejs/plugin-react";
/// <reference types="@batijs/core/types" />

import vike from "vike/plugin";
import { defineConfig } from "vite";
import tailwindcss from '@tailwindcss/vite'


export default defineConfig({
  plugins: [vike(), react(), tailwindcss()],
});
