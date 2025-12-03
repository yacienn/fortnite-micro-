#include <windows.h>
#include <atomic>

// Button IDs
#define BUTTON_START 1
#define BUTTON_STOP  2
#define STATUS_TEXT  3


wchar_t editKey = L'F';  //
int delayMs = 5;       // 5m
std::atomic<bool> macroRunning(false);

// Macro loop 
DWORD WINAPI MacroLoop(LPVOID lpParam) {
    while (macroRunning) {
        if (GetAsyncKeyState(editKey) & 0x8000) { // If F is pressed
            
            keybd_event(editKey, 0, 0, 0);
            Sleep(delayMs);                       // Wait to make sure game registers
            keybd_event(editKey, 0, KEYEVENTF_KEYUP, 0);

            while ((GetAsyncKeyState(editKey) & 0x8000) && macroRunning) {
                Sleep(1);
            }
        }
        Sleep(1);
    }
    return 0;
}

// Window GUI
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HWND status;

    switch (msg) {
    case WM_CREATE:
        CreateWindowW(L"BUTTON", L"Start the Macro",
                      WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                      50, 50, 100, 30, hwnd, (HMENU)BUTTON_START, NULL, NULL);

        CreateWindowW(L"BUTTON", L"Stop the Macro",
                      WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                      160, 50, 100, 30, hwnd, (HMENU)BUTTON_STOP, NULL, NULL);

        status = CreateWindowW(L"Mode : ", L"Macro is stopped",
                               WS_VISIBLE | WS_CHILD | SS_CENTER,
                               50, 100, 210, 20, hwnd, (HMENU)STATUS_TEXT, NULL, NULL);
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case BUTTON_START:
            if (!macroRunning) {
                macroRunning = true;
                CreateThread(NULL, 0, MacroLoop, NULL, 0, NULL);
                SetWindowTextW(status, L"Macro is running...");
            }
            break;
        case BUTTON_STOP:
            macroRunning = false;
            SetWindowTextW(status, L"Macro is stopped");
            break;
        }
        break;

    case WM_DESTROY:
        macroRunning = false;
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
    return 0;
}

// WinMain
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
 {
    const wchar_t* CLASS_NAME = L"MacroEditGUI";

    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClassW(&wc);

    HWND hwnd = CreateWindowExW(
        0,
        CLASS_NAME,
        L"Edit Macro GUI",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 350, 200,
        NULL, NULL, hInstance, NULL
    );

    if (!hwnd) return 0;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg = {};
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return 0;
}
