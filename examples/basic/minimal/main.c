#include <stdio.h>

#include "board.h"
#include "periph_conf.h"

int main(void) {
    printf("Hello, World!\n");

    while (1) {
        for (int i = 0; i < 750000; i++) __NOP();
    }

    return 0;
}
