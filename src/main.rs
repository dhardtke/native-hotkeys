#![windows_subsystem = "windows"]

use std::collections::HashMap;
use std::convert::TryInto;
use std::env;
use std::hash::{Hash, Hasher};
use std::mem::MaybeUninit;

use configparser::ini::Ini;

use bindings::{
    Windows::Win32::UI::KeyboardAndMouseInput::{HOT_KEY_MODIFIERS, MOD_ALT, MOD_CONTROL, MOD_SHIFT, MOD_WIN, RegisterHotKey},
    Windows::Win32::UI::Shell::ShellExecuteW,
    Windows::Win32::UI::WindowsAndMessaging::{GetMessageW, LPARAM, MSG, SW_SHOWMAXIMIZED, WM_HOTKEY},
};

mod bindings {
    windows::include_bindings!();
}

unsafe fn register_hotkey(modifiers: HOT_KEY_MODIFIERS, key: u32) -> bool {
    RegisterHotKey(None, 1, modifiers, key).as_bool()
}

fn read_config(file: &str) -> Result<Ini, String> {
    let mut config = Ini::new();
    let contents = std::fs::read_to_string(file).map_err(|e| { e.to_string() })?;
    config.read(contents).map(|_| { config })
}

#[derive(PartialEq, Eq)]
struct Hotkey(HOT_KEY_MODIFIERS, u32);

impl Hash for Hotkey {
    fn hash<H: Hasher>(&self, state: &mut H) {
        self.0.0.hash(state);
        self.1.hash(state);
    }
}

#[derive(PartialEq, Eq, Hash)]
struct Action {
    exec: String,
    dir: String,
}

fn modifier_from_string(str: &str) -> Option<HOT_KEY_MODIFIERS> {
    match str {
        "MOD_ALT" => Some(MOD_ALT),
        "MOD_CONTROL" => Some(MOD_CONTROL),
        "MOD_SHIFT" => Some(MOD_SHIFT),
        "MOD_WIN" => Some(MOD_WIN),
        _ => None,
    }
}

fn parse_modifiers(modifiers: String) -> HOT_KEY_MODIFIERS {
    let mut parsed: HOT_KEY_MODIFIERS = HOT_KEY_MODIFIERS::default();
    for maybe_modifier in modifiers.split(",") {
        parsed |= modifier_from_string(maybe_modifier)
            .unwrap_or_else(|| panic!("Invalid modifier: {}", maybe_modifier));
    }
    parsed
}

fn initialize_registry(config: Ini) -> HashMap<Hotkey, Action> {
    let mut registry: HashMap<Hotkey, Action> = HashMap::new();
    for section in config.sections() {
        let key = config.get(section.as_str(), "key")
            .unwrap_or_else(|| { panic!("Missing key in section {}", section) });
        let modifiers = config.get(section.as_str(), "modifiers")
            .unwrap_or_else(|| { panic!("Missing modifiers in section {}", section) });
        let exec = config.get(section.as_str(), "exec")
            .unwrap_or_else(|| { panic!("Missing exec in section {}", section) });
        let dir = config.get(section.as_str(), "dir")
            .unwrap_or(String::from(""));

        let hotkey = Hotkey(parse_modifiers(modifiers), key.to_uppercase().chars().nth(0)
            .unwrap_or_else(|| { panic!("Missing or invalid key in section {}", section) }) as u32);
        let action = Action { exec, dir };
        registry.insert(hotkey, action);
    }
    registry
}

#[inline(always)]
fn loword(lparam: LPARAM) -> u32 {
    (lparam.0 & 0xFFFF).try_into().unwrap()
}

#[inline(always)]
fn hiword(lparam: LPARAM) -> u32 {
    ((lparam.0 & 0xFFFF0000) >> 16).try_into().unwrap()
}

fn main() -> windows::Result<()> {
    let file = env::args().nth(1).unwrap_or_else(|| { String::from("config.ini") });
    let config = read_config(&file).unwrap_or_else(|_| { panic!("Could not read config {}", file) });
    let registry = initialize_registry(config);

    unsafe {
        for (hotkey, _) in &registry {
            if !register_hotkey(hotkey.0, hotkey.1) {
                panic!("Could not register hotkey");
            }
        }

        let mut wrapper = MaybeUninit::<MSG>::uninit();
        while GetMessageW(wrapper.as_mut_ptr(), None, 0, 0).as_bool() {
            let msg: MSG = wrapper.assume_init();
            if msg.message == WM_HOTKEY {
                let modifiers: u32 = loword(msg.lParam);
                let key_code: u32 = hiword(msg.lParam);
                let hotkey = Hotkey(HOT_KEY_MODIFIERS(modifiers), key_code);
                let action = registry.get(&hotkey).expect("");
                ShellExecuteW(None, "open", action.exec.as_str(), None, action.dir.as_str(), SW_SHOWMAXIMIZED.0 as i32);
            }
        }
    }

    Ok(())
}
