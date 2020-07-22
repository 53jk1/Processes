// > cl /W4 createprocess.cc
// /out:createprocess.exe
// > createprocess.exe

#include <windows.h>
#include <stdio.h>

int main() {
    // Start the process.
    PROCESS_INFORMATION process_info;
    STARTUPINFO startup_info = {sizeof(STARTUPINFO) };

    BOOL result = CreateProcess(
        "C:\\Windows\\System32\\notepad.exe",
        NULL, NULL, NULL, FALSE, 0, NULL, NULL,
        &startup_info, &process_info);

    if(!result) {
        fprintf(stderr, "CreateProcess failed.\n");
        return 1;
    }

    // By passing the CreateProcess function an instance of
    // the PROCESS_INFORMATION structure to be filled, our
    // process is assigned two handles (to the process and
    // to the first thread). If we are going to use the
    // process handle, the thread handle can be closed
    // immediately.
    CloseHandle(process_info.hThread);

    printf("New process started (pid: %u).\n",
        static_cast<unsigned int>(process_info.dwProcessId));

    // Wait for the process to complete. The process handle,
    // which we have thanks to the PROCESS_INFORMATION
    // structure, also allows synchronization with the child
    // process (ie the function waiting for a "signal" used
    // below will return only when the child process finishes
    // its operation).
    WaitForSingleObject(process.info.hProcess, INFINITE);
    CloseHandle(process_info.hProcess);

    puts("The child process has exited.");
    return 0;
}