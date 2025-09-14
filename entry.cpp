#include "main/includes.h"
#include "thirdparty/km/kernel.h"
#include <ovr/overlay.h>
#include <wininet.h>
#include <sddl.h>  


LPDIRECT3D9              g_pD3D = nullptr;
LPDIRECT3DDEVICE9        g_pd3dDevice = nullptr;
D3DPRESENT_PARAMETERS    g_d3dpp = {};

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool CreateDeviceD3D(HWND hWnd) {
    SPOOF_FUNC;
    g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
    if (!g_pD3D) return false;

    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    if (g_pD3D->CreateDevice(
        D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
        D3DCREATE_HARDWARE_VERTEXPROCESSING,
        &g_d3dpp, &g_pd3dDevice) < 0)
    {
        return false;
    }
    return true;
}

void CleanupDeviceD3D() {
    SPOOF_FUNC;
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
    if (g_pD3D) { g_pD3D->Release(); g_pD3D = nullptr; }
}

void ResetDevice() {
    SPOOF_FUNC;
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
    if (hr == D3DERR_INVALIDCALL) IM_ASSERT(0);
    ImGui_ImplDX9_CreateDeviceObjects();
}

ImVec2 size = ImVec2(300, 350);

enum ACCENT_STATE
{
    ACCENT_DISABLED = 0,
    ACCENT_ENABLE_GRADIENT = 1,
    ACCENT_ENABLE_TRANSPARENTGRADIENT = 2,
    ACCENT_ENABLE_BLURBEHIND = 3,
    ACCENT_ENABLE_ACRYLICBLURBEHIND = 4,
    ACCENT_ENABLE_HOSTBACKDROP = 5,
    ACCENT_INVALID_STATE = 6
};

struct ACCENT_POLICY
{
    int nAccentState;
    int nFlags;
    int nColor;
    int nAnimationId;
};

enum WINDOWCOMPOSITIONATTRIB
{
    WCA_UNDEFINED = 0,
    WCA_NCRENDERING_ENABLED = 1,
    WCA_NCRENDERING_POLICY = 2,
    WCA_TRANSITIONS_FORCEDISABLED = 3,
    WCA_ALLOW_NCPAINT = 4,
    WCA_CAPTION_BUTTON_BOUNDS = 5,
    WCA_NONCLIENT_RTL_LAYOUT = 6,
    WCA_FORCE_ICONIC_REPRESENTATION = 7,
    WCA_EXTENDED_FRAME_BOUNDS = 8,
    WCA_HAS_ICONIC_BITMAP = 9,
    WCA_THEME_ATTRIBUTES = 10,
    WCA_NCRENDERING_EXILED = 11,
    WCA_NCADORNMENTINFO = 12,
    WCA_EXCLUDED_FROM_LIVEPREVIEW = 13,
    WCA_VIDEO_OVERLAY_ACTIVE = 14,
    WCA_FORCE_ACTIVEWINDOW_APPEARANCE = 15,
    WCA_DISALLOW_PEEK = 16,
    WCA_CLOAK = 17,
    WCA_CLOAKED = 18,
    WCA_ACCENT_POLICY = 19,
    WCA_FREEZE_REPRESENTATION = 20,
    WCA_EVER_UNCLOAKED = 21,
    WCA_VISUAL_OWNER = 22,
    WCA_LAST = 23
};

struct WINDOWCOMPOSITIONATTRIBDATA
{
    WINDOWCOMPOSITIONATTRIB Attrib;
    PVOID pvData;
    SIZE_T cbData;
};

// Function pointer type
typedef BOOL(WINAPI* pSetWindowCompositionAttribute)(HWND, WINDOWCOMPOSITIONATTRIBDATA*);

void EnableBlur(HWND hwnd)
{
    // Load function dynamically
    HMODULE hUser = GetModuleHandle(skCrypt("user32.dll"));
    if (!hUser)
        return;

    pSetWindowCompositionAttribute SetWindowCompositionAttribute =
        (pSetWindowCompositionAttribute)GetProcAddress(hUser, skCrypt("SetWindowCompositionAttribute"));

    if (!SetWindowCompositionAttribute)
        return;

    ACCENT_POLICY accent = {};
    accent.nAccentState = ACCENT_ENABLE_BLURBEHIND;
    accent.nFlags = 2; // optional, 2 is standard blur behind

    WINDOWCOMPOSITIONATTRIBDATA data = {};
    data.Attrib = WCA_ACCENT_POLICY;
    data.pvData = &accent;
    data.cbData = sizeof(accent);

    SetWindowCompositionAttribute(hwnd, &data);
}

bool logged = false;

const char* text = " Valhalla - By cenky";

bool done = false;



HWND hwnd;


void updthread()
{
    SPOOF_FUNC;

    text = "checking kernel...";
    if (!km::init())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1500));
		text = "mapping driver...";
		text = "driver mapped!";
		std::this_thread::sleep_for(std::chrono::milliseconds(1500));
    }

    if (km::init())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1500));
        text = "kernel loaded!";
    }

    waiting:
    std::this_thread::sleep_for(std::chrono::milliseconds(4500));
    text = "wating for valorant...";
    
    while (true)
    {
       if (!km::process_id)
       {
		   km::process_id = km::find_process(skCrypt("VALORANT-Win64-Shipping.exe"));
       }
       else
       {
           break;
       }
    }
    text = "found valorant press f1 in lobby";

    while (!GetAsyncKeyState(VK_F1));

    text = "initializing Valhalla";

	km::process_id = km::find_process(skCrypt("VALORANT-Win64-Shipping.exe"));
    if (km::process_id == 0)
    {
        text = "valorant not open!";
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        goto waiting;
    }

	km::attach_to_process(km::process_id);

    while (!km::vgk)
    {
        km::vgk = utils::GetDriverModuleBase("vgk.sys");
        Sleep(1000);
    }


 
    std::this_thread::sleep_for(std::chrono::milliseconds(1500)); // will add window shi l8r but im lazy
  
    done = true;
}

void menu()
{
    SPOOF_FUNC

        ImGuiIO& io = ImGui::GetIO();
    ImVec2 window_size = ImVec2(size.x, size.y); // your desired window size

    // Center window on the screen
    ImGui::SetNextWindowPos(ImVec2(
        (io.DisplaySize.x - window_size.x) * 0.5f,
        (io.DisplaySize.y - window_size.y) * 0.5f
    ));
    ImGui::SetNextWindowSize(window_size);

        ImGui::Begin(skCrypt("Valhalla"), nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar |ImGuiWindowFlags_NoResize);
        static char username[64] = "";

        if (!logged) {
            // Set colors
            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.15f, 0.15f, 0.15f, 1.0f)); // dark gray
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));       // white text

            // Make it less wide
            ImGui::SetNextItemWidth(150); // width = 150 pixels

            // Make it thicker by adjusting frame padding
            float boxWidth = 150.0f;
            ImGui::SetNextItemWidth(boxWidth);
            float boxHeight = 30.0f; // controls thickness
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 3)); // x = horizontal padding, y = vertical (thickness)

            // Draw textbox
            float windowWidth = ImGui::GetWindowSize().x;
            ImVec2 windowSize = ImGui::GetWindowSize();

            ImGui::SetCursorPosX((windowWidth - boxWidth) * 0.5f);
            ImGui::SetCursorPosY((windowSize.y - boxHeight - 45) * 0.5f);


            ImGui::InputTextWithHint("##username", "      username", username, IM_ARRAYSIZE(username));

            ImGui::SetCursorPosX((windowWidth - boxWidth) * 0.5f);
            ImGui::SetCursorPosY((windowSize.y - boxHeight) * 0.5f);

            if (ImGui::Button(skCrypt("Login"), ImVec2(boxWidth, 20))) {
                if (strlen(username) > 0) {
        
                        std::thread(updthread).detach();
                        logged = true;
        
                }
            }

            ImGui::PopStyleVar();        // pop FramePadding
            ImGui::PopStyleColor(2);     // pop colors
        }

        if (logged) {

            // Fade in/out
            static float alpha = 0.0f;
            static bool increasing = true;
            const float duration = 1.5f; // seconds for fade in or out
            float speed = ImGui::GetIO().DeltaTime / duration;

            if (increasing) {
                alpha += speed;
                if (alpha >= 1.0f) {
                    alpha = 1.0f;
                    increasing = false;
                }
            }
            else {
                alpha -= speed;
                if (alpha <= 0.0f) {
                    alpha = 0.0f;
                    increasing = true;
                }
            }

            // Get text size and window size for centering
            ImVec2 windowSize = ImGui::GetWindowSize();
            ImVec2 textSize = ImGui::CalcTextSize(text); // text is const char*

            // Calculate centered position
            float posX = (windowSize.x - textSize.x) * 0.5f;
            float posY = (windowSize.y - textSize.y - 15) * 0.5f;
            ImGui::SetCursorPos(ImVec2(posX, posY));

            // Push text color with alpha
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, alpha));
            ImGui::Text("%s", text); // use "%s" for const char*
            ImGui::PopStyleColor();
        }



		ImGui::End();
}

void valcheck()
{
    SPOOF_FUNC;
    while (km::find_process("VALORANT-Win64-Shipping.exe"))
    {
        Sleep(1500);
    }
    exit(1);
}

int main()
{



   // 
 FreeConsole();
//	 hide consol window
	HWND consoleWindow = GetConsoleWindow();
    ShowWindow(consoleWindow, SW_HIDE);

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // Calculate top-left corner for centered window
    int x = (screenWidth - size.x) / 2;
    int y = (screenHeight - size.y) / 2;

	//std::thread(setupradio).detach();

    SPOOF_FUNC;
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L,
                    GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
                    skCrypt("plsnohackvgkibeg"), NULL };
    RegisterClassEx(&wc);

    hwnd = CreateWindow(
        wc.lpszClassName, skCrypt("plsnohackvgkibeg"),
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_POPUP, x, y, size.x, size.y,
        NULL, NULL, wc.hInstance, NULL);

    SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);

    if (!CreateDeviceD3D(hwnd)) {
        CleanupDeviceD3D();
        UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }


    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	io.IniFilename = NULL; // Disable .ini file

    ImGuiStyle& style = ImGui::GetStyle();

    io.FontDefault = io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/consola.ttf", 13.0f);

    // Example: semi-transparent gray
    ImVec4 bgColor = ImVec4(0.1f, 0.1f, 0.1f, 0.3f); // R,G,B,Alpha (0.0-1.0)

    // Set window background color
    style.Colors[ImGuiCol_WindowBg] = bgColor;

    // Optional: tweak rounding and padding
    style.WindowRounding = 5.0f;       // rounded corners
    style.FrameRounding = 3.0f;        // buttons, sliders rounding
    style.WindowPadding = ImVec2(10, 10);

    // Optional: transparent borders
    style.Colors[ImGuiCol_Border] = ImVec4(0, 0, 0, 0);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0, 0, 0, 0);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0, 0, 0, 0);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));          // Normal button
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));    // Hovered
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));     // Pressed

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);

	EnableBlur(hwnd);

    MSG msg;
    ZeroMemory(&msg, sizeof(msg));

    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        }

        if (done)
			break; // exit the loaders loop to go into the main one

        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        if (ImGui::IsMouseDragging(ImGuiMouseButton_Left) && !ImGui::IsAnyItemActive())
        {
            RECT rect;
            GetWindowRect(hwnd, &rect);
            SetWindowPos(hwnd, NULL,
                rect.left + (int)ImGui::GetIO().MouseDelta.x,
                rect.top + (int)ImGui::GetIO().MouseDelta.y,
                0, 0, SWP_NOSIZE | SWP_NOZORDER);
        }
        menu();
        ImGui::EndFrame();
        g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
        D3DCOLOR clear_col_dx = D3DCOLOR_ARGB(255, 45, 45, 48);
        g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
            D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);

        if (g_pd3dDevice->BeginScene() >= 0) {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }
        HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

        // Handle device lost
        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
            ResetDevice();
    }
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    DestroyWindow(hwnd);
    UnregisterClass(wc.lpszClassName, wc.hInstance);

    Sleep(1000);

    createwindow();
    if (!dx9())
    {
        MessageBoxA(0, "DirectX9 initialization failed", "Error", MB_ICONERROR);
        return 1;
    }

	std::thread(valcheck).detach();

    bool WindowFocus = false;
    while (WindowFocus == false) {
        DWORD ForegroundWindowProcessID;
        GetWindowThreadProcessId(GetForegroundWindow(), &ForegroundWindowProcessID);
        if (km::find_process("VALORANT-Win64-Shipping.exe") == ForegroundWindowProcessID) {
            Process.ID = GetCurrentProcessId();
            Process.Handle = GetCurrentProcess();
            Process.Hwnd = GetForegroundWindow();

            RECT TempRect;
            GetWindowRect(Process.Hwnd, &TempRect);
            Width = TempRect.right - TempRect.left;
            Height = TempRect.bottom - TempRect.top;
            Process.WindowLeft = TempRect.left;
            Process.WindowRight = TempRect.right;
            Process.WindowTop = TempRect.top;
            Process.WindowBottom = TempRect.bottom;

            char TempTitle[MAX_PATH];
            GetWindowText(Process.Hwnd, TempTitle, sizeof(TempTitle));
            Process.Title = TempTitle;

            char TempClassName[MAX_PATH];
            GetClassName(Process.Hwnd, TempClassName, sizeof(TempClassName));
            Process.ClassName = TempClassName;

            char TempPath[MAX_PATH];
            GetModuleFileNameEx(Process.Handle, NULL, TempPath, sizeof(TempPath));
            Process.Path = TempPath;

            WindowFocus = true;
        }
    }

  std::thread(cache::cache).detach();
   
  while (true)
  {
      valhalla::loop();
  }
}

#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    SPOOF_FUNC;
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;
    switch (msg) {

    case WM_NCHITTEST:
    {
        POINT pt;
        pt.x = GET_X_LPARAM(lParam);
        pt.y = GET_Y_LPARAM(lParam);
        ScreenToClient(hWnd, &pt);

        // Check if ImGui wants to capture the mouse
        if (!ImGui::GetIO().WantCaptureMouse)
            return HTCAPTION; // make the window draggable
        else
            return HTCLIENT;  // let ImGui handle clicks
    }


    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED) {
            g_d3dpp.BackBufferWidth = LOWORD(lParam);
            g_d3dpp.BackBufferHeight = HIWORD(lParam);
            ResetDevice();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) return 0;
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}