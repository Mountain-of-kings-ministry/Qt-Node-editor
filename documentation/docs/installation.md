# Installation

## Requirements

- Qt 6.11+
- C++20 compatible compiler (GCC 11+, Clang 14+, MSVC 2022+)
- CMake 3.28+
- Ninja (recommended)

## Building from Source

```bash
# Clone or navigate to the project
cd nodeapackage

# Configure
cmake --preset debug

# Build
cmake --build --preset debug

# Run tests
./build/debug/tests/TestGraphModel
./build/debug/tests/TestDataFlowEngine
```

Available presets: `debug`, `release`.

## Integrating into Your Project

Add the NodeEditor subdirectory to your CMakeLists.txt:

```cmake
add_subdirectory(path/to/NodeEditor)

# ...

# For C++ access
target_link_libraries(your_target PRIVATE NodeEditorCore)

# For QML access
target_link_libraries(your_target PRIVATE NodeEditor)
```

Then register the types in your main.cpp:

```cpp
#include <NodeEditor/GraphModel.h>
#include <NodeEditor/UndoManager.h>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    NodeEditor::GraphModel *graphModel = new NodeEditor::GraphModel(&engine);
    NodeEditor::UndoManager *undoManager = new NodeEditor::UndoManager(graphModel, &engine);
    DefaultNodeRegistry::registerAll(graphModel);

    engine.rootContext()->setContextProperty("_graphModel", graphModel);
    engine.rootContext()->setContextProperty("_undoManager", undoManager);

    engine.loadFromModule("yourapp", "Main");
    return app.exec();
}
```

## QML Usage

```qml
import NodeEditor

NodeEditorWorkspace {
    anchors.fill: parent
    graphModel: _graphModel
    undoManager: _undoManager
}
```
