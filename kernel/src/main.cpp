#include <ntifs.h>

extern "C" {
    NTKERNELAPI NTSTATUS IoCreateDriver(PUNICODE_STRING DriverName, PDRIVERINITIALIZE InitializationFunction);
    NTKERNELAPI NTSTATUS MmCopyVirtualMemory(PEPROCESS SourceProcess, PVOID SourceAddress, 
                                            PEPROCESS TargetProcess, PVOID TargetAddress,
                                            SIZE_T BufferSize, KPROCESSOR_MODE PreviousMode,
                                            PSIZE_T ReturnSize);
}

void debug(PCSTR text) {
#ifndef DEBUG
    UNREFERENCED_PARAMETER(text);
#endif
    KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, text));
    // setting up logging from kernel level
}

namespace driver {

    NTSTATUS create(PDEVICE_OBJECT device_object, PIRP irp) {
        UNREFERENCED_PARAMETER(device_object);
        IoCompleteRequest(irp, IO_NO_INCREMENT);
        return irp->IoStatus.Status;
    }

    NTSTATUS close(PDEVICE_OBJECT device_object, PIRP irp) {
        UNREFERENCED_PARAMETER(device_object);
        IoCompleteRequest(irp, IO_NO_INCREMENT);
        return irp->IoStatus.Status;
    }

    NTSTATUS control(PDEVICE_OBJECT device_object, PIRP irp) {
        UNREFERENCED_PARAMETER(device_object);
        debug("[CALL] Driver control called.\n");

        NTSTATUS status = STATUS_UNSUCCESSFUL;
        PIO_STACK_LOCATION irp_stack = IoGetCurrentIrpStackLocation(irp);
        auto request = reinterpret_cast<Request*>(irp->AssociatedIrp.SystemBuffer);
        if (irp_stack == nullptr || request == nullptr) {
            IoCompleteRequest(irp, IO_NO_INCREMENT);
            return status
        } // return STATUS_UNSUCCESSFULL

        static PEPROCESS target_process = nullptr;
        const ULONG control_code = irp_stack->Parameters.DeviceIoControl.IoControlCode;
        switch (control_code) {
            case codes::attach: {
                debug("[ATT] Attaching to process.\n");
                status = PsLookupProcessById(request->pid, &target_process);
                break;
            }
            case codes::read: {
                debug("[READ] Reading from process.\n");
                if (target_process != nullptr) 
                    status = MmCopyVirtualMemory(target_process, request->target, PsGetCurrentProcess(),
                                                request->buffer, request->size,
                                                KernelMode, &request->return_size);
                break;
            }
            case codes::write: {
                debug("[WRT] Writing to process.\n");
                if (target_process != nullptr) 
                    status = MmCopyVirtualMemory(PsGetCurrentProcess(), request->buffer, target_process,
                                                request->target, request->size,
                                                KernelMode, &request->return_size);
                break;
            }
            default: {
                debug("[ERR] Invalid control code.\n");
                status = STATUS_UNSUCCESSFUL;
                break;
            }
        }
        irp->IoStatus.Status = status;
        irp->IoStatus.Information = sizeof(Request)
        IoCompleteRequest(irp, IO_NO_INCREMENT);
        return status;
    }

    struct Request {
        HANDLE pid; // process id
        PVOID target;
        PVOID buffer;
        SIZE_T size;
        SIZE_T return_size;
    }; // structure implementing request from usermode to kernel level

    namespace codes {
        constexpr ULONG attach = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x696, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);
        constexpr ULONG read = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x697, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);
        constexpr ULONG write = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x698, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);
    } // implementing ctl driver codes to attach, read and write memory addresses
}

NTSTATUS main(PDRIVER_OBJECT driver_object, PUNICODE_STRING registry_path) {
    UNREFERENCED_PARAMETER(registry_path);

    UNICODE_STRING device_name = {};
    RtlInitUnicodeString(&device_name, L"\\Device\\Pathogen");
    // driver creation
    PDEVICE_OBJECT device_object = nullptr;
    NTSTATUS status = IoCreateDriver(driver_object, 0, &device_name, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &device_object);
    if (status != STATUS_SUCCESS) {
        debug("[ERR] Driver creation failed.\n");
        return status;
    }
    debug("[OK] Driver creation succeeded.\n");
    //symlinc creation
    UNICODE_STRING symlink = {};
    RtlInitUnicodeString(&symlink, L"\\DosDevices\\Pathogen");
    status = IoCreateSymbolicLink(&symlink, &device_name);
    if (status!= STATUS_SUCCESS) {
        debug("[ERR] Symlink creation failed.\n");
        return status;
    }
    debug("[OK] Symlink creation succeeded.\n");

    // Setting up the communication
    SetFlag(device_object->Flags, DO_BUFFERED_IO);
    driver_object->MajorFunction[IRP_MJ_CREATE] = driver::create;
    driver_object->MajorFunction[IRP_MJ_CLOSE] = driver::close;
    driver_object->MajorFunction[IRP_MJ_DEVICE_CONTROL] = driver::control;
    ClearFlag(device_object->Flags, DO_DEVICE_INITIALIZING);

    debug("[OK] Driver setup succeeded.\n");

    return status;
}


NTSTATUS DriverEntry() {
    debug("[ENTRY] Driver entry started.\n");

    UNICODE_STRING driver_name = {}; // unicode string ptr
    RtlInitUnicodeString(&driver_name, L"\\Driver\\Pathogen");

    return IoCreateDriver(&driver_name, &main);
}