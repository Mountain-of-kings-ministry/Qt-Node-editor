# API Reference

## C++ Classes

### GraphModel
`include/NodeEditor/GraphModel.h`

The central data model for a node graph. Manages nodes, edges, data, and topological order.

| Method | Description |
|---|---|
| `addNode(type, position, existingId)` | Add a node of the given type |
| `removeNode(nodeId)` | Remove a node and its edges; resets downstream input ports to defaults |
| `connectPorts(src, srcPort, tgt, tgtPort)` | Create an edge between ports |
| `disconnectEdge(edgeId)` | Remove an edge; resets target input port to default |
| `setNodeData(nodeId, key, value)` | Set a node's data value |
| `nodeData(nodeId, key)` | Get a node's data value |
| `topologicalSort()` | Return nodes in dependency order |
| `hasCycles()` | Check for cycles in the graph |
| `registerNodeType(type, info)` | Register a node type |
| `registerCategory(category)` | Register a node category |
| `clear()` | Remove all nodes and edges (Q_INVOKABLE) |

### UndoManager
`include/NodeEditor/UndoManager.h`

Wraps `QUndoStack` with commands for all graph operations. Exposed to QML as `_undoManager`.

| Method | Description |
|---|---|
| `undo()` | Undo last command |
| `redo()` | Redo last undone command |
| `clear()` | Clear undo history |
| `canUndo()` / `canRedo()` | Check availability |

### BaseNode
`include/NodeEditor/BaseNode.h`

Abstract base for custom nodes.

| Method | Description |
|---|---|
| `nodeName()` | Display name |
| `nodeType()` | Unique type path |
| `nodeCategory()` | Category ID |
| `nodeSubCategory()` | Subcategory path |
| `displayColor()` | Hex color for node header (e.g. `"#4A9EFF"`) |
| `inputSpec()` | List of input PortInfo |
| `outputSpec()` | List of output PortInfo |
| `compute(inputs)` | Execute node logic; returns QVariantMap of outputs |
| `setDirty(bool)` | Mark node for re-evaluation |

### registerNodeType&lt;T&gt;(model, categoryId)
Template helper in `BaseNode.h`. Registers a node class T by reading its specs at runtime:

```cpp
registerNodeType<MyNode>(model, "MyCategory");
```

Equivalent to:
```cpp
MyNode tmp;
model->registerNodeType(tmp.nodeType(), {/* auto-filled from tmp */});
```

### DataFlowEngine
`include/NodeEditor/DataFlowEngine.h`

Propagates data through the graph. Writes resolved input values back to the model so QML can display them.
Triggered by `nodeDataChanged`, `nodeAdded`, and `edgeAdded` signals.

## QML API (GraphModel)

| Method | Description |
|---|---|
| `qmlAddNode(type, x, y)` | Add a node from QML |
| `qmlRemoveNode(nodeId)` | Remove a node from QML |
| `qmlNodeIds()` | List all node IDs |
| `qmlNodeInfo(nodeId)` | Get node metadata as QVariantMap |
| `qmlNodeData(nodeId, key)` | Get a node's data value |
| `qmlSetNodeData(nodeId, key, value)` | Set a node's data value |
| `qmlConnectPorts(src, srcPort, tgt, tgtPort)` | Create an edge |
| `qmlDisconnectEdge(edgeId)` | Remove an edge |
| `qmlEdgeIds()` | List all edge IDs |
| `qmlEdgeInfo(edgeId)` | Get edge metadata as QVariantMap |
| `qmlIsPortConnected(nodeId, port, isInput)` | Check if a port has a connection |
| `qmlAllNodeTypes()` | List all registered node types (grouped by category order) |
| `qmlCategories()` | List registered categories in registration order |
| `qmlNodesInCategory(categoryId)` | List node types belonging to a category |
| `qmlSaveToFile(path)` | Save graph to a file on disk (C++ QFile, no XMLHttpRequest) |
| `qmlLoadFromFile(path)` | Load graph from a file on disk |
| `clear()` | Remove all nodes and edges |

## Port Types

| ID | Name | Color |
|---|---|---|
| 0 | Int | `#4A9EFF` |
| 1 | Float | `#4CDF8B` |
| 2 | String | `#FF9F43` |
| 3 | Bool | `#FF6B6B` |
| 4 | Color | `#A29BFE` |
| 5 | Generic | `#888888` |
| 6 | Double | `#F0DB4F` |
| 7-9 | Vec2/3/4 | `#E17055` |
| 10-12 | Array/Map/JSON | `#00CEC9` |
| 13 | Image | `#FD79A8` |
| 14 | AudioBuffer | `#6C5CE7` |

## QML Components

### NodeEditorWorkspace
The main workspace component. Self-contained with toolbar, canvas, properties panel, and tab bar.

**Properties**: `graphModel`, `undoManager`, `showTopBar`, `showBottomBar`, `framelessWindow`

### Embedding / Configuration

`NodeEditorWorkspace` exposes three boolean properties for embedding as a package:

| Property | Default | Description |
|---|---|---|
| `showTopBar` | `true` | Show/hide the top toolbar (File, Add, Fit, Zoom, Undo/Redo, Compute) |
| `showBottomBar` | `true` | Show/hide the bottom tab bar (tabs, add tab, node count) |
| `framelessWindow` | `false` | When `true`, sets parent window flags to `Qt.Window \| Qt.FramelessWindowHint` |

All keyboard shortcuts remain active regardless of bar visibility.

```qml
// Example: embed with only the canvas (no bars, no window frame)
NodeEditorWorkspace {
    anchors.fill: parent
    showTopBar: false
    showBottomBar: false
    framelessWindow: true
}
```

### NodeCanvas
The node graph canvas. Handles pan, zoom, selection, connections, and drag-drop.

**Properties**: `graphModel`, `undoManager`, `zoom`, `panX`, `panY`, `selectedNodeId`, `selectedNodeIds`, `selectMode`

### AddNodePopup
Searchable popup for adding nodes by type.

**Properties**: `graphModel`, `undoManager`

### Node
Visual representation of a graph node. Input TextFields become read-only when the port is connected to an output. Output values are displayed in green. **Display nodes** (`output/display/*`) render their output inline inside the node body (images for charts/gauges/LED matrix, formatted text for numbers/vectors).

**Properties**: `graphModel`, `undoManager`, `nodeId`, `nodeInfo`, `selected`

### Edge
Visual connection between two ports (straight line + arrow head).

**Properties**: `graphModel`, `undoManager`, `edgeId`, `sourceNodeId`, `sourcePort`, `targetNodeId`, `targetPort`

### Port
Visual connection point on a node. 15 port types with distinct colors.

**Properties**: `graphModel`, `nodeId`, `portName`, `isInput`, `portType`

### PropertiesPanel
Shows selected node's properties and data fields.

**Properties**: `graphModel`, `undoManager`, `nodeId`

### NodePalette
Sidebar list of node types for drag-and-drop.

**Properties**: `graphModel`, `undoManager`, `nodeTypes`, `colorMap`
