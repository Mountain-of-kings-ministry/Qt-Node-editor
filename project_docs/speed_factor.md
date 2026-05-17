All 5 phases complete. Here's the full summary of everything built:

**Phase 1 — Core Data Layer**
- 3-layer separation (`GraphNode` / `NodeRuntime` / `NodeUIState`)
- `uint64_t` IDs with `NodeID`/`EdgeID` typedefs
- `QMutex`-protected runtime data (`setNodeData`/`nodeData`)
- `QHash`-based O(1) lookups
- RapidJSON serialization (`qmlSerializeToJson`/`qmlDeserializeFromJson`)
- Topology cache with Kahn's algorithm (`cachedTopologicalOrder()` / `topologicalLevels()`)
- Dirty DAG tracking with downstream propagation

**Phase 2 — Async Evaluation Engine**
- `AsyncEvaluator` — snapshot-based async eval with `QThreadPool` + `QtConcurrent::run`
- Topological-level parallelism (nodes at same DAG depth run in parallel)
- `evaluateDirty(dirtyNodes)` / `evaluateFrom(nodeId, downstream)`
- `DataFlowEngine::requestValue(nodeId, port)` — pull-based evaluation (only computes upstream subgraph needed for a port)
- Thread-safe node instance cache

**Phase 3 — RapidJSON Serialization** (done within Phase 1)

**Phase 4 — Preview Manager**
- `PreviewManager` with LRU cache by `(nodeId, evalVersion)`, mutex-protected
- `renderBudget` (default 3) — limits previews per `processRequests()` call
- `maxCacheSize` (default 50) with LRU eviction
- Auto-wires to `GraphModel::nodeDataChanged` for caching
- `requestPreview()` / `previewData()` / `hasPreview()` QML API
- QML integration: `previewManager` property in `NodeEditorWorkspace` → `NodeCanvas` → `Node`

**Phase 5 — Display Node Split**
- `BaseNode::render(inputs, maxSize)` — virtual method returning `QImage` (null = unsupported)
- `BaseNode::renderToBase64(inputs, maxSize)` — render + encode helper
- `render()` overrides in 11 display node classes (LED matrix, charts, gauges, boolean, compass, progress bar, heatmap)
- Each overridden `compute()` now calls `render()` then encodes
- `PreviewManager::renderNode(nodeId)` — direct off-thread render via `QtConcurrent`
- `imageToBase64` helper consolidated

**All 17 tests pass** across GraphModel (12) and DataFlowEngine (5).