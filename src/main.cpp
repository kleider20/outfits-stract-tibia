#include <iostream>
#include <string>
#include <vector>
#include <windows.h>
#include <shlobj.h>
#include <commctrl.h>
#include "dat_parser.hpp"
#include "lzma_handler.hpp"
#include "image_processor.hpp"
#include "filesystem_mgr.hpp"

#pragma comment(lib, "comdlg32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "comctl32.lib")

#define ID_BTN_ASSETS 1001
#define ID_BTN_OUTPUT 1002
#define ID_BTN_START 1003
#define ID_EDIT_ASSETS 1004
#define ID_EDIT_OUTPUT 1005
#define ID_TXT_LOG 1006

HWND hEditAssets;
HWND hEditOutput;
HWND hTxtLog;
bool bRunning = false;

std::string assetsDir;
std::string outputDir;

void LogMessage(const std::string& msg) {
    SetWindowTextA(hTxtLog, (GetWindowTextA(hTxtLog, nullptr, 0) + "\r\n" + msg).c_str());
}

bool SelectFolder(HWND hWnd, std::string& outPath, const std::string& title) {
    IFileDialog* pfd = nullptr;
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    
    HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_IFileOpenDialog, (void**)&pfd);
    if (FAILED(hr)) {
        CoUninitialize();
        return false;
    }

    DWORD options;
    pfd->GetOptions(&options);
    pfd->SetOptions(options | FOS_PICKFOLDERS);
    
    pfd->SetTitle(title.c_str());
    hr = pfd->Show(hWnd);
    
    if (SUCCEEDED(hr)) {
        IShellItem* psi;
        pfd->GetResult(&psi);
        LPWSTR pszPath = nullptr;
        psi->GetDisplayName(SIGDN_FILESYSPATH, &pszPath);
        
        int size = WideCharToMultiByte(CP_UTF8, 0, pszPath, -1, nullptr, 0, nullptr, nullptr);
        char* buffer = new char[size];
        WideCharToMultiByte(CP_UTF8, 0, pszPath, -1, buffer, size, nullptr, nullptr);
        outPath = buffer;
        delete[] buffer;
        
        CoTaskMemFree(pszPath);
        psi->Release();
        pfd->Release();
        CoUninitialize();
        return true;
    }
    
    pfd->Release();
    CoUninitialize();
    return false;
}

DWORD WINAPI WorkerThread(LPVOID lpParam) {
    LogMessage("[1/4] Parsing staticdata...");
    
    DatParser parser;
    std::string datPath = assetsDir + "/staticdata.dat"; 
    if (!parser.load(datPath)) {
        LogMessage("Failed to load staticdata.dat");
        bRunning = false;
        EnableWindow(GetDlgItem((HWND)lpParam, ID_BTN_START), TRUE);
        return 1;
    }

    std::vector<uint32_t> outfitIds = parser.getAllOutfitIds();
    LogMessage("Found " + std::to_string(outfitIds.size()) + " outfits.");

    for (uint32_t id : outfitIds) {
        const OutfitInfo* info = parser.getOutfit(id);
        if (!info) continue;

        LogMessage("Processing Outfit " + std::to_string(id) + " (" + std::string(info->name) + ")...");

        if (!FilesystemMgr::createOutfitFolder(id, outputDir)) continue;
        
        FilesystemMgr::writeOutfitData(id, info->frames, outputDir);
    }

    LogMessage("Extraction complete!");
    bRunning = false;
    EnableWindow(GetDlgItem((HWND)lpParam, ID_BTN_START), TRUE);
    return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            CreateWindowA("STATIC", "Assets Directory:", WS_VISIBLE | WS_CHILD, 10, 10, 100, 20, hwnd, NULL, NULL, NULL);
            hEditAssets = CreateWindowA("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_READONLY, 120, 10, 300, 25, hwnd, (HMENU)ID_EDIT_ASSETS, NULL, NULL);
            CreateWindowA("BUTTON", "Browse...", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 430, 10, 80, 25, hwnd, (HMENU)ID_BTN_ASSETS, NULL, NULL);

            CreateWindowA("STATIC", "Output Directory:", WS_VISIBLE | WS_CHILD, 10, 45, 100, 20, hwnd, NULL, NULL);
            hEditOutput = CreateWindowA("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_READONLY, 120, 45, 300, 25, hwnd, (HMENU)ID_EDIT_OUTPUT, NULL, NULL);
            CreateWindowA("BUTTON", "Browse...", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 430, 45, 80, 25, hwnd, (HMENU)ID_BTN_OUTPUT, NULL, NULL);

            CreateWindowA("BUTTON", "START EXTRACTION", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 10, 80, 500, 35, hwnd, (HMENU)ID_BTN_START, NULL, NULL);

            hTxtLog = CreateWindowA("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_READONLY | WS_VSCROLL, 10, 125, 500, 200, hwnd, (HMENU)ID_TXT_LOG, NULL, NULL);
            break;
        }
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case ID_BTN_ASSETS:
                    if (SelectFolder(hwnd, assetsDir, "Select Tibia Assets Folder")) {
                        SetWindowTextA(hEditAssets, assetsDir.c_str());
                    }
                    break;
                case ID_BTN_OUTPUT:
                    if (SelectFolder(hwnd, outputDir, "Select Output Folder")) {
                        SetWindowTextA(hEditOutput, outputDir.c_str());
                    }
                    break;
                case ID_BTN_START:
                    if (bRunning) return 0;
                    if (assetsDir.empty() || outputDir.empty()) {
                        MessageBoxA(hwnd, "Please select both Assets and Output directories!", "Error", MB_ICONERROR);
                        return 0;
                    }
                    bRunning = true;
                    EnableWindow(GetDlgItem(hwnd, ID_BTN_START), FALSE);
                    SetWindowTextA(hTxtLog, "");
                    CreateThread(NULL, 0, WorkerThread, hwnd, 0, NULL);
                    break;
            }
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProcA(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSEXA wc = {};
    wc.cbSize = sizeof(WNDCLASSEXA);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "OutfitExtractorClass";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    
    RegisterClassExA(&wc);
    
    InitCommonControls();
    
    HWND hwnd = CreateWindowExA(0, "OutfitExtractorClass", "Tibia Outfit Extractor", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 540, 380, NULL, NULL, hInstance, NULL);
    
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return (int)msg.wParam;
}
