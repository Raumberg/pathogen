use wdk_sys::{
    PVOID,
    ULONG,
    FILE_DEVICE_UNKNOWN,
    METHOD_BUFFERED,
};
use std::os::windows::raw::HANDLE;
use windows_kernel_sys::base::DRIVER_OBJECT;
use winapi::um::winioctl::{FILE_SPECIAL_ACCESS, CTL_CODE}; // == file_any_access
use Win32::IO::DeviceIoControl;
//use winapi::shared::minwindef::DWORD;

pub mod driver {
    #[repr(C)]
    pub struct Request {
        pub pid: HANDLE,
        pub target: PVOID,
        pub buffer: PVOID,
        pub size: usize,              //SIZE_T
        pub return_size: usize,     //SIZE_T
    }
    impl Request {
        fn new {}
    }
    pub mod codes {
        pub const ATTACH: ULONG = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x696, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);
        pub const READ: ULONG = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x697, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);
        pub const WRITE: ULONG = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x698, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);
    }
    #[no_mangle]
    pub unsafe extern "C" fn CTL_CODE () {
        // TODO:
        // no need?
    }

    pub fn ATTACH(driver_handle: HANDLE, pid: u32) -> bool {
        let req = Request {pid: u32 as *HANDLE};

        return DeviceIoControl(&driver_handle, )
    }

}