#pragma once
#include "utils.h"

// Driver Codes
#define READ CTL_CODE(FILE_DEVICE_UNKNOWN, 0x1, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define WRITE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x2, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

#define GET_CLIENT CTL_CODE(FILE_DEVICE_UNKNOWN, 0x10, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define GET_ENGINE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x11, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

#define PING CTL_CODE(FILE_DEVICE_UNKNOWN, 0x20, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define INTERNAL_READ CTL_CODE(FILE_DEVICE_UNKNOWN, 0x21, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)


// Request for read/writing memory from/to a process.
#define IOCTL_DRIVER_COPY_MEMORY ((ULONG)CTL_CODE(DRIVER_DEVICE_TYPE, 0x808, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS))

// Copy requeest data.
typedef struct _DRIVER_COPY_MEMORY {
	ULONGLONG Source; // Source buffer address.
	ULONGLONG Target; // Target buffer address.
	ULONGLONG Size; // Buffer size.
	ULONG ProcessId; // Target process ID.
	BOOLEAN Write; // TRUE if writing, FALSE if reading.
} DRIVER_COPY_MEMORY, *PDRIVER_COPY_MEMORY;