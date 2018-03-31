#include <afxres.h>
#include <cstdio>
#include <functional>
#include <vector>
#include <iostream>
#include <process.h>
#include <map>
#include <fstream>
#include <algorithm>

// this mapping consists of (modifiers, keyCode) => function mappings
// notice: for modifiers see https://msdn.microsoft.com/en-us/library/windows/desktop/ms646309%28v=vs.85%29.aspx
// notice: for keyCodes see https://msdn.microsoft.com/en-us/library/windows/desktop/dd375731(v=vs.85).aspx
std::map<std::pair<int, int>, std::function<void()>> mapping{
        {{MOD_CONTROL | MOD_ALT, 0x53}, [] {
            // CTRL + ALT + S
            ShellExecute(NULL, "open", "X:\\media\\Serien", NULL, NULL, SW_SHOWMAXIMIZED);
        }},
        {{MOD_CONTROL | MOD_ALT, 0x44}, [] {
            // CTRL + ALT + D
            ShellExecute(NULL, "open", "X:\\private\\downloads", NULL, NULL, SW_SHOWMAXIMIZED);
        }},
        {{MOD_CONTROL | MOD_ALT, 0x46}, [] {
            // CTRL + ALT + F
            ShellExecute(NULL, "open", "X:\\media\\Filme", NULL, NULL, SW_SHOWMAXIMIZED);
        }},
        {{MOD_CONTROL | MOD_ALT, 0x50}, [] {
            // CTRL + ALT + P
            ShellExecute(NULL, "open", "X:\\private\\pron", NULL, NULL, SW_SHOWMAXIMIZED);
        }}
};

std::function<void()> getFunctionFromMapping(int modifiers, int keyCode) {
    for (auto iterator: mapping) {
        std::pair<int, int> hotkey = iterator.first;
        if (std::get<0>(hotkey) == modifiers && std::get<1>(hotkey) == keyCode) {
            return iterator.second;
        }
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    // register hotkeys
    for (auto const iterator : mapping) {
        std::pair<int, int> hotkey = iterator.first;
        if (!RegisterHotKey(NULL, 1, (UINT) std::get<0>(hotkey), (UINT) std::get<1>(hotkey))) {
            std::cerr << "Couldn't register hotkey." << std::endl;
            std::exit(1);
        }
    }

    // listen to hotkey presses
    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0) != 0) {
        if (msg.message == WM_HOTKEY) {
            int modifiers = LOWORD(msg.lParam);
            int keyCode = HIWORD(msg.lParam);

            std::function<void()> function = getFunctionFromMapping(modifiers, keyCode);

            if (function == NULL) {
                std::cerr << "Couldn't find function in mapping." << std::endl;
                std::exit(1);
            }

            function();
        }
    }

    return 0;
}
