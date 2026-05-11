# Getting Started

## Your First Node Graph

1. Launch the demo application.
2. Press **Shift+A** to open the Add Node popup.
3. Type "add" to filter for math nodes, then click one to add it.
4. Add a few more nodes (e.g., Input, Multiply, Output).
5. **Connect nodes** by clicking and dragging from an output port (right side) to an input port (left side).
6. **Edit values** by typing directly into node input fields or the Properties panel.
7. Press **F** to fit all nodes into view.

## Workspace Overview

```
┌──────────────────────────────────────────┐
│ File  | Add | Fit | - 100% + | Select  │  ← Top bar
│ Undo  Redo                     Compute │
├──────────────────────────────────────────┤
│                                          │
│   [Input] ──→ [Add] ──→ [Output]        │  ← Canvas
│                                          │
├──────────────────────────────────────────┤
│ Canvas 1 | +                  3 nodes   │  ← Bottom bar
└──────────────────────────────────────────┘
```

## Core Concepts

### Nodes
Each node has a colored header (drag handle), input ports on the left, output ports on the right, and optional data fields. Nodes execute a specific operation.

### Ports
Ports are typed connections between nodes. Each port has a color indicating its data type:
- **Blue** - Int
- **Green** - Float
- **Orange** - String
- **Red** - Bool
- **Purple** - Color
- **Gray** - Generic
- **Yellow** - Double
- **Teal** - Array / Map / JSON
- **Pink** - Image
- **Violet** - AudioBuffer

### Connections
Edges carry data from output ports to input ports. Right-click an edge to delete it.

### Data Flow
The engine automatically propagates data changes through the graph. When you edit a node's input, all downstream nodes recompute.

### Display Nodes (Inline Preview)
Output display nodes (`output/display/*`) render their computed output visually inside the node body:
- **Text-based** (Number, Text, Vector, Matrix, Memory, FPS): formatted text shown in a monospace area
- **Image-based** (LED Matrix, Line Plot, Bar Chart, Pie Chart, Histogram, Heatmap, Progress, Gauge, Compass, Boolean): rendered QImage shown inside the node

Display nodes do not show raw output values in the Properties panel — the inline preview replaces that.

## Embedding as a Package

`NodeEditorWorkspace` exposes three boolean properties for embedding in other projects:

| Property | Default | Description |
|---|---|---|
| `showTopBar` | `true` | Show/hide the top toolbar |
| `showBottomBar` | `true` | Show/hide the bottom tab bar |
| `framelessWindow` | `false` | Remove window decorations |

All keyboard shortcuts remain active regardless of bar visibility.

```qml
NodeEditorWorkspace {
    anchors.fill: parent
    showTopBar: false
    showBottomBar: false
}
```
