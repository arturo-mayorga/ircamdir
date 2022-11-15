cmake_minimum_required(VERSION 3.14)
project(my_project)

# GoogleTest requires at least C++17
set(CMAKE_CXX_STANDARD 17)


include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)

FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  GIT_TAG "dfdbe1eecf0451e0da5420c4b9c39ecb415681ac"
  # GIT_TAG v3.0.0
)

FetchContent_GetProperties(ftxui)
if(NOT ftxui_POPULATED)
  FetchContent_Populate(ftxui)
  add_subdirectory(${ftxui_SOURCE_DIR} ${ftxui_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

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
    src/txt-view/car-event-log-table.cpp
    src/txt-view/tv-driver-table.cpp
    src/txt-view/simple-label.cpp
    src/systems/irtelemetry-sys.cpp
    src/systems/overtake-detector-system.cpp
    src/systems/incident-detector-system.cpp
    src/systems/closest-battle-director-sys.cpp
    src/systems/broadcast-car-info-collector-sys.cpp 
    src/systems/tui-sys.cpp
    src/systems/broadcast-summary-sys.cpp
    src/systems/tv-point-director-sys.cpp
    src/systems/head-of-direction-sys.cpp
)

TARGET_LINK_LIBRARIES(camdir
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component 
)

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
