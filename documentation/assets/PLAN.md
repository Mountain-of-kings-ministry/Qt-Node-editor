# MyNodeEditor — Implementation Plan

## Project Structure

```
nodeapackage/
├── CMakeLists.txt                   # Top-level: orchestrates lib + demo + tests
├── PLAN.md
├── readme
├── NodeEditor/                      # Reusable library
│   ├── CMakeLists.txt               # Static lib + QML module "NodeEditor 1.0"
│   ├── include/NodeEditor/
│   │   ├── GraphModel.h
│   │   ├── DataFlowEngine.h
│   │   └── BaseNode.h
│   ├── src/
│   │   ├── backend/
│   │   │   ├── GraphModel.cpp
│   │   │   └── DataFlowEngine.cpp
│   │   └── nodes/
│   │       └── BaseNode.cpp
│   └── qml/
│       ├── NodeCanvas.qml
│       ├── Node.qml
│       ├── Edge.qml
│       ├── Port.qml
│       ├── NodePalette.qml
│       └── PropertiesPanel.qml
├── examples/
│   └── Demo/
│       ├── CMakeLists.txt
│       ├── main.cpp
│       └── Main.qml
└── tests/
    ├── CMakeLists.txt
    ├── tst_GraphModel.cpp
    └── tst_DataFlowEngine.cpp
```

---

## Design Decisions

### Node Component Layout

```
┌─────────────────────────────┐  ← Header (draggable ONLY here)
│  ● [Node Title]             │  ← Colored gradient header per type
│     accent color             │     MouseArea captures drag — no other interaction
└─────────────────────────────┘
┌─────────────────────────────┐  ← Body (NOT draggable)
│  ○ in1    [input field]     │  ← Left: input port circle + label
│  ○ in2    [input field]     │     Right: data entry widget (SpinBox, TextField, etc.)
│  ○ in3    [input field]     │     No drag interference — body is inert to drag
│                             │
│              [output]  ●    │  ← Output ports right-aligned for connections
│              [output]  ●    │
└─────────────────────────────┘
```

- **Header**: Only drag point. Gradient accent color per node type. Title text.
- **Body**: Fixed, non-draggable. Contains input ports (left), data entry fields (right), output ports (right).
- **Ports**: Circles only for wiring connections. Ports and data fields are separate concepts.
- **No drag on body**: Users freely interact with fields/ports without moving the node.

---

## C++ Backend

### GraphModel
- `struct NodeData`: id (QUuid), type (string), position (QPointF), `QMap<QString, PortInfo> inputs`, `QMap<QString, PortInfo> outputs`, `QVariantMap data`
- `struct EdgeData`: id, sourceNodeId, sourcePort, targetNodeId, targetPort
- `enum class PortType { Int, Float, String, Bool, Color, Generic }`
- Signals: `nodeAdded`, `nodeRemoved`, `edgeAdded`, `edgeRemoved`, `nodeDataChanged`, `nodesDirty`
- Methods: `addNode/removeNode`, `connect/disconnect`, `setNodeData/getNodeData`, `topologicalSort`

### BaseNode
- `virtual QVariantMap compute(const QVariantMap& inputs) = 0`
- `virtual QMap<QString, PortType> inputSpec() const`
- `virtual QMap<QString, PortType> outputSpec() const`
- `virtual QString nodeType() const = 0`
- `virtual QString displayColor() const`
- `setDirty() / isDirty()`
- Registry: `QHash<QString, std::function<BaseNode*()>>`

### DataFlowEngine
- `processNodeChange(int nodeId)` — marks downstream dirty, topo-sorts, computes
- Kahn's algorithm with cycle detection
- Returns `bool hasCycle` + ordered node list
- Emits `propagationComplete(QList<int> computedNodes)`

---

## QML Frontend

### NodeCanvas.qml
- `Flickable` + `PinchArea` for zoom/pan
- Grid background: `#333` lines on `#1E1E1E`, 20px squares
- `Repeater` for nodes (absolutely positioned)
- `Repeater` for edges (Bézier curves)
- Handles connection creation: port click → rubber band line → drop on target port

### Node.qml
- Header drag via `MouseArea` bound to `GraphModel.nodePosition(id)`
- Body with input columns (port circle + label + data field)
- Output ports in dedicated row/column
- Selection highlight with `#00B4FF`

### Port.qml
- Color-coded circle per PortType
- `MouseArea` for click-drag connections
- Validates: same type, different node, no duplicate

### Edge.qml
- Cubic Bézier curve via `Canvas`
- Control points offset horizontally from endpoints
- Color: `#888` default, `#00B4FF` hover/selected
- Hover: thicker + glow effect

### NodePalette.qml
- `ListView` of node types
- Drag onto canvas → `graphModel.addNode(type, position)`

### PropertiesPanel.qml
- Optional advanced view for selected node
- Shows all ports, metadata, raw data

---

## Styling (Unreal/Blender Dark)

| Element | Color |
|---------|-------|
| Background | `#1E1E1E` |
| Node body | `#2A2A2A` |
| Grid lines | `#333333` |
| Selection | `#00B4FF` |
| Edge default | `#888888` |
| Edge hover | `#00B4FF` |
| Port Int | `#4A9EFF` |
| Port Float | `#4CDF8B` |
| Port String | `#FF9F43` |
| Port Bool | `#FF6B6B` |
| Port Color | `#A29BFE` |

---

## Implementation Order

### Step 1 — Project Restructure
- Create directory tree
- Top-level CMakeLists.txt orchestrating lib + demo + tests
- NodeEditor/CMakeLists.txt (static lib + QML module)
- examples/Demo/ files
- tests/CMakeLists.txt
- Verify build

### Step 2 — GraphModel (C++)
- Full data model with typed ports
- QtTest unit test

### Step 3 — BaseNode + DataFlowEngine (C++)
- Abstract node, registry, topological sort, propagation
- QtTest unit test

### Step 4 — QML Frontend
- NodeCanvas (zoom/pan/grid)
- Node (header drag, body with ports/fields)
- Port + Edge (connections)
- Wire to GraphModel

### Step 5 — Demo Application
- SplitView: Palette | Canvas | Properties
- 2-3 demo node types (Input, Add, Output)
- Dark theme, end-to-end flow test

### Step 6 — Polish
- NodePalette drag-drop
- PropertiesPanel
- Edge glow, validation
- Save/load (optional)

---

## Testing

- **C++ unit tests** via QtTest in `tests/`
  - `tst_GraphModel.cpp`: add/remove nodes/edges, signals
  - `tst_DataFlowEngine.cpp`: topological sort, cycle detection, propagation
- **Manual**: Demo app validates QML behavior
