#include <Windows.h>

const int main_window_width = 800;
const int main_window_height = 600;

int buffer_width;
int buffer_height;
void* buffer_memory;

float player_x = 100;
float player_y = 100;
float camera_x = 0;

int playerWidth = 36;
int playerHeight = 36;

float velocity_x = 0;
float velocity_y = 0;
float gravity = 0.05f;

const float offset_x = 0.3f;
const float offset_y = 0.3f;

const int backgroundColor = 0x000000;
const int playerColor = 0x00FF00;
const int platformColor = 0xFF0000;
const int coinColor = 0xFFFF00;

const char* character_pixels[] = {"....RRRRR...",
    "...RRRRRRRR.",
    "...HHHSSH...",
    "..HSHSSSHH..",
    "..HSHHSSSH..",
    "..HSSSSS....",
    "...RRBRR....",
    "..RRRBRRR...",
    "..RRBBRR....",
    "..BBBBBB....",
    "..HH..HH....",
    ".HH....HH..."};

const int character_sprite_w = 12;
const int character_sprite_h = 12;
int pixel_scale = 3;

struct
{
    int left;
    int right;
    int up;
    int down;
} input;

struct Platform
{
    int x, y, width, height;
};

struct Platform platforms[] = {
    {200, 400, 200, 30},  //
    {500, 250, 150, 30},  //
    {50, 300, 100, 30},   //
    {600, 500, 200, 30},  //
    {1100, 150, 400, 30}, //
    {800, 450, 300, 30},  //
    {1300, 300, 200, 30}, //
};

int platform_count = sizeof(platforms) / sizeof(platforms[0]);

struct Coin
{
    int x, y;
    int active;
};

struct Coin coins[] = {
    {300, 350, 1}, //
    {550, 200, 1}, //
    {900, 400, 1}, //
    {1150, 300, 1} //
};

int coin_count = sizeof(coins) / sizeof(coins[0]);
int score = 0;

int flag_x = 1800;
int game_won = 0;

BITMAPINFO buffer_bitmap_info;

void ResizeDIBSection(int width, int height)
{
    if (buffer_memory)
    {
        VirtualFree(buffer_memory, 0, MEM_RELEASE);
    }

    buffer_width = width;
    buffer_height = height;

    buffer_bitmap_info.bmiHeader.biSize = sizeof(buffer_bitmap_info.bmiHeader);
    buffer_bitmap_info.bmiHeader.biWidth = width;

    buffer_bitmap_info.bmiHeader.biHeight = -height;

    buffer_bitmap_info.bmiHeader.biPlanes = 1;
    buffer_bitmap_info.bmiHeader.biBitCount = 32;
    buffer_bitmap_info.bmiHeader.biCompression = BI_RGB;

    int bitmap_memory_size = (width * height) * 4;

    buffer_memory = VirtualAlloc(0, bitmap_memory_size, MEM_COMMIT, PAGE_READWRITE);
}

void DrawRect(int x, int y, int width, int height, unsigned int color)
{
    int x0 = x;
    int x1 = x + width;
    int y0 = y;
    int y1 = y + height;

    if (x0 < 0)
    {
        x0 = 0;
    }
    if (x1 > buffer_width)
    {
        x1 = buffer_width;
    }
    if (y0 < 0)
    {
        y0 = 0;
    }
    if (y1 > buffer_height)
    {
        y1 = 0;
    }

    for (int py = y0; py < y1; py++)
    {
        unsigned int* row = (unsigned int*)buffer_memory + (py * buffer_width);

        for (int px = x0; px < x1; px++)
        {
            unsigned int* pixel = row + px;
            *pixel = color;
        }
    }
}

void DrawCharacterSprite(int x, int y)
{
    for (int row = 0; row < character_sprite_h; row++)
    {
        for (int col = 0; col < character_sprite_w; col++)
        {
            char c = character_pixels[row][col];

            unsigned int color = 0;
            int draw = 1;

            switch (c)
            {
            case 'R':
                color = 0x00FF0000;
                break;
            case 'B':
                color = 0x000000FF;
                break;
            case 'S':
                color = 0x00FFCCAA;
                break;
            case 'H':
                color = 0x00884400;
                break;
            default:
                draw = 0;
                break;
            }

            if (draw)
            {
                DrawRect(x + (col * pixel_scale), //
                    y + (row * pixel_scale),      //
                    pixel_scale,
                    pixel_scale,
                    color);
            }
        }
    }
}

void RenderGame()
{
    DrawRect(0, 0, buffer_width, buffer_height, backgroundColor);

    for (int i = 0; i < platform_count; i++)
    {
        DrawRect(platforms[i].x - (int)camera_x, //
            platforms[i].y,                      //
            platforms[i].width,                  //
            platforms[i].height,                 //
            platformColor);                      //
    }

    for (int i = 0; i < coin_count; i++)
    {
        if (coins[i].active)
        {
            DrawRect(coins[i].x - (int)camera_x, //
                coins[i].y,                      //
                20,                              //
                20,                              //
                coinColor);                      //
        }
    }

    DrawRect(                   //
        flag_x - (int)camera_x, //
        100,                    //
        10,                     //
        400,
        0xAAAAAA //
    );

    DrawRect(                   //
        flag_x - (int)camera_x, //
        120,                    //
        40,                     //
        30,                     //
        0x0000FF                //
    );

    DrawCharacterSprite(               //
        (int)player_x - (int)camera_x, //
        (int)player_y                  //
    );
}

void Win32UpdateWindow(HDC device_context, int window_width, int window_height)
{
    StretchDIBits(      //
        device_context, //
        0,
        0,
        window_width,
        window_height, //
        0,
        0,
        buffer_width,
        buffer_height,       //
        buffer_memory,       //
        &buffer_bitmap_info, //
        DIB_RGB_COLORS,      //
        SRCCOPY              //
    );
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_KEYDOWN:
    case WM_KEYUP:
    {
        int is_down = (uMsg == WM_KEYDOWN);

        switch (wParam)
        {
        case VK_LEFT:
            input.left = is_down;
            break;
        case VK_RIGHT:
            input.right = is_down;
            break;
        case VK_UP:
            input.up = is_down;
            break;
        case VK_DOWN:
            input.down = is_down;
            break;
        }
        return 0;
    }
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdSHow)
{
    const wchar_t CLASS_NAME[] = L"PlatformGameWindow";
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(           //
        0,                                //
        CLASS_NAME,                       //
        L"My Platform Game",              //
        WS_OVERLAPPEDWINDOW | WS_VISIBLE, //
        CW_USEDEFAULT,                    //
        CW_USEDEFAULT,                    //
        main_window_width,                //
        main_window_height,               //
        NULL,                             //
        NULL,                             //
        hInstance,                        //
        NULL                              //
    );

    if (hwnd == NULL)
    {
        return 0;
    }

    ResizeDIBSection(main_window_width, main_window_height);

    DWORD last_fps_time = GetTickCount64();
    int frames_this_second = 0;
    int current_fps = 0;

    int running = 1;
    int on_ground = 0;

    while (running)
    {
        MSG msg;

        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                running = 0;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        velocity_y += gravity;
        velocity_x = 0;

        if (game_won == 0)
        {
            if (input.left)
            {
                velocity_x = -offset_x;
            }
            if (input.right)
            {
                velocity_x = +offset_x;
            }
            if (input.up && on_ground)
            {
                velocity_y = -5.0f;
                on_ground = 0;
            }
        }

        if (game_won == 0 && (player_x + playerWidth) >= flag_x)
        {
            game_won = 1;

            SetWindowText(hwnd, L"YOU WON!");
        }

        player_x += velocity_x;

        for (int i = 0; i < platform_count; i++)
        {
            struct Platform p = platforms[i];

            if (player_x + playerWidth > p.x && player_x < p.x + p.width && //
                player_y + playerHeight > p.y && player_y < p.y + p.height)
            {
                if (velocity_x > 0)
                {
                    player_x = p.x - playerWidth;
                }
                else if (velocity_x < 0)
                {
                    player_x = p.x + p.width;
                }

                velocity_x = 0;
            }
        }

        player_y += velocity_y;

        for (int i = 0; i < platform_count; i++)
        {
            struct Platform p = platforms[i];

            if (player_x + playerWidth > p.x && player_x < p.x + p.width && //
                player_y + playerHeight > p.y && player_y < p.y + p.height)
            {
                if (velocity_y > 0)
                {
                    player_y = p.y - playerHeight;
                    velocity_y = 0;
                    on_ground = 1;
                }
                else if (velocity_y < 0)
                {
                    player_y = p.y + p.height;
                    velocity_y = 0;
                }
            }
        }

        for (int i = 0; i < coin_count; i++)
        {
            if (coins[i].active == 0)
            {
                continue;
            }

            if (player_x + 30 > coins[i].x && //
                player_x < coins[i].x + 20 && //
                player_y + 30 > coins[i].y && //
                player_y < coins[i].y + 20)
            {
                coins[i].active = 0;
                score++;

                char title[64];
                wsprintf(title, L"My Platformer Score: %d", score);
                SetWindowText(hwnd, title);
            }
        }

        if (player_y > 500)
        {
            player_y = 500;
            on_ground = 1;
            velocity_y = 0;
        }

        if (player_x < 0)
        {
            player_x = 0;
        }

        camera_x = player_x - (main_window_width / 2);

        if (camera_x < 0)
        {
            camera_x = 0;
        }

        RenderGame();

        frames_this_second++;
        DWORD current_time = GetTickCount();

        if (current_time - last_fps_time >= 1000)
        {
            current_fps = frames_this_second;
            frames_this_second = 0;
            last_fps_time = current_time;
        }

        RECT client_rect;
        GetClientRect(hwnd, &client_rect);
        int window_width = client_rect.right - client_rect.left;
        int window_height = client_rect.bottom - client_rect.top;

        HDC hdc = GetDC(hwnd);

        Win32UpdateWindow(hdc, window_width, window_height);

        SetBkMode(hdc, TRANSPARENT);

        SetTextColor(hdc, RGB(255, 255, 255));

        wchar_t fps_text[64];
        int fps_len = wsprintf(fps_text, L"FPS: %d", current_fps);
        TextOut(hdc, 10, 10, fps_text, fps_len);

        wchar_t score_text[64];
        int score_len = wsprintf(score_text, L"Score: %d", score);
        TextOut(hdc, 10, 30, score_text, score_len);

        if (game_won)
        {
            SetTextColor(hdc, RGB(255, 215, 0));
            wchar_t win_text[] = L"YOU WON!";
            TextOut(hdc, 396, 250, win_text, 8);
        }

        ReleaseDC(hwnd, hdc);
    }

    return 0;
}