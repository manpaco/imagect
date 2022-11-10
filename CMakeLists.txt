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
	"rectangleevent.cpp"
)
list(TRANSFORM SRC_FILES PREPEND "${PROJECT_SOURCE_DIR}/src/")
add_executable(imgcrop ${SRC_FILES})

target_include_directories(imgcrop PRIVATE "${PROJECT_SOURCE_DIR}/include")

# Magick++
add_definitions( -DMAGICKCORE_QUANTUM_DEPTH=16 )
add_definitions( -DMAGICKCORE_HDRI_ENABLE=0 )
find_package(ImageMagick COMPONENTS Magick++)
target_include_directories(imgcrop PRIVATE ${ImageMagick_INCLUDE_DIRS})
target_link_libraries(imgcrop ${ImageMagick_LIBRARIES})

# wxWidgets
find_package(wxWidgets REQUIRED COMPONENTS net core base)
include(${wxWidgets_USE_FILE})
target_link_libraries(imgcrop ${wxWidgets_LIBRARIES})
