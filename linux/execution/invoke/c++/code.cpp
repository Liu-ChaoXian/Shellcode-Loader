/*
    Shellcode Loader
    Archive of Reversing.ID

    Executing shellcode as new thread.

Compile:
    $ g++ code.cpp

Technique:
    - allocation: mmap
    - writing:    memcpy
    - permission: mprotect
    - execution:  
*/

#include <sys/mman.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

int main ()
{
    void *  runtime;
    int     retval;

    // shellcode storage in stack
    uint8_t     payload []  = { 0x90, 0x90, 0xCC, 0xC3 };
    uint32_t    payload_len = 4;

    // allocate memory buffer for payload as READ-WRITE (no executable)
    runtime = mmap (0, payload_len, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);

    // copy payload to the buffer
    memcpy (runtime, payload, payload_len);

    // make buffer executable (R-X)
    retval  = mprotect (runtime, payload_len, PROT_READ|PROT_EXEC);
    if (retval == 0)
    {
        // create pointer to function and assign with address of shellcode
        int (*func)();
        func = (int (*)())runtime;

        // executing
        func();
    }

    // dealocate memory map
    munmap (runtime, payload_len);

    return 0;
}