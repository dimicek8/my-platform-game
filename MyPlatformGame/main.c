#include <Windows.h>

const int main_window_width = 800;
const int main_window_height = 600;

int buffer_width;
int buffer_height;
void* buffer_memory;

struct
{
    int left;
    int right;
    int up;
    int down;
} input;

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

void RenderWeirdGradient(int x_offset, int y_offset)
{
    unsigned int* pixel = (unsigned int*)buffer_memory;
    int width = buffer_width;
    int height = buffer_height;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            unsigned char green = (x + x_offset);
            unsigned char blue = (y + y_offset);

            *pixel++ = (0 << 16) | (green << 8) | blue;
        }
    }
}

void Win32UpdateWindow(HDC device_context, int window_width, int window_height)
{
    StretchDIBits(                         //
        device_context,                    //
        0, 0, window_width, window_height, //
        0, 0, buffer_width, buffer_height, //
        buffer_memory,                     //
        &buffer_bitmap_info,               //
        DIB_RGB_COLORS,                    //
        SRCCOPY                            //
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

    int running = 1;
    int x_offset = 0;

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

        if (input.left)
        {
            x_offset -= 5;
        }
        if (input.right)
        {
            x_offset += 5;
        }

        RenderWeirdGradient(x_offset, 0);

        RECT client_rect;
        GetClientRect(hwnd, &client_rect);
        int window_width = client_rect.right - client_rect.left;
        int window_height = client_rect.bottom - client_rect.top;

        HDC hdc = GetDC(hwnd);

        Win32UpdateWindow(hdc, window_width, window_height);

        ReleaseDC(hwnd, hdc);
    }

    return 0;
}