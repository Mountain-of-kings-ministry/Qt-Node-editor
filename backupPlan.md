Got it. Since I'm in read-only plan mode, I'll present the full plan here as text. When you say "go", I'll write the plan file and begin building.

---

## Detailed Implementation Plan

### Project Structure
```
nodeapackage/
├── CMakeLists.txt                   # Top-level: add_subdirectory(NodeEditor) + add_subdirectory(examples/Demo)
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
│       ├── main.cpp
│       ├── Main.qml
│       └── CMakeLists.txt
└── tests/
    ├── CMakeLists.txt
    ├── tst_GraphModel.cpp
    └── tst_DataFlowEngine.cpp
```

### C++ Backend Design

**1. GraphModel** — Core data container
- `struct NodeData`: id (int/QUuid), type (string), position (QPointF), `QMap<QString, PortInfo> inputs`, `QMap<QString, PortInfo> outputs`, `QVariantMap data`
- `struct EdgeData`: id, sourceNodeId, sourcePort, targetNodeId, targetPort
- `enum class PortType { Int, Float, String, Bool, Color, Generic }` for typed ports
- Signals: `nodeAdded`, `nodeRemoved`, `edgeAdded`, `edgeRemoved`, `nodeDataChanged`, `nodesDirty(QList<int>)`
- Methods: `addNode/removeNode`, `connect/disconnect`, `setNodeData/getNodeData`, `topologicalSort`

**2. BaseNode** — Abstract compute node
- `virtual QVariantMap compute(const QVariantMap& inputs) = 0`
- `virtual QMap<QString, PortType> inputSpec() const`
- `virtual QMap<QString, PortType> outputSpec() const`
- `setDirty()` / `isDirty()`
- Registration system: `QHash<QString, std::function<BaseNode*()>> nodeRegistry`

**3. DataFlowEngine** — Propagation engine
- `processNodeChange(int nodeId)` — marks downstream dirty, topo-sorts, computes
- Kahn's algorithm with cycle detection
- Returns `bool hasCycle` + list of nodes in order
- Emits `propagationComplete(QList<int> computedNodes)`

### QML Frontend Design

**NodeCanvas.qml** — `Flickable` + `PinchArea` for zoom/pan
- Grid background via `Canvas` with 20px squares, `#333` lines on `#1E1E1E`
- `Repeater` for nodes (positioned absolutely)
- `Repeater` for edges (Bézier curves)
- Property: `graphModel` (attached C++ GraphModel)

**Node.qml** — `Rectangle` with drag support
- Header: colored `Rectangle` with title text, type accent color
- Body: grey `#2A2A2A`, contains port columns
- Left column: input `Port` items
- Right column: output `Port` items
- Drag: `MouseArea` on header + `Drag.active` binding to GraphModel position
- Selection: blue border when `selected`
- Binding: `x/y` ↔ `GraphModel.nodePosition(id)`

**Port.qml** — Small colored circle
- Color mapped to `PortType`: Int=blue, Float=green, String=orange, Bool=red, Color=rainbow
- `MouseArea` for click-drag to create connections
- Emits `connectionStarted(portId)` which NodeCanvas handles
- Validates connection target (same type, not same node, not duplicate)

**Edge.qml** — `Canvas` or `ShapePath` Bézier curve
- Cubic Bézier: control points offset horizontally from endpoints
- Color: `#888` default, `#00B4FF` on hover/selected
- Hover: thicker stroke + glow (via layer.effect or second ShapePath)
- `MouseArea` for selection + deletion

**NodePalette.qml** — `ListView` of node types
- Drag source: `Drag.dragType = Drag.Automatic`
- On drop into canvas → `graphModel.addNode(type, position)`

**PropertiesPanel.qml** — Dynamic form
- Reads selected node's port specs
- Creates appropriate input controls: `SpinBox` for int, `TextField` for string, etc.
- Writes back via `graphModel.setNodeData(nodeId, portName, value)`

### Styling (Unreal/Blender Dark)
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

### Implementation Order

**Step 1** — Project restructure
- Create directory tree
- Rewrite `CMakeLists.txt` top-level + NodeEditor lib + Demo + Tests
- Verify it builds

**Step 2** — GraphModel (C++)
- Full data model with typed ports
- QtTest unit test

**Step 3** — BaseNode + DataFlowEngine (C++)
- Abstract node, registry, topological sort + propagation
- QtTest unit test for engine

**Step 4** — QML components
- `NodeCanvas.qml` with zoom/pan/grid
- `Node.qml` with drag, header, ports
- `Port.qml` + `Edge.qml` with connection logic
- Wire up to GraphModel via context property

**Step 5** — Demo application
- `SplitView` layout with Palette / Canvas / Properties
- 2-3 demo node types (Input, Add, Output)
- Dark theme applied
- Verify end-to-end: create nodes → connect → type data → see propagation

**Step 6** — Polish
- `NodePalette.qml` drag-drop
- `PropertiesPanel.qml` dynamic controls
- Edge hover glow, port connection validation
- README with build instructions

### Testing Strategy
- **C++ unit tests** (QtTest): `tests/tst_GraphModel.cpp`, `tests/tst_DataFlowEngine.cpp`
  - Run via: `cmake --build build --target NodeEditorTest && ./build/tests/NodeEditorTest`
- **Manual QML testing**: Demo app validates UI behavior
- No integration testing with external projects until core is solid

---

That's the full plan. Shall I go ahead and write it to `PLAN.md`, then start implementing **Step 1**?
