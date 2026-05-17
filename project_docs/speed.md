
For a high-performance node editor with inline rendering (especially for your MotionEditor project), the biggest mistake is storing *render state* together with *graph state*.

The most efficient architecture is:

# 1. Separate the System Into 3 Layers

## A. Graph Data (persistent)

Very lightweight.

Store only:

```cpp
struct Node {
    uint64_t id;
    NodeType type;

    SmallVector<InputPin, 8> inputs;
    SmallVector<OutputPin, 8> outputs;

    uint32_t flags;
};
```

Connections:

```cpp
struct Edge {
    PinID from;
    PinID to;
};
```

NO:

* textures
* framebuffers
* decoded images
* GPU handles
* timeline caches
* UI state

This layer should serialize instantly.

---

## B. Runtime Evaluation Cache

Transient.

This contains:

* decoded frames
* GPU textures
* waveform cache
* thumbnails
* shader outputs
* compiled expressions

This is rebuilt as needed.

Use:

* arena allocators
* frame allocators
* object pools
* LRU caches

---

## C. UI State

Separate entirely.

```cpp
struct NodeUIState {
    Vec2 pos;
    bool selected;
    bool collapsed;
    float previewZoom;
};
```

Do NOT pollute graph data with editor state.

---

# 2. Use Stable IDs Instead of Pointers

Pointers become expensive and dangerous when graphs mutate.

Use:

```cpp
using NodeID = uint64_t;
using PinID  = uint64_t;
```

Then store nodes in:

```cpp
ankerl::unordered_dense::map<NodeID, Node>
```

or:

```cpp
std::vector<Node>
```

with ID indexing.

This dramatically improves:

* cache locality
* undo/redo
* serialization
* multithreading

---

# 3. Never Recompute Entire Graphs

This is where most editors hang.

Use a dirty propagation system.

Example:

```txt
ImageLoader
    ↓
ColorCorrect
    ↓
Blur
    ↓
Viewer
```

If only Blur changes:

* DO NOT rerun ImageLoader
* DO NOT rerun ColorCorrect

Only recompute downstream invalidated nodes.

---

# 4. Use Pull Evaluation Instead of Push

Bad:

```txt
Node updates → propagates to everything
```

Good:

```txt
Viewer requests frame
↓
Dependency graph resolves only required nodes
```

This is how:

* Blender compositor
* Unreal materials
* Resolve Fusion
* Nuke

avoid freezing.

---

# 5. Inline Rendering Must Be Virtualized

This is critical.

Do NOT render previews for all visible nodes every frame.

Instead:

## Render previews only when:

* node is visible
* node changed
* zoom level allows it
* preview is requested

Use:

```txt
PreviewManager
```

with:

* render budget per frame
* async jobs
* texture reuse

Example:

```txt
Frame budget:
2 preview renders/frame
```

This alone removes UI hitching.

---

# 6. GPU Resource Pooling

Never create/destroy textures every frame.

Use pools:

```cpp
TexturePool.acquire(width, height, format);
TexturePool.release(tex);
```

GPU allocations are extremely expensive.

Pooling gives massive performance gains.

---

# 7. Chunked Timeline Storage

For timeline-heavy editors:

Store clips in chunks/pages.

Instead of:

```txt
Huge vector of clips
```

Use:

```txt
Track
 ├── Chunk 0
 ├── Chunk 1
 ├── Chunk 2
```

Benefits:

* partial loading
* cache efficiency
* fast seek
* scalable undo

Especially important for:

* thousands of clips
* long timelines
* procedural tracks

---

# 8. Binary Serialization, Not JSON

JSON becomes a bottleneck quickly.

Use:

* FlatBuffers
* Cap’n Proto
* MessagePack
* custom binary

Ideal architecture:

```txt
project.mokm
 ├── graph.bin
 ├── ui.bin
 ├── cache/
 ├── thumbnails/
 └── media/
```

This is how professional editors avoid startup lag.

---

# 9. Job System + Background Evaluation

Never evaluate nodes on the UI thread.

Use:

* task graph
* worker threads
* dependency scheduler

UI thread should ONLY:

* draw
* handle input
* issue render commands

Everything else async.

---

# 10. Incremental Rendering

For inline previews:

Do not render full resolution.

Use:

* quarter res
* adaptive scaling
* temporal preview caching

Example:

```txt
Idle:
render 512x512

Dragging:
render 128x128

Stopped:
refine to 512x512
```

Huge responsiveness improvement.

---

# Recommended Core Architecture

For your editor specifically:

```txt
Graph Core
    ↓
Dependency Evaluator
    ↓
Task Scheduler
    ↓
GPU Resource Cache
    ↓
Preview Renderer
    ↓
ImGui/Qt UI
```

---

# Biggest Performance Killers

These cause node editors to hang:

## BAD

* storing textures inside nodes
* evaluating every node every frame
* recursive graph traversal
* synchronous thumbnail generation
* reallocating vectors constantly
* GPU creation/destruction loops
* giant JSON saves
* UI thread decoding media

---

# Best Overall Strategy

The most scalable design is:

```txt
Persistent Graph
+ Runtime Cache
+ Async Evaluation
+ GPU Pooling
+ Dirty Propagation
+ Preview Virtualization
```

That architecture scales to:

* huge timelines
* realtime previews
* thousands of nodes
* procedural rendering
* compositing graphs

without freezing the editor.


https://rapidjson.org/
