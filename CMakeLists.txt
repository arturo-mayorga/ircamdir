cmake_minimum_required(VERSION 3.14)
project(my_project)

# GoogleTest requires at least C++17
set(CMAKE_CXX_STANDARD 17)


include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()



add_executable(
  hello_test
  src/gpad.cpp
)

SET( LIBS XInput DSound gtest_main )
TARGET_LINK_LIBRARIES(hello_test ${LIBS} )

add_executable(
    camdir
    src/llgame-main.cpp
    src/irsdk_client.cpp
    src/irsdk_diskclient.cpp
    src/irsdk_utils.cpp
    src/yaml_parser.cpp
    src/irtelemetry-sys.cpp
    src/battle-detect-sys.cpp
    src/screen-time-stats-sys.cpp
)

TARGET_LINK_LIBRARIES(camdir)

add_executable(
    ir2stdout
   src/ir2stdout.cpp
    src/irsdk_client.cpp
    src/irsdk_diskclient.cpp
    src/irsdk_utils.cpp
    src/yaml_parser.cpp
)

TARGET_LINK_LIBRARIES(ir2stdout)

add_executable(
    irsendmsg
   src/irsendmsg.cpp
    src/irsdk_client.cpp
    src/irsdk_diskclient.cpp
    src/irsdk_utils.cpp
    src/yaml_parser.cpp
)

TARGET_LINK_LIBRARIES(irsendmsg)

include(GoogleTest)
gtest_discover_tests(hello_test)
