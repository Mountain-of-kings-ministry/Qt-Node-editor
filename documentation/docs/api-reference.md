# API Reference

## C++ Classes

### GraphModel
`include/NodeEditor/GraphModel.h`

The central data model for a node graph. Manages nodes, edges, data, and topological order.

| Method | Description |
|---|---|
| `addNode(type, position, existingId)` | Add a node of the given type |
| `removeNode(nodeId)` | Remove a node and its edges |
| `connectPorts(src, srcPort, tgt, tgtPort)` | Create an edge between ports |
| `disconnectEdge(edgeId)` | Remove an edge |
| `setNodeData(nodeId, key, value)` | Set a node's data value |
| `nodeData(nodeId, key)` | Get a node's data value |
| `topologicalSort()` | Return nodes in dependency order |
| `hasCycles()` | Check for cycles in the graph |
| `registerNodeType(type, info)` | Register a node type |
| `registerCategory(category)` | Register a node category |
| `clear()` | Remove all nodes and edges |

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
| `inputSpec()` | List of input PortInfo |
| `outputSpec()` | List of output PortInfo |
| `compute(inputs, outputs)` | Execute node logic |

### DataFlowEngine
`include/NodeEditor/DataFlowEngine.h`

Propagates data through the graph. Listens to `GraphModel` signals.

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

**Properties**: `graphModel`, `undoManager`

### NodeCanvas
The node graph canvas. Handles pan, zoom, selection, connections, and drag-drop.

**Properties**: `graphModel`, `undoManager`, `zoom`, `panX`, `panY`, `selectedNodeId`, `selectedNodeIds`, `selectMode`

### AddNodePopup
Searchable popup for adding nodes by type.

**Properties**: `graphModel`, `undoManager`

### Node
Visual representation of a graph node.

**Properties**: `graphModel`, `undoManager`, `nodeId`, `nodeInfo`, `selected`

### Edge
Visual connection between two ports.

**Properties**: `graphModel`, `undoManager`, `edgeId`, `sourceNodeId`, `sourcePort`, `targetNodeId`, `targetPort`

### Port
Visual connection point on a node.

**Properties**: `graphModel`, `nodeId`, `portName`, `isInput`, `portType`

### PropertiesPanel
Shows selected node's properties and data fields.

**Properties**: `graphModel`, `undoManager`, `nodeId`

### NodePalette
Sidebar list of node types for drag-and-drop.

**Properties**: `graphModel`, `undoManager`, `nodeTypes`, `colorMap`
