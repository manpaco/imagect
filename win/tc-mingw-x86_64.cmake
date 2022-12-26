# the name of the target operating system
set(CMAKE_SYSTEM_NAME Windows)

# which compilers to use for C and C++
set(CMAKE_C_COMPILER mingw-w64-ucrt-x86_64-gcc)
set(CMAKE_CXX_COMPILER mingw-w64-ucrt-x86_64-gcc)

message("$ENV{HOME}")

# where is the target environment located
set(CMAKE_FIND_ROOT_PATH "$ENV{HOME}/imgcrop")

# adjust the default behavior of the FIND_XXX() commands:
# search programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# search headers and libraries in the target environment
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
