// > gcc print_and_wait.c -o print_and_wait.exe
// > print_and_wait
// Address: <address>

// > g++ -Wall -Wextra readmem.cc -o readmem.exe
// > tasklist | find "print_and_wait"
// > readmem 13776 0x<address> 24

#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <new>

void usage() {
    puts("usage: readmem <pid> <address> <length>\n"
         "e.g. : readmem 1234 0x12345678 32\n"
         "       Reads 32 bytes from address 0x12345678 of process 1234.\n");
}

int main(int argc, char **argv) {
    if (argc != 4) {
        usage()
        return 1;
    }

    unsigned int pid;
    uint64_t address;
    unsigned int length;
    if (sscanf(argv[1], "%u", &pid) != 1 ||
        sscanf(argv[2], "%I64i", &address) != 1 ||
        sscanf(argv[3], "%i", &length) != 1) {
            puts("error: incorrect argument");
            usage();
            return 1;
        }

        // Open a process with permission to read memory.
        HANDLE h = OpenProcess(PROCESS_VM_READ, FALSE, pid);
        if (h == NULL) {
            printf("error: could not open remote process (code: %u)\n",
                    static_cast<unsigned int>(GetLastError()));
            return 1;
        }

        // Read and write the given fragment from memory.
        BYTE *data = new(std::nothrow) BYTE[length];
        if (data == NULL) {
            printf("error: failed to allocate %u bytes of memory\n", length);

            CloseHandle(h);
            return 1;
        }

        BOOL result = ReadProcessMemory(
            h, reinterpret_cast<void *>(address), data, length, NULL);
        if (!result) {
            puts("error: failed to read memory");
            delete [] data;
            CloseHandle(h);
            return 1;
        }

        fwrite(data, 1, length, stdout);
        putchar('\n');

        delete [] data;
        CloseHandle(h);
        return 0;
}