# Core C++ library (separate from QML module to avoid AUTOMOC conflicts)
qt_add_library(NodeEditorCore STATIC
    src/backend/GraphModel.cpp
    src/backend/DataFlowEngine.cpp
    src/backend/AsyncEvaluator.cpp
    src/backend/PreviewManager.cpp
    src/backend/UndoManager.cpp
    src/nodes/BaseNode.cpp
    src/nodes/DefaultNodes.cpp
    include/NodeEditor/GraphModel.h
    include/NodeEditor/DataFlowEngine.h
    include/NodeEditor/AsyncEvaluator.h
    include/NodeEditor/PreviewManager.h
    include/NodeEditor/UndoManager.h
    include/NodeEditor/BaseNode.h
    include/NodeEditor/DefaultNodes.h
    src/nodes/system/SystemNodes.h
    src/nodes/math/MathNodes.h
    src/nodes/color/ColorNodes.h
    src/nodes/data/DataNodes.h
    src/nodes/logic/LogicNodes.h
    src/nodes/events/EventNodes.h
    src/nodes/generators/GeneratorNodes.h
    src/nodes/output/OutputNodes.h
    src/nodes/qt/QtNodes.h
    src/nodes/utility/UtilityNodes.h
)

target_include_directories(NodeEditorCore PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/third_parties/rapidjson/include
)

target_link_libraries(NodeEditorCore PUBLIC
    Qt6::Core Qt6::Gui Qt6::Concurrent
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
