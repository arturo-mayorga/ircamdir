#include "window-sys.h"
#include <windows.h>
#include <winuser.h>

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
}

void WindowSystem::unconfigure(class ECS::World *world)
{
}

void WindowSystem::tick(class ECS::World *world, float deltaTime)
{
}
