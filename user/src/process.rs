use std::mem;
use std::ffi::{OsString, c_void};
use std::os::windows::prelude::OsStringExt;
use std::sync::{Arc, Mutex};

use lazy_static::lazy_static;

use windows::Win32::Foundation::{HANDLE, BOOL, CloseHandle};
use windows::Win32::System::Diagnostics::Debug::ReadProcessMemory;
use windows::Win32::System::Diagnostics::ToolHelp::{CreateToolhelp32Snapshot, CREATE_TOOLHELP_SNAPSHOT_FLAGS, PROCESSENTRY32W, Process32NextW, MODULEENTRY32W, TH32CS_SNAPMODULE, Module32NextW};
use windows::Win32::System::Threading::{OpenProcess, PROCESS_ALL_ACCESS, PROCESS_CREATE_THREAD};
// use windows::Win32::System::Memory::{VirtualQueryEx, MEMORY_BASIC_INFORMATION};


lazy_static! {
    pub static ref PROCESS: Arc<Mutex<Process>> = Arc::new(Mutex::new(Process {
        attached: false,
        h_process: HANDLE::default(),
        process_id: 0,
        module_address: 0
    }));
}

pub struct Process {
    pub attached: bool,
    pub h_process: HANDLE,
    pub process_id: u32,
    pub module_address: u64
}

impl Drop for Process {
    fn drop(&mut self) {
        detach_process(self);
    }
}

pub fn get_process_id(process_name: &str) -> u32 {
    let mut process_info: PROCESSENTRY32W = PROCESSENTRY32W::default();
    let h_snapshot = match unsafe { CreateToolhelp32Snapshot(CREATE_TOOLHELP_SNAPSHOT_FLAGS(15), 0) } {
        Ok(snapshot) => snapshot,
        Err(_) => { return 0; }
    };

    process_info.dwSize = mem::size_of::<PROCESSENTRY32W>() as u32;

    unsafe {
        while Process32NextW(h_snapshot, &mut process_info).is_ok() {
            let current_name = OsString::from_wide(&process_info.szExeFile[..]).into_string().unwrap().replace("\u{0}", "");
            if current_name == process_name {
                CloseHandle(h_snapshot).ok();
                return process_info.th32ProcessID;
            }
        }

        CloseHandle(h_snapshot).ok();
        return 0;
    }
}

pub fn get_process_amount(process_name: &str) -> u32 {
    let mut amount = 0;
    let mut process_info: PROCESSENTRY32W = PROCESSENTRY32W::default();
    let h_snapshot = match unsafe { CreateToolhelp32Snapshot(CREATE_TOOLHELP_SNAPSHOT_FLAGS(15), 0) } {
        Ok(snapshot) => snapshot,
        Err(_) => { return 0; }
    };

    process_info.dwSize = mem::size_of::<PROCESSENTRY32W>() as u32;

    unsafe {
        while Process32NextW(h_snapshot, &mut process_info).is_ok() {
            let current_name = OsString::from_wide(&process_info.szExeFile[..]).into_string().unwrap().replace("\u{0}", "");

            if current_name.to_lowercase() == process_name.to_lowercase() {
                amount += 1;
            }
        }

        CloseHandle(h_snapshot).ok();
        return amount;
    }
}

pub fn get_process_module_handle(module_name: &str) -> u64 {
    let process = PROCESS.clone();
    let process = process.lock().unwrap();

    let mut module_info: MODULEENTRY32W = MODULEENTRY32W::default();
    let h_snapshot = match unsafe { CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, (*process).process_id) } {
        Ok(snapshot) => snapshot,
        Err(_) => { return 0; }
    };

    module_info.dwSize = mem::size_of::<MODULEENTRY32W>() as u32;

    unsafe {
        while Module32NextW(h_snapshot, &mut module_info).is_ok() {
            let current_name = OsString::from_wide(&module_info.szModule[..]).into_string().unwrap().replace("\u{0}", "");

            if current_name == module_name {
                CloseHandle(h_snapshot).ok();
                return module_info.hModule.0 as u64;
            }
        }

        CloseHandle(h_snapshot).ok();
        return 0;
    }
}