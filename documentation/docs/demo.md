# Demo Applications

The project ships with **6 executable examples** demonstrating various NodeEditor capabilities.

## 1. Basic Demo (`examples/Demo`)

A minimal workspace with 4 custom node types for testing evaluation.

### Built-in Demo Nodes

| Node | Type | Inputs | Outputs |
|---|---|---|---|
| Demo Input | `demo/input` | input (Float) | value (Float) |
| Demo Add | `demo/add` | a (Float), b (Float) | result (Float) |
| Demo Multiply | `demo/multiply` | a (Float), b (Float) | result (Float) |
| Demo Output | `demo/output` | value (Float) | — |

```
Shift+A → click "Demo Input" → click "Demo Add" → connect ports → edit values
```

## 2. Time Display (`examples/time-display`)

Displays the current time and timestamp using the built-in `CurrentTimeNode` and display nodes.

### Graph
- `system/info/currentTime` → `output/display/text` (formatted time)
- `system/info/currentTime` → `output/display/number` (timestamp)

### Run
```bash
./build/debug/examples/time-display/TimeDisplayApp
```

Import the pre-made graph from `examples/time-display/time-display.json`.

## 3. LED Matrix (`examples/led-matrix`)

Demonstrates the `LEDMatrixDisplayNode` — renders text as a retro LED matrix visualization.

### Graph
- `utility/value/constant` → `output/display/ledMatrix`

### Run
```bash
./build/debug/examples/led-matrix/LedMatrixApp
```

## 4. Key Press Visualizer (`examples/key-press`)

Captures keyboard input and visualizes key state with a boolean indicator. Focus the window and press any key.

### Graph
- `keyPress/capture` → `output/display/boolean`

### Run
```bash
./build/debug/examples/key-press/KeyPressApp
```

## 5. Electric Circuit Simulator (`examples/electric-demo`)

Simulates a basic electrical circuit: battery → resistor → LED, with voltage and brightness displays.

### Graph
- `electric/battery` → `electric/resistor` → `electric/led`
- `electric/resistor` → `output/display/number` (voltage)
- `electric/led` → `output/display/progress` (brightness)

### Run
```bash
./build/debug/examples/electric-demo/ElectricDemoApp
```

## 6. JSON Script Interpreter (`examples/script-demo`)

Executes simple stack-based scripts written in JSON (push, add, mul, sub, div, dup, swap, drop, print). Displays the result inline.

### Graph
- `script/jsonInterpreter` → `output/display/number`

### Run
```bash
./build/debug/examples/script-demo/ScriptDemoApp
```

## Usage Tips

1. Build all: `cmake --preset debug && cmake --build --preset debug`
2. Press **Shift+A** to search and add nodes
3. Click-drag from output ports (right, green dots) to input ports (left, colored dots)
4. Type values into input fields, press **Enter** to commit
5. Connected input fields are automatically locked (read-only)
6. Delete nodes with **Delete** key or right-click → remove
7. **Display nodes** show their output visually inside the node body (inline preview)
8. **Right-click an output port** to disconnect
9. Press **F** to fit all nodes into view

## Embedding Configuration

When using `NodeEditorWorkspace` in another project, you can hide UI elements:

```qml
NodeEditorWorkspace {
    anchors.fill: parent
    showTopBar: false     // hide toolbar
    showBottomBar: false  // hide tab bar
}
```

All keyboard shortcuts remain active. Use `framelessWindow: true` to remove window decorations (requires parent `Window` with `Qt.FramelessWindowHint`).

## Running Tests

```bash
./build/debug/tests/TestGraphModel
./build/debug/tests/TestDataFlowEngine
```
