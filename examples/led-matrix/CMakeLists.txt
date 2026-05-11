qt_add_executable(LedMatrixApp
    main.cpp
)

qt_add_qml_module(LedMatrixApp
    URI "LedMatrix"
    VERSION 1.0
    QML_FILES
        Main.qml
)

target_link_libraries(LedMatrixApp PRIVATE NodeEditor Qt6::Quick Qt6::Qml)

set_target_properties(LedMatrixApp PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.ledmatrix
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS LedMatrixApp
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
