qt_add_executable(TimeDisplayApp
    main.cpp
    TimeDisplayNodes.h
)

qt_add_qml_module(TimeDisplayApp
    URI "TimeDisplay"
    VERSION 1.0
    QML_FILES
        Main.qml
)

target_link_libraries(TimeDisplayApp PRIVATE NodeEditor Qt6::Quick Qt6::Qml)

set_target_properties(TimeDisplayApp PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.timedisplay
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS TimeDisplayApp
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
