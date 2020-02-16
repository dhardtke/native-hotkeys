#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <Windows.h>
#include <map>
#include <string>
#include <utility>
#include "lib/INIReader.h"
#include "lib/Error.h"

#include <io.h> // for _open_osfhandle

// hotkeys are a pair of modifiers and their corresponding key
typedef std::pair<unsigned int, unsigned int> Hotkey;

class Action {
public:
    std::string exec;
    std::string dir;

    Action(std::string execParam, std::string dirParam) : exec(std::move(execParam)), dir(std::move(dirParam)) {
    }

    Action() : exec(""), dir("") {
        // operator[] of std::map needs an empty constructor, see https://stackoverflow.com/a/695663
    }
};

// this mapping consists of (keyCode, modifiers) => directories
// for keyCodes see https://msdn.microsoft.com/en-us/library/windows/desktop/dd375731(v=vs.85).aspx
// for modifiers see https://msdn.microsoft.com/en-us/library/windows/desktop/ms646309%28v=vs.85%29.aspx
std::map<Hotkey, Action> mappings;

void readHotkeysFromFile(const std::string &filename) {
    static std::map<std::string, unsigned int> MODIFIERS{
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
        const auto dir = reader.Get(section, "dir", "");

        const int keyCode = toupper(rawKey[0]);
        unsigned int modifiers = 0;
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
        const Hotkey hotkey = std::make_pair(keyCode, modifiers);
        const Action action(exec, dir);
        mappings[hotkey] = action;
    }
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
    readHotkeysFromFile(lpCmdLine);

    // register hotkeys
    for (auto const &iterator : mappings) {
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
            unsigned int modifiers = LOWORD(msg.lParam);
            unsigned int keyCode = HIWORD(msg.lParam);

            const Action action = mappings[std::make_pair(keyCode, modifiers)];
            ShellExecute(nullptr, "open", action.exec.c_str(), nullptr, action.dir.c_str(), SW_SHOWMAXIMIZED);
        }
    }

    return 0;
}
