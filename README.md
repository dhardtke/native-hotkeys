# Native Hotkeys
_Windows only_

This program allows to register hotkeys that open folders / programs on the file system.

# Usage
```cli
hotkeys.exe path-to-ini-file
```

# Sample ini file
```ini
[downloads]
key = d
modifiers = MOD_ALT,MOD_CONTROL
exec = C:\Users\User\Downloads\myProgram.exe
dir = C:\
```

Opens the file `C:\Users\User\Downloads\myProgram.exe` when pressing `CTRL` + `ALT` + `D` in the working directory `C:\`.
(The `dir` argument is optional.)