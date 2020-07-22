// > g++ -Wall -Wextra processes.cc -o processes
// > processes

#include <windows.h>
#include <stdio.h>
#include <tlhelp32.h>

int main() {
    // Get a list of processes present in the system. Alternatively, you can use the EnumProcesses function.
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
    BOOL process_info_available = Process32First(snapshot, &entry);
    while (process_info_available) {
        printf("%6u %12u    %s\n",
            static_cast<unsigned int>(entry.th32ProcessID),
            static_cast<unsigned int>(entry.cntThreads),
            entry.szExeFile);
        process_info_available = Process32Next(snapshot, &entry);
    }

    CloseHandle(snapshot);
    return 0;
}