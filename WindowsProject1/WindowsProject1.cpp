#include <windows.h>
#include <gdiplus.h>
#include <iostream>
#include <map>
#pragma comment(lib, "gdiplus.lib")
#include <vector>
#include <string>
#include <stdlib.h>

using namespace Gdiplus;
using namespace std;
string item_name;
map < string, Image*> itemses;
int x = 100;
int y = 100;
int num_to;
int step = 3;
Image* h_image = nullptr;
Image* l_image = nullptr;
Image* e_image = nullptr;
const int sum_location = 4;
Image* _image = nullptr;
int r;
string item;

struct inventory {
    string name;
    int posX;
    int posY;
    const wchar_t* path_to_image;
};

struct doors {
    int num_location;
    int posX;
    int posY;
};

struct collision {
    int weight;
    int height;
    int posX;
    int posY;
};

struct closed_doors {
    int num_location;
    int posX;
    int posY;
};

struct enemys {
    int posX;
    int posY;
    const wchar_t* enemy_image;
    bool active; // Добавлено поле активности врага
};

struct location {
    string name;
    string description;
    int num;
    vector<doors> door;
    vector<closed_doors> close_door;
    vector<inventory> items;
    vector<collision> collisions;
    const wchar_t* location_path;
};

struct hero {
    int current_location;
    int hp;
    vector<string> inventory;
    int posX;
    int posY;
    const wchar_t* hero_path;
};

enemys enemy{ 25,25,LR"(enemy.jpg)", false }; // Инициализация врага как неактивного
location locations[sum_location]{

    { "hall","center of home",0,{{1,726,264}},{},{},{{111,130,200,31},
    {111,130,372,31},{111,130,556,31},{800,1,0,106},{49,131,80,423},
    {49,131,246,423},{49,131,418,423},{49,131,597,423}}, LR"(hall.png)" },

    { "hallway","connect room",1,{{0,0,264}},{{2,726,264}},
    {{"key to kitchen",400,300,LR"(key.png)"}},{{800,41,0,156},
    {800,1,0,427}}, LR"(hallway.png)"},

    { "kitchen", "eat room", 2, {{1,0,264},{3,726,264}},{},
    {{"poizon",400,300,LR"(hp_poizon.jpg)"}},
    {{800,83,0,136},{142,72,372,465}}, LR"(kitchen.png)"},

    { "bathroom", "wash room", 3,{{2,0,264}},{},{},
    {{71,104,667,321},{800,1,0,156},{113,97,519,118},{268,97,0,118}}
     ,LR"(bathroom.png)"} };

hero man{ 0,2,{},50,300,LR"(human.png)" };

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        Image himage(man.hero_path);
        Image limage(locations[man.current_location].location_path);
        Image eimage(enemy.enemy_image); 
        h_image = himage.GetThumbnailImage(50, 50, nullptr, nullptr);
        l_image = limage.GetThumbnailImage(800, 600, nullptr, nullptr);
        e_image = eimage.GetThumbnailImage(50, 50, nullptr, nullptr); 
        for (inventory item : locations[man.current_location].items) {
            Image* img = new Image(item.path_to_image);
            if (img->GetLastStatus() == Ok) {
                itemses[item.name] = img->GetThumbnailImage(50, 50, nullptr, nullptr);
            }
            delete img;
        }
        SetTimer(hWnd, 1, 100, NULL); 
        break;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        RECT rect;
        GetClientRect(hWnd, &rect);
        int width = rect.right - rect.left;
        int height = rect.bottom - rect.top;

        HDC hdcMem = CreateCompatibleDC(hdc);
        HBITMAP hbmMem = CreateCompatibleBitmap(hdc, width, height);
        SelectObject(hdcMem, hbmMem);

        Graphics graphics(hdcMem);
        graphics.Clear(Color::White);


        if (l_image)
            graphics.DrawImage(l_image, 0, 0);


        if (!locations[man.current_location].items.empty())
        {
            for (const auto& item : locations[man.current_location].items)
            {
                if (itemses.count(item.name) && itemses[item.name])
                {
                    graphics.DrawImage(itemses[item.name], item.posX, item.posY);
                }
            }
        }

        if (enemy.active && e_image) 
            graphics.DrawImage(e_image, enemy.posX, enemy.posY);
        
        if (h_image)
            graphics.DrawImage(h_image, man.posX, man.posY);

        BitBlt(hdc, 0, 0, width, height, hdcMem, 0, 0, SRCCOPY);

        DeleteObject(hbmMem);
        DeleteDC(hdcMem);
        EndPaint(hWnd, &ps);
        break;
    }

    case WM_TIMER:
        if (wParam == 1 && enemy.active) {
            if (enemy.posX < man.posX)
                enemy.posX += step;
            else if (enemy.posX > man.posX)
                enemy.posX -= step;

            if (enemy.posY < man.posY)
                enemy.posY += step;
            else if (enemy.posY > man.posY)
                enemy.posY -= step;

            if (abs(enemy.posX - man.posX) < 50 and abs(enemy.posY - man.posY) < 50) {
                enemy.active = false; 
                man.hp -= 1;
                if (man.hp == 0) {
                    PostMessage(hWnd, WM_CLOSE, 0, 0); 
                    return 0;
                }

            }

            InvalidateRect(hWnd, nullptr, FALSE);
        }
        break;

    case WM_KEYDOWN:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        RECT rect;
        GetClientRect(hWnd, &rect);
        int width = rect.right - rect.left;
        int height = rect.bottom - rect.top;

        HDC hdcMem = CreateCompatibleDC(hdc);
        HBITMAP hbmMem = CreateCompatibleBitmap(hdc, width, height);
        SelectObject(hdcMem, hbmMem);

        Graphics graphics(hdcMem);
        int step = 2;

        if (GetAsyncKeyState(VK_RIGHT) & 0x8000 and man.posX + 50 < 780) man.posX += step;
        if (GetAsyncKeyState(VK_LEFT) & 0x8000 and man.posX > 0) man.posX -= step;
        if (GetAsyncKeyState(VK_UP) & 0x8000 and man.posY > 0) man.posY -= step;
        if (GetAsyncKeyState(VK_DOWN) & 0x8000 and man.posY+50 < 560) man.posY += step;
        for (int i = 0; i < locations[man.current_location].collisions.size(); i++) {
            if (man.posX + 50 > locations[man.current_location].collisions[i].posX and
                man.posX < locations[man.current_location].collisions[i].posX + locations[man.current_location].collisions[i].weight)
            {
                if (man.posY < locations[man.current_location].collisions[i].posY
                    + locations[man.current_location].collisions[i].height and
                    man.posY + 50 == locations[man.current_location].collisions[i].posY
                    + locations[man.current_location].collisions[i].height + 49)
                {
                    man.posY += step;
                }
                if (man.posY+50 > locations[man.current_location].collisions[i].posY and
                    man.posY == locations[man.current_location].collisions[i].posY - 49)
                {
                    man.posY -= step;
                }
            }
            if (man.posY + 50 > locations[man.current_location].collisions[i].posY
                and man.posY < locations[man.current_location].collisions[i].posY
                + locations[man.current_location].collisions[i].height) {
                if(man.posX+50> locations[man.current_location].collisions[i].posX and
                    man.posX + 50 < locations[man.current_location].collisions[i].posX + locations[man.current_location].collisions[i].weight)
                {
                    man.posX -= step;
                }
                if (man.posX < locations[man.current_location].collisions[i].posX + locations[man.current_location].collisions[i].weight and
                    man.posX + 50 > locations[man.current_location].collisions[i].posX + locations[man.current_location].collisions[i].weight)
                {
                    man.posX += step;
                }
            }
        }

        
        for (int i = 0; i < locations[man.current_location].door.size(); i++) {
            if (((man.posX + 50 > locations[man.current_location].door[i].posX and
                man.posX < locations[man.current_location].door[i].posX + 74) or
                (man.posX > locations[man.current_location].door[i].posX + 50 and
                    man.posX < locations[man.current_location].door[i].posX + 75)) and
                (man.posY > locations[man.current_location].door[i].posY - 50 and
                    man.posY < locations[man.current_location].door[i].posY + 100)) {
                if (man.posX > 400)
                    man.posX = locations[man.current_location].door[i].posX - 650;
                else if (man.posX < 400)
                    man.posX = locations[man.current_location].door[i].posX + 675;
                man.posY = 300;
                man.current_location = locations[man.current_location].door[i].num_location;

                graphics.Clear(Color::White);
                Image limage(locations[man.current_location].location_path);
                l_image = limage.GetThumbnailImage(800, 600, nullptr, nullptr);

                srand(time(0));
                r = rand() % 3;
                enemy.active = (r == 0);
                if (enemy.active) {
                    enemy.posX = 25;
                    enemy.posY = 25;
                }

                if (l_image)
                    graphics.DrawImage(l_image, 0, 0);
                if (h_image)
                    graphics.DrawImage(h_image, man.posX, man.posY);

                for (auto& item : itemses) {
                    delete item.second;
                }
                itemses.clear();
                for (inventory item : locations[man.current_location].items) {
                    Image* img = new Image(item.path_to_image);
                    if (img->GetLastStatus() == Ok) {
                        itemses[item.name] = img->GetThumbnailImage(30, 30, nullptr, nullptr);
                    }
                    delete img;
                }
            }
        }

        if (!locations[man.current_location].items.empty()) {
            for (int i = 0; i < (locations[man.current_location].items).size(); i++) {
                if (((man.posX + 50 > locations[man.current_location].items[i].posX and
                    man.posX < locations[man.current_location].items[i].posX + 50) or
                    (man.posX < locations[man.current_location].items[i].posX + 50 and
                        man.posX + 50 > locations[man.current_location].items[i].posX)) and
                    (locations[man.current_location].items[i].posY + 15 > man.posY and
                        locations[man.current_location].items[i].posY + 15 < man.posY + 50))
                {
                    if (locations[man.current_location].items[i].name.find("key to") != string::npos) {
                        string loc = locations[man.current_location].items[i].name.substr(7);
                        size_t index_to_remove = i;
                        locations[man.current_location].items.erase(locations[man.current_location].items.begin() + index_to_remove);
                        for (int j = 0; j < sum_location; j++) {
                            if (loc == locations[j].name) {
                                num_to = locations[j].num;
                                break;
                            }
                        }
                        for (int j = 0; j < (locations[man.current_location].close_door).size(); j++) {
                            if (locations[man.current_location].close_door[j].num_location == num_to) {
                                locations[man.current_location].door.push_back({ locations[man.current_location].close_door[j].num_location,locations[man.current_location].close_door[j].posX,locations[man.current_location].close_door[j].posY });
                                size_t index_to_remove = j;
                                if (index_to_remove < (locations[man.current_location].close_door).size()) {
                                    locations[man.current_location].close_door.erase(locations[man.current_location].close_door.begin() + index_to_remove);
                                    break;
                                }
                            }
                        }
                    }
                    else if (locations[man.current_location].items[i].name == "poizon") {
                        size_t index_to_remove = i;
                        locations[man.current_location].items.erase(locations[man.current_location].items.begin() + index_to_remove);
                        man.hp += 1;
                    }

                    for (auto& item : itemses) {
                        delete item.second;
                    }
                    itemses.clear();
                    for (inventory item : locations[man.current_location].items) {
                        Image* img = new Image(item.path_to_image);
                        if (img->GetLastStatus() == Ok) {
                            itemses[item.name] = img->GetThumbnailImage(30, 30, nullptr, nullptr);
                        }
                        delete img;
                    }
                }
            }
        }

        BitBlt(hdc, 0, 0, width, height, hdcMem, 0, 0, SRCCOPY);
        DeleteObject(hbmMem);
        DeleteDC(hdcMem);
        EndPaint(hWnd, &ps);
        InvalidateRect(hWnd, nullptr, FALSE);
        break;
    }

    case WM_DESTROY:
        KillTimer(hWnd, 1); 
        if (h_image) delete h_image;
        if (l_image) delete l_image;
        if (e_image) delete e_image;
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
        L"2d advenchure",
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