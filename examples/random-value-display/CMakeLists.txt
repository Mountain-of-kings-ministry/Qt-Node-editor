qt_add_executable(RandomValueDisplayApp
    main.cpp
    RandomValueNodes.h
)

qt_add_qml_module(RandomValueDisplayApp
    URI "RandomValueDisplay"
    VERSION 1.0
    QML_FILES
        Main.qml
)

target_link_libraries(RandomValueDisplayApp PRIVATE NodeEditor Qt6::Quick Qt6::Qml)

set_target_properties(RandomValueDisplayApp PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.randomvaluedisplay
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS RandomValueDisplayApp
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
