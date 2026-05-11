# Demo Application

## Overview

The demo application (`examples/Demo`) provides a working NodeEditor workspace with 4 built-in node types for testing and evaluation.

## Built-in Demo Nodes

| Node | Category | Inputs | Outputs | Description |
|---|---|---|---|---|
| Input | Input | input (Float) | value (Float) | Pass-through: sets a value to send downstream |
| Add | Math → Basic Operations | a (Float), b (Float) | result (Float) | Computes a + b |
| Multiply | Math → Basic Operations | a (Float), b (Float) | result (Float) | Computes a * b |
| Output | Output | value (Float) | — | Sink: receives and displays a value |

## Usage

1. Build: `cmake --preset debug && cmake --build --preset debug`
2. Run: `./build/debug/examples/Demo/NodeEditorDemoApp`
3. Press **Shift+A** to add nodes
4. Click-drag from output ports (right, green dots) to input ports (left, colored dots) to connect
5. Type values into input fields, press **Enter** to commit
6. Connected input fields are automatically locked (read-only)
7. Delete nodes with **Delete** key or right-click → remove
8. Disconnect edges by right-clicking on them

## Build & Run

```bash
cmake --preset debug && cmake --build --preset debug
./build/debug/examples/Demo/NodeEditorDemoApp
```

Running all tests:

```bash
./build/debug/tests/TestGraphModel
./build/debug/tests/TestDataFlowEngine
```