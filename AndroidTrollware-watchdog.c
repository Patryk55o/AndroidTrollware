/* Transparent companion: logs an unexpected parent-process termination. */
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    if (argc < 4) return 2;
    DWORD parent_pid = (DWORD)strtoul(argv[1], NULL, 10);
    HANDLE parent = OpenProcess(SYNCHRONIZE, FALSE, parent_pid);
    HANDLE normal = OpenEventA(SYNCHRONIZE, FALSE, argv[2]);
    if (!parent || !normal) return 3;
    HANDLE handles[2] = {normal, parent};
    DWORD result = WaitForMultipleObjects(2, handles, FALSE, INFINITE);
    if (result == WAIT_OBJECT_0 + 1) {
        FILE *log = fopen(argv[3], "a");
        if (log) {
            SYSTEMTIME now;
            GetLocalTime(&now);
            fprintf(log, "%04u-%02u-%02u %02u:%02u:%02u parent PID %lu ended unexpectedly\n",
                    now.wYear, now.wMonth, now.wDay, now.wHour, now.wMinute,
                    now.wSecond, (unsigned long)parent_pid);
            fclose(log);
        }
    }
    CloseHandle(parent);
    CloseHandle(normal);
    return 0;
}
