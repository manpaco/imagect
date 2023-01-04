cmake_minimum_required(VERSION 3.12.4)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

project(ImageCroppingTool)

list(APPEND SRC_FILES
    "imgcropapp.cpp"
    "mainframe.cpp"
    "previewpanel.cpp"
    "toolspanel.cpp"
    "canvaspanel.cpp"
    "imgtools.cpp"
    "rectangle.cpp"
    "cropevent.cpp"
    "imagewindow.cpp"
    "filext.cpp"
    "exportdlg.cpp"
    "controller.cpp"
    "scrolview.cpp"
)
list(TRANSFORM SRC_FILES PREPEND "${PROJECT_SOURCE_DIR}/src/")
add_executable(imgcrop ${SRC_FILES})

target_include_directories(imgcrop PRIVATE "${PROJECT_SOURCE_DIR}/include")

# Magick++
# Set search strings
set(DISH "HDRI_ENABLE=0")
set(LOWD "QUANTUM_DEPTH=8")
# Ask about CXX flags
execute_process(COMMAND "bash" "-c" "Magick++-config --cxxflags" OUTPUT_VARIABLE MAGICK_CXX_FLAGS)
string(STRIP "${MAGICK_CXX_FLAGS}" MAGICK_CXX_FLAGS)
# Find positions in flags
string(FIND "${MAGICK_CXX_FLAGS}" "${DISH}" POSDH)
string(FIND "${MAGICK_CXX_FLAGS}" "${LOWD}" POSLD)

if(POSDH LESS 0)
    add_definitions( -DMAGICKCORE_HDRI_ENABLE=1 )
else()
    add_definitions( -DMAGICKCORE_HDRI_ENABLE=0 )
endif()

if(POSLD LESS 0)
    add_definitions( -DMAGICKCORE_QUANTUM_DEPTH=16 )
else()
    add_definitions( -DMAGICKCORE_QUANTUM_DEPTH=8 )
endif()

find_package(ImageMagick REQUIRED COMPONENTS Magick++ MagickWand MagickCore)
target_include_directories(imgcrop PRIVATE ${ImageMagick_INCLUDE_DIRS})
target_link_libraries(imgcrop ${ImageMagick_LIBRARIES})

# wxWidgets
find_package(wxWidgets REQUIRED COMPONENTS net core base)
include(${wxWidgets_USE_FILE})
target_link_libraries(imgcrop ${wxWidgets_LIBRARIES})
