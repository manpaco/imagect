cmake_minimum_required(VERSION 3.25.1)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(PROJECT "imagect")
project(${PROJECT})

execute_process(COMMAND "bash" "-c" "git describe --tags" OUTPUT_VARIABLE LATEST_TAG)

string(STRIP ${LATEST_TAG} LATEST_TAG)

string(REGEX REPLACE "^v([0-9]+).*" "\\1" VERSION_MAJOR "${LATEST_TAG}")
string(REGEX REPLACE "^v[0-9]+\\.([0-9]+).*" "\\1" VERSION_MINOR "${LATEST_TAG}")
string(REGEX REPLACE "^v[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" VERSION_PATCH "${LATEST_TAG}")
string(REGEX REPLACE "^v[0-9]+\\.[0-9]+\\.[0-9]+(.*)" "\\1" VERSION_SUFFIX "${LATEST_TAG}")
if(${VERSION_SUFFIX})
    set(VERSION_SUFFIX "-dirty")
endif()

set(VERSION_SHORT "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

configure_file(${PROJECT_SOURCE_DIR}/buildvalues.hpp.in
               ${PROJECT_SOURCE_DIR}/include/buildvalues.hpp @ONLY)

list(APPEND SRC_FILES
    "imagectapp.cpp"
    "mainframe.cpp"
    "previewpanel.cpp"
    "toolspanel.cpp"
    "cropevent.cpp"
    "imagewindow.cpp"
    "filext.cpp"
    "exportdlg.cpp"
    "zoomctrl.cpp"
    "zoomevent.cpp"
    "scrolledcanvas.cpp"
    "canvasitem.cpp"
    "scaler.cpp"
)
list(TRANSFORM SRC_FILES PREPEND "${PROJECT_SOURCE_DIR}/src/")

if(CMAKE_SYSTEM MATCHES Windows)
    list(APPEND RCS_FILES
        "wimagect.rc"
    )
    list(TRANSFORM RCS_FILES PREPEND "${PROJECT_SOURCE_DIR}/win/")
endif()

add_executable(${PROJECT} ${SRC_FILES} ${RCS_FILES})

target_include_directories(${PROJECT} PRIVATE "${PROJECT_SOURCE_DIR}/include")

# wxWidgets
find_package(wxWidgets REQUIRED COMPONENTS net core base)
include(${wxWidgets_USE_FILE})
target_link_libraries(${PROJECT} ${wxWidgets_LIBRARIES})
