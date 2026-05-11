qt_add_executable(TestGraphModel
    tst_GraphModel.cpp
)

target_link_libraries(TestGraphModel PRIVATE
    NodeEditor
    Qt6::Test
)

add_test(NAME TestGraphModel COMMAND TestGraphModel)


qt_add_executable(TestDataFlowEngine
    tst_DataFlowEngine.cpp
)

target_link_libraries(TestDataFlowEngine PRIVATE
    NodeEditor
    Qt6::Test
)

add_test(NAME TestDataFlowEngine COMMAND TestDataFlowEngine)
