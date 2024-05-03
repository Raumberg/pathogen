use wdk_sys::{
    PVOID,
    ULONG,
    FILE_DEVICE_UNKNOWN,
    METHOD_BUFFERED,
}
use std::os::windows::raw::HANDLE;
use windows_kernel_sys::base::DRIVER_OBJECT;

mod process;

pub mod driver {
    #[repr(C)]
    pub struct Request {
        pub pid: HANDLE,
        pub target: PVOID,
        pub buffer: PVOID,
        pub size: usize              //SIZE_T
        pub return_size: usize,     //SIZE_T
    }
    pub mod codes {
        pub const ATTACH: ULONG = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x696, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);
        pub const READ: ULONG = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x697, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);
        pub const WRITE: ULONG = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x698, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);
    }
}

fn main() {
    ()
}



