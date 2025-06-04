cmake_minimum_required(VERSION 3.16)

project(Client VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Quick QuickControls2)

qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(appClient
    main.cpp
    NetworkManager.cpp

)

qt_add_qml_module(appClient.Components
    URI Client.Components
    VERSION 1.0
    DEPENDENCIES
        QtQuick
        QtQuick.Controls
    QML_FILES
        qml/Components/AppHeader.qml
        qml/Components/CloseButton.qml
        qml/Components/CustomTextField.qml
        qml/Components/FloatingLogo.qml
        qml/Components/CustomButton.qml
        qml/Components/LoginForm.qml
        qml/Components/MainBackground.qml
        qml/Components/ParticlesRepeater.qml
        qml/Components/PrimaryButton.qml
        qml/Components/RegisterForm.qml
        qml/Components/TextLink.qml
    RESOURCES
        resources.qrc


    OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Client/Components
    QML_FILES qml/Components/AppHeader.qml
)

qt_add_qml_module(appClient.Pages
    URI Client.Pages
    VERSION 1.0
    DEPENDENCIES
        Client.Components
    QML_FILES
        qml/Pages/LoginPage.qml
        qml/Pages/Main.qml
        qml/Pages/ProfileWindow.qml
        qml/Pages/RegisterPage.qml


    OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Client/Pages
)

qt_add_qml_module(appClient
    URI Client
    VERSION 1.0


)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appClient PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appClient
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appClient
    PRIVATE Qt6::Quick Qt6::QuickControls2 appClient.Components appClient.Pages
)

include(GNUInstallDirs)
install(TARGETS appClient
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
