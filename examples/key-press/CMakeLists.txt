qt_add_executable(KeyPressApp
    main.cpp
    KeyPressNodes.h
)

qt_add_qml_module(KeyPressApp
    URI "KeyPress"
    VERSION 1.0
    QML_FILES
        Main.qml
)

target_link_libraries(KeyPressApp PRIVATE NodeEditor Qt6::Quick Qt6::Qml)

set_target_properties(KeyPressApp PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.keypress
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS KeyPressApp
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
