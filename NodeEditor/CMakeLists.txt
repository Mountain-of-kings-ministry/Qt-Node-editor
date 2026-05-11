# Core C++ library (separate from QML module to avoid AUTOMOC conflicts)
qt_add_library(NodeEditorCore STATIC
    src/backend/GraphModel.cpp
    src/backend/DataFlowEngine.cpp
    src/backend/UndoManager.cpp
    src/nodes/BaseNode.cpp
    src/nodes/DefaultNodes.cpp
    include/NodeEditor/GraphModel.h
    include/NodeEditor/DataFlowEngine.h
    include/NodeEditor/UndoManager.h
    include/NodeEditor/BaseNode.h
    include/NodeEditor/DefaultNodes.h
)

target_include_directories(NodeEditorCore PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(NodeEditorCore PUBLIC
    Qt6::Core Qt6::Gui
)

# QML module wrapping the core library
qt_add_qml_module(NodeEditor
    URI "NodeEditor"
    VERSION 1.0
    QML_FILES
        qml/NodeCanvas.qml
        qml/Node.qml
        qml/Edge.qml
        qml/Port.qml
        qml/NodePalette.qml
        qml/PropertiesPanel.qml
        qml/NodeEditorWorkspace.qml
        qml/AddNodePopup.qml
)

target_link_libraries(NodeEditor PUBLIC NodeEditorCore)
target_link_libraries(NodeEditor PRIVATE Qt6::Qml Qt6::Quick)
