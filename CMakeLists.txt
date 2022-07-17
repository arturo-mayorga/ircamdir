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

find_package(Vulkan REQUIRED)



add_executable(
  hello_test
  src/gpad.cpp
)

SET( LIBS XInput DSound gtest_main )
TARGET_LINK_LIBRARIES(hello_test ${LIBS} )

add_executable(
    llgame
    src/llgame-main.cpp
    src/gamepad-sys.cpp
    src/window-sys.cpp
    src/vulkan-loader.cpp
)

TARGET_LINK_LIBRARIES(llgame XInput Vulkan::Vulkan )

# target_include_directories(llgame PUBLIC
#     ${CMAKE_SOURCE_DIR}/../../VulkanSDK/1.3.216.0/Include/vulkan
# )

include(GoogleTest)
gtest_discover_tests(hello_test)
