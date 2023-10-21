#CROSS_COMPILE ?= /opt/linaro-7.5.0/gcc-linaro-7.5.0-2019.12-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-
#export PATH="/opt/linaro-7.5.0/gcc-linaro-7.5.0-2019.12-x86_64_aarch64-linux-gnu/bin":$PATH

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++)
set(CMAKE_FIND_ROOT_PATH /opt/linaro-7.5.0/gcc-linaro-7.5.0-2019.12-x86_64_aarch64-linux-gnu ${CMAKE_CURRENT_SOURCE_DIR}/../install)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
