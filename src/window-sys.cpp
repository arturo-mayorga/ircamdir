#include "window-sys.h"
#include <windows.h>
#include <winuser.h>
#include "vulkan.h"
#include "vulkan-loader.h"

#include <iostream>

namespace
{
    enum UserMessage
    {
        USER_MESSAGE_RESIZE = WM_USER + 1,
        USER_MESSAGE_QUIT,
        USER_MESSAGE_MOUSE_CLICK,
        USER_MESSAGE_MOUSE_MOVE,
        USER_MESSAGE_MOUSE_WHEEL
    };
}

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    std::cout << "WinProc: [" << message << "]" << std::endl;

    switch (message)
    {
    case WM_LBUTTONDOWN:
        PostMessage(hWnd, USER_MESSAGE_MOUSE_CLICK, 0, 1);
        break;
    case WM_LBUTTONUP:
        PostMessage(hWnd, USER_MESSAGE_MOUSE_CLICK, 0, 0);
        break;
    case WM_RBUTTONDOWN:
        PostMessage(hWnd, USER_MESSAGE_MOUSE_CLICK, 1, 1);
        break;
    case WM_RBUTTONUP:
        PostMessage(hWnd, USER_MESSAGE_MOUSE_CLICK, 1, 0);
        break;
    case WM_MOUSEMOVE:
        PostMessage(hWnd, USER_MESSAGE_MOUSE_MOVE, LOWORD(lParam), HIWORD(lParam));
        break;
    case WM_MOUSEWHEEL:
        PostMessage(hWnd, USER_MESSAGE_MOUSE_WHEEL, HIWORD(wParam), 0);
        break;
    case WM_SIZE:
    case WM_EXITSIZEMOVE:
        PostMessage(hWnd, USER_MESSAGE_RESIZE, wParam, lParam);
        break;
    case WM_KEYDOWN:
        if (VK_ESCAPE == wParam)
        {
            PostMessage(hWnd, USER_MESSAGE_QUIT, wParam, lParam);
        }
        break;
    case WM_CLOSE:
        PostMessage(hWnd, USER_MESSAGE_QUIT, wParam, lParam);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

WindowSystem::~WindowSystem()
{
}

void WindowSystem::configure(class ECS::World *world)
{
    int x = 50;
    int y = 25;
    int width = 1280;
    int height = 800;
    auto HInstance = GetModuleHandle(nullptr);

    WNDCLASSEX window_class = {
        sizeof(WNDCLASSEX),             // UINT         cbSize
                                        /* Win 3.x */
        CS_HREDRAW | CS_VREDRAW,        // UINT         style
        WindowProcedure,                // WNDPROC      lpfnWndProc
        0,                              // int          cbClsExtra
        0,                              // int          cbWndExtra
        HInstance,                      // HINSTANCE    hInstance
        nullptr,                        // HICON        hIcon
        LoadCursor(nullptr, IDC_ARROW), // HCURSOR      hCursor
        (HBRUSH)(COLOR_WINDOW + 1),     // HBRUSH       hbrBackground
        nullptr,                        // LPCSTR       lpszMenuName
        "llgame",                       // LPCSTR       lpszClassName
                                        /* Win 4.0 */
        nullptr                         // HICON        hIconSm
    };

    std::cout << "rClass " << RegisterClassEx(&window_class) << std::endl;
    auto HWnd = CreateWindow("llgame", "window_title", WS_OVERLAPPEDWINDOW, x, y, width, height, nullptr, nullptr, HInstance, nullptr);

    std::cout << HInstance << std::endl;
    std::cout << HWnd << std::endl;

    std::cout << GetLastError() << std::endl;

    ShowWindow(HWnd, SW_SHOWNORMAL);

    HMODULE vulkan_library;
    vulkan_library = LoadLibrary("vulkan-1.dll");

    if (vulkan_library == nullptr)
    {
        std::cout << "Could not connect with a Vulkan Runtime library." << std::endl;
    }
    else
    {
        std::cout << "Vulkan loaded" << std::endl;
    }

    Vulkan::LoadFunctionExportedFromVulkanLoaderLibrary(vulkan_library);
    Vulkan::LoadGlobalLevelFunctions();

    VkInstance Instance;

    std::vector<char const *> instance_extensions;

    Vulkan::CreateVulkanInstance(instance_extensions, "Vulkan Cookbook", Instance);

    Vulkan::LoadInstanceLevelFunctions(Instance, instance_extensions);

    std::vector<VkPhysicalDevice> physical_devices;
    Vulkan::EnumerateAvailablePhysicalDevices(Instance, physical_devices);
}

void WindowSystem::unconfigure(class ECS::World *world)
{
}

void WindowSystem::tick(class ECS::World *world, float deltaTime)
{
}
