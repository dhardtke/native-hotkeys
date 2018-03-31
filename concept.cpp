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
        }}
};

// this mapping consists of section_name => section_values mappings
std::map<std::string, std::map<std::string, std::string>> hotkeys;

// this specifies where settings are stored
std::string settingsFilename = "C:\\Users\\nick\\CLion Projects\\hotkeys\\hotkeys.ini";

std::function<void()> getFunctionFromMapping(int modifiers, int keyCode) {
    for (auto iterator: mapping) {
        std::pair<int, int> hotkey = iterator.first;
        if (std::get<0>(hotkey) == modifiers && std::get<1>(hotkey) == keyCode) {
            return iterator.second;
        }
    }

    return NULL;
}

void readHotkeysFromFile() {
    // TODO free'ing?
    // get all section names
    LPTSTR sections = LPTSTR(65536);
    GetPrivateProfileSectionNames(sections, 65536, settingsFilename.c_str());

    std::vector<TCHAR> current = std::vector<TCHAR>();
    for (int i = 0;; i++) {
        if (sections[i] == '\0') {
            hotkeys.insert({std::string(current.begin(), current.end()), std::map<std::string, std::string>()});

            // empty buffer
            current.clear();

            // two times \0 means the end of 'sections'
            if (sections[i + 1] == '\0') {
                break;
            }
        } else {
            current.push_back(sections[i]);
        }
    }

    // read all section's values
    for (auto &kv : hotkeys) {
        LPTSTR keys = LPTSTR(65536);
        GetPrivateProfileString(kv.first.c_str(), NULL, NULL, keys, 65536, settingsFilename.c_str());
        // std::cout << kv.first << std::endl;

    }

    std::exit(0);

    /*
    std::for_each(sections, sections + sizeof(sections) / sizeof(*sections) + 3, [](char arg) {
        std::cout << arg << "\n";
    });
    */


    // TODO loop through sections
    // convert settingsFilename to LPCWSTR
    // LPCWSTR filename = std::wstring(settingsFilename.begin(), settingsFilename.end()).c_str();
    // GetPrivateProfileStringW(NULL, NULL, NULL, buffer, 1000, sw);
}

int main(int argc, char *argv[]) {
    // check if settingsFilename exists
    std::ifstream f(settingsFilename);
    if (!f.good()) {
        std::cerr << "Couldn't read " << settingsFilename << "." << std::endl;
        std::exit(1);
    }
    // read hotkeys from file
    readHotkeysFromFile();
    std::exit(0);

    /*
     * CNotifyDirCheck m_ndc;
…
UINT DirCallback(...) {...}
...
m_ndc.SetDirectory( m_dir );//root directory
m_ndc.SetData( this );//user’s data
m_ndc.SetActionCallback( DirCallback );//user’s callback
…
m_ndc.Run();//start workthread
     */

    // register hotkeys
    for (auto iterator : mapping) {
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
