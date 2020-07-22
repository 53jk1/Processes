// > g++ -Wall -Wextra cmdline.cc c:\windows\System32\ntdll.dll -o cmdline.exe
// > cmdline


#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <tlhelp32.h>
#include <ntdef.h>

// https://msdn.microsoft.com/en-us/library/windows/desktop/ms684280.aspx

extern "C" LONG WINAPI NtQueryInformationProcess {
    HANDLE ProcessHandle,
    DWORD ProcessInformationClass,
    PVOID ProcessInformation,
    ULONG ProcessInformationLength,
    PULONG ReturnLength
};

// Source:
// https://msdn.microsoft.com/en-us/library/windows/desktop/ms684280.aspx

typedef struct _PROCESS_BASIC_INFORMATION {
    PVOID Reserved1;
    PVOID Peb BaseAddress;
    PVOID Reserved2[2];
    ULONG_PTR UniqueProcessId;
    PVOID Reserved3;
} PROCESS_BASIC_INFORMATION;

//https://msdn.microsoft.com/en-us/library/windows/desktop/aa813741.aspx

typedef struct _RTL_USER_PROCESS_PARAMETERS {
    BYTE           Reserved1[16];
    PVOID          Reserved2[10];
    UNICODE_STRING ImagePathName;
    UNICODE_STRING CommandLine;
} RTL_USER_PROCESS_PARAMETERS, *PRTL_USER_PROCESS_PARAMETERS;

//https://msdn.microsoft.com/en-us/library/windows/desktop/aa813706.aspx

typedef struct _PEB {
    BYTE Reserved1[2];
    BYTE BeingDebugged;
    BYTE Reserved2[1];
    PVOID Reserved3[2];
    PVOID Ldr;
    PRTL_USER_PROCESS_PARAMETERS ProcessParameters;
    BYTE Reserved4[104];
    PVOID Reserved5[52];
    PVOID PostProcessInitRoutine;
    BYTE Reserved6[128];
    PVOID Reserved7[1];
    ULONG SessionId;
} PEB, *PPEB;

void fetch_cmdline(DWORD pid, char *buffer, size_t max_length) {
    // If the buffer is too small, nothing needs to be done.
    if (buffer == NULL || max_length == 0) {
        return;
    }
    buffer[0] = '\0';

    // Open the remote process so that you can retrieve information about it and read its memory.
    HANDLE h = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (h == NULL) {
        return;
    }

    // Get basic information about the process.
    PROCESS_BASIC_INFORMATION pqi;
    LONG status = NtQueryInformationProcess(h, 0 /* ProcessBasicInformation */, &pqi, sizeof(pqi), NULL);
    if (status < 0) {
        CloseHandle(h);
        return;
    }

    // A PEB address equal to NULL means that we are probably dealing with a 64-bit process whose PEB is
    // outside the range of a 32-bit application (which also means that our application is 32-bit).
    // In that case, the ReadProcessMemory function would not be able to read the appropriate piece of
    // memory anyway.
    if (pqi.PebBaseAddress == NULL) {
        CloseHandle(h);
        return;
    }

    // Get the contents of the Process Environment Block from the remote process.

    PEB peb;
    SIZE_T read_bytes;
    BOOL result = ReadProcessMemory(h, pqi.PebBaseAddress, &peb, sizeof(peb), &read_bytes);
    if (!result || read_bytes != sizeof(peb)) {
        CloseHandle(h);
        return;
    }

    // Get the contents of the RTL_USER_PROCESS_PARAMETERS structure of the remote process.

    RTL_USER_PROCESS_PARAMETERS upp;
    result = ReadProcessMemory(h, peb.ProcessParameters, &upp, sizeof(upp), &read_bytes);
    if (!result || read_bytes != sizeof(upp)) {
        CloseHandle(h);
        return;
    }

    // Allocate an appropriate buffer to get the command line.
    size_t length = upp.CommandLine.Length + 2;
    WCHAR *command_line_utf16 = new WCHAR[length];
    memset(command_line_utf16, 0, length * sizeof(WCHAR));

    // Get the command line into the allocated buffer.
    result = ReadProcessMemory(h, upp.CommandLine.Buffer, command_line_utf16, length - 2, &read_bytes);
    if (!result || read_bytes != length - 2) {
        CloseHandle(h);
        return;
    }

    // Convert UTF-16 to the ASCII extension we use.
    memset(buffer, 0, max_length);
    WideCharToMultiByte(CP_ACP, 0, command_line_utf16, -1, buffer, max_length - 1, NULL, NULL);
    CloseHandle(h);
}

int main() {
    // Download the processes present on the system.
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "CreateToolhelp32Snapshot failed.\n");
        return 1;
    }

    puts("  PID Thread Count    Executable\n"
         "---------------------------------");

    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    // For each process on the list, write information about it.
    BOOL result = Process32First(snapshot, &entry);
    while (result) {
        char cmdline[4096];
        fetch_cmdline(entry.th32ProcessID, cmdline, sizeof(cmdline));

        printf("%6u %12u    %-20s   %s\n",
            static_cast<unsigned int>(entry.th32ProcessID),
            static_cast<unsigned int>(entry.cntThreads),
            entry.szExeFile, cmdline);
        result = Process32Next(snapshot, &entry);
    }

    CloseHandle(snapshot);
    return 0;
}