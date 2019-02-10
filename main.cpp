#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>
#include <Windows.h>
#include <map>
#include <string>
#include <sstream>
#include "lib/INIReader.h"

#include <fcntl.h> // for _O_TEXT and _O_BINARY */
#include <io.h> // for _open_osfhandle

// copied from https://stackoverflow.com/a/46050762
void RedirectIOToConsole() {
    // Create a console for this application
    AllocConsole();

    // Get STDOUT handle
    HANDLE ConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    int SystemOutput = _open_osfhandle(intptr_t(ConsoleOutput), _O_TEXT);
    FILE *COutputHandle = _fdopen(SystemOutput, "w");

    // Get STDERR handle
    HANDLE ConsoleError = GetStdHandle(STD_ERROR_HANDLE);
    int SystemError = _open_osfhandle(intptr_t(ConsoleError), _O_TEXT);
    FILE *CErrorHandle = _fdopen(SystemError, "w");

    // Get STDIN handle
    HANDLE ConsoleInput = GetStdHandle(STD_INPUT_HANDLE);
    int SystemInput = _open_osfhandle(intptr_t(ConsoleInput), _O_TEXT);
    FILE *CInputHandle = _fdopen(SystemInput, "r");

    // make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog point to console as well
    std::ios::sync_with_stdio(true);

    // Redirect the CRT standard input, output, and error handles to the console
    freopen_s(&CInputHandle, "CONIN$", "r", stdin);
    freopen_s(&COutputHandle, "CONOUT$", "w", stdout);
    freopen_s(&CErrorHandle, "CONOUT$", "w", stderr);

    // Clear the error state for each of the C++ standard stream objects. We need to do this, as
    // attempts to access the standard streams before they refer to a valid target will cause the
    // iostream objects to enter an error state. In versions of Visual Studio after 2005, this seems
    // to always occur during startup regardless of whether anything has been read from or written to
    // the console or not.
    std::wcout.clear();
    std::cout.clear();
    std::wcerr.clear();
    std::cerr.clear();
    std::wcin.clear();
    std::cin.clear();
}

void error(const std::string &msg) {
    RedirectIOToConsole();
    std::cerr << msg << std::endl;
    system("pause");
    std::exit(1);
}

void readHotkeysFromFile(const std::string &filename, std::map<std::pair<int, int>, std::string> &hotkeys) {
    static std::map<std::string, int> MODIFIERS{
            {"MOD_ALT",     MOD_ALT},
            {"MOD_CONTROL", MOD_CONTROL},
            {"MOD_SHIFT",   MOD_SHIFT},
            {"MOD_WIN",     MOD_WIN}
    };
    INIReader reader(filename);
    if (reader.ParseError() != 0) {
        error("Couldn't read " + filename + "!");
    }
    for (const auto &section : reader.Sections()) {
        const auto rawKey = reader.Get(section, "key", "");
        const auto rawModifiers = reader.Get(section, "modifiers", "");
        const auto exec = reader.Get(section, "exec", "");
        if (rawKey.empty() || rawModifiers.empty() || exec.empty()) {
            error("key, modifiers and exec are mandatory for section " + section + "!");
        }

        const int keyCode = toupper(rawKey[0]);
        int modifiers = 0;
        std::string tmp;
        std::istringstream stream(rawModifiers);
        while (getline(stream, tmp, ',')) {
            try {
                modifiers |= MODIFIERS[tmp];
            } catch (std::out_of_range &) {
                error("The modifier " + tmp +
                      " is invalid. Only MOD_ALT, MOD_CONTROL, MOD_SHIFT AND MOD_WIN are allowed");
            }
        }
        const auto identifier = std::make_pair(keyCode, modifiers);
        hotkeys[identifier] = exec;
    }
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
    // this mapping consists of (keyCode, modifiers) => directories
    // for keyCodes see https://msdn.microsoft.com/en-us/library/windows/desktop/dd375731(v=vs.85).aspx
    // for modifiers see https://msdn.microsoft.com/en-us/library/windows/desktop/ms646309%28v=vs.85%29.aspx
    std::map<std::pair<int, int>, std::string> hotkeys;
    readHotkeysFromFile(lpCmdLine, hotkeys);

    // register hotkeys
    for (auto const &iterator : hotkeys) {
        std::pair<int, int> hotkey = iterator.first;
        if (!RegisterHotKey(nullptr, 1, static_cast<UINT>(hotkey.second), static_cast<UINT>(hotkey.first))) {
            error("Couldn't register hotkey " + std::to_string(hotkey.first) + " (modifiers " +
                  std::to_string(hotkey.second) + ")");
        }
    }

    // listen to hotkey presses
    MSG msg = {nullptr};
    BOOL ret;
    // TODO: GetMessage is blocking when sending exit to the program
    while ((ret = GetMessage(&msg, nullptr, 0, 0)) != 0) {
        if (ret == -1) {
            return 1;
        }
        if (msg.message == WM_HOTKEY) {
            int modifiers = LOWORD(msg.lParam);
            int keyCode = HIWORD(msg.lParam);

            const std::string exec = hotkeys[std::make_pair(keyCode, modifiers)];
            ShellExecute(nullptr, "open", exec.c_str(), nullptr, nullptr, SW_SHOWMAXIMIZED);
        }
    }

    return 0;
}
