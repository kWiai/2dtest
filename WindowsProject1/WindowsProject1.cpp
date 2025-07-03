#include <windows.h>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;
int x = 100;
int y = 100;
Image* g_image = nullptr;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        const wchar_t* imagePath = LR"(C:\Users\Пользователь\Desktop\human.png)";
        Image image(imagePath);
        g_image = image.GetThumbnailImage(50, 50, nullptr, nullptr);
        SetTimer(hWnd, 1, 20, nullptr); 
        break;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        Graphics graphics(hdc);
        graphics.Clear(Color::White);

        if (g_image)
            graphics.DrawImage(g_image, x, y);

        EndPaint(hWnd, &ps);
        break;
    }

    case WM_TIMER:
    {
        int step = 3; 

        if (GetAsyncKeyState(VK_RIGHT) & 0x8000 and x<750) x += step;
        if (GetAsyncKeyState(VK_LEFT) & 0x8000 and x>0) x -= step;
        if (GetAsyncKeyState(VK_UP) & 0x8000 and y>0) y -= step;
        if (GetAsyncKeyState(VK_DOWN) & 0x8000 and y<515) y += step;

        InvalidateRect(hWnd, nullptr, TRUE);
        break;
    }

    case WM_DESTROY:
        if (g_image) delete g_image;
        KillTimer(hWnd, 1);
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);

    WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszClassName = L"ImageWindowClass";

    if (!RegisterClassEx(&wcex))
    {
        MessageBox(nullptr, L"Ошибка регистрации класса окна!", L"Ошибка", MB_ICONERROR);
        return 0;
    }

    HWND hWnd = CreateWindow(
        L"ImageWindowClass",
        L"Плавное перемещение изображения",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        MessageBox(nullptr, L"Ошибка создания окна!", L"Ошибка", MB_ICONERROR);
        return 0;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    GdiplusShutdown(gdiplusToken);

    return (int)msg.wParam;
}