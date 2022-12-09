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
)
list(TRANSFORM SRC_FILES PREPEND "${PROJECT_SOURCE_DIR}/src/")
add_executable(imgcrop ${SRC_FILES})

target_include_directories(imgcrop PRIVATE "${PROJECT_SOURCE_DIR}/include")

# Magick++
# Find where Magick++-config lives
find_program(MAGICK_CONFIG "Magick++-config")
# Ask about CXX flags
execute_process(COMMAND "${MAGICK_CONFIG}" "--cxxflags" OUTPUT_VARIABLE MAGICK_CXX_FLAGS)
string(STRIP "${MAGICK_CXX_FLAGS}" MAGICK_CXX_FLAGS)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MAGICK_CXX_FLAGS}")
find_package(ImageMagick COMPONENTS Magick++)
target_include_directories(imgcrop PRIVATE ${ImageMagick_INCLUDE_DIRS})
target_link_libraries(imgcrop ${ImageMagick_LIBRARIES})

# wxWidgets
find_package(wxWidgets REQUIRED COMPONENTS net core base)
include(${wxWidgets_USE_FILE})
target_link_libraries(imgcrop ${wxWidgets_LIBRARIES})
