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
    src/irsdk/irsdk_client.cpp
    src/irsdk/irsdk_diskclient.cpp
    src/irsdk/irsdk_utils.cpp
    src/irsdk/yaml_parser.cpp
    src/systems/irtelemetry-sys.cpp
    src/systems/closest-battle-director-sys.cpp
    src/systems/screen-time-stats-sys.cpp 
    src/systems/tui-sys.cpp
    src/systems/console-kb-sys.cpp
    src/systems/broadcast-summary-sys.cpp
    src/systems/tv-point-director-sys.cpp
    src/systems/head-of-direction-sys.cpp
)

TARGET_LINK_LIBRARIES(camdir)

add_executable(
    ir2stdout
    src/irsdk/examples/ir2stdout.cpp
    src/irsdk/irsdk_client.cpp
    src/irsdk/irsdk_diskclient.cpp
    src/irsdk/irsdk_utils.cpp
    src/irsdk/yaml_parser.cpp
)

TARGET_LINK_LIBRARIES(ir2stdout)

add_executable(
    irsendmsg
    src/irsdk/examples/irsendmsg.cpp
    src/irsdk/irsdk_client.cpp
    src/irsdk/irsdk_diskclient.cpp
    src/irsdk/irsdk_utils.cpp
    src/irsdk/yaml_parser.cpp
)

TARGET_LINK_LIBRARIES(irsendmsg)

include(GoogleTest)
gtest_discover_tests(hello_test)
