project (PolicyKit-example)
cmake_minimum_required(VERSION 2.6.0)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/../cmake/modules")

find_package(Qt4 REQUIRED)
include( ${QT_USE_FILE} )
find_package (PolkitQt)

if (POLKITQT_FOUND)
    include_directories(
      ${POLKITQT_INCLUDE_DIR}
      ${CMAKE_BINARY_DIR}
    )

    set(polkit_example_SRCS
        main.cpp
        PkExample.cpp
    )

    QT4_WRAP_CPP(polkit_example_MOC_SRCS
        PkExample.h
    )

    QT4_WRAP_UI(polkit_example_UI_SRCS
        PkExample.ui
    )

    add_executable(polkit-example
        ${polkit_example_SRCS}
        ${polkit_example_MOC_SRCS}
        ${polkit_example_UI_SRCS}
    )

    target_link_libraries(polkit-example ${QT_LIBRARIES} polkit-qt)

endif (POLKITQT_FOUND)

