# Node Editor

A Qt6/C++ node editor with inline display nodes, data-flow evaluation, and QML workspace.

## Features

- **153+ built-in nodes** across 15 categories (Math, Logic, Color, Data, Output, System, etc.)
- **28 Output nodes** including 17 display nodes with inline preview rendering
- **Display nodes** render output visually inside the node body (charts, gauges, LED matrix, text)
- **Data flow engine** with automatic propagation and cycle detection
- **QML-based workspace** with drag, zoom, select, undo/redo
- **Serialization** to/from JSON files
- **Dynamic port management** (add/remove ports at runtime)
- **External node registration** via `registerNodeType<T>` template

## Build

```bash
cmake --preset debug
cmake --build --preset debug
```

## Run

```bash
./build/debug/examples/Demo/NodeEditorDemoApp
./build/debug/examples/time-display/TimeDisplayApp
./build/debug/examples/led-matrix/LedMatrixApp
./build/debug/examples/key-press/KeyPressApp
./build/debug/examples/electric-demo/ElectricDemoApp
./build/debug/examples/script-demo/ScriptDemoApp
```

## Tests

```bash
./build/debug/tests/TestGraphModel
./build/debug/tests/TestDataFlowEngine
```

## Documentation

See `documentation/docs/` for full API reference, node list, and example guides.
