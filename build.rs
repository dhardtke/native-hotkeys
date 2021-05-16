fn main() {
    windows::build!(
        Windows::Win32::UI::WindowsAndMessaging::{GetMessageW, MSG, WM_HOTKEY, SW_SHOWMAXIMIZED, LPARAM},
        Windows::Win32::UI::KeyboardAndMouseInput::{RegisterHotKey, MOD_ALT, MOD_SHIFT, MOD_WIN, MOD_CONTROL, HOT_KEY_MODIFIERS},
        Windows::Win32::UI::Shell::ShellExecuteW,
    );
}
