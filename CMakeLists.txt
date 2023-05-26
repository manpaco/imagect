cmake_minimum_required(VERSION 3.12.4)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

project(ImageCT)

execute_process(COMMAND "bash" "-c" "git describe --tags" OUTPUT_VARIABLE LATEST_TAG)

string(STRIP ${LATEST_TAG} LATEST_TAG)

string(REGEX REPLACE "^v([0-9]+).*" "\\1" VERSION_MAJOR "${LATEST_TAG}")
string(REGEX REPLACE "^v[0-9]+\\.([0-9]+).*" "\\1" VERSION_MINOR "${LATEST_TAG}")
string(REGEX REPLACE "^v[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" VERSION_PATCH "${LATEST_TAG}")
string(REGEX REPLACE "^v[0-9]+\\.[0-9]+\\.[0-9]+(.*)" "\\1" VERSION_SUFFIX "${LATEST_TAG}")

set(VERSION_SHORT "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

configure_file(${PROJECT_SOURCE_DIR}/buildvalues.h.in
               ${PROJECT_SOURCE_DIR}/include/buildvalues.h @ONLY)

list(APPEND SRC_FILES
    "imagectapp.cpp"
    "mainframe.cpp"
    "previewpanel.cpp"
    "toolspanel.cpp"
    "canvaspanel.cpp"
    "imgtools.cpp"
    "cropevent.cpp"
    "imagewindow.cpp"
    "filext.cpp"
    "exportdlg.cpp"
    "controller.cpp"
    "scrolview.cpp"
    "zoomctrl.cpp"
    "zoomevent.cpp"
)
list(TRANSFORM SRC_FILES PREPEND "${PROJECT_SOURCE_DIR}/src/")

if(CMAKE_SYSTEM MATCHES Windows)
    list(APPEND RCS_FILES
        "wimagect.rc"
    )
    list(TRANSFORM RCS_FILES PREPEND "${PROJECT_SOURCE_DIR}/win/")
endif()

add_executable(imagect ${SRC_FILES} ${RCS_FILES})

target_include_directories(imagect PRIVATE "${PROJECT_SOURCE_DIR}/include")

# wxWidgets
find_package(wxWidgets REQUIRED COMPONENTS net core base)
include(${wxWidgets_USE_FILE})
target_link_libraries(imagect ${wxWidgets_LIBRARIES})
