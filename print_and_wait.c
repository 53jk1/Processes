// $ gcc print_and_wait.c -o print_and_wait
// $ ./print_and_wait

#include <stdio.h>
int main(void) {
    printf("Address: %p\n", "This is a sample string.");

    // The easiest way to pause execution.
    getchar();
    
    return 0;
}