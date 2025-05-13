#include <stdio.h>

#include "board.h"
#include "periph_conf.h"

int main(void) {
    while (1) {
        // Sleep for 12MHz
        LED0_ON;
        xosc_sleep(12000000000);
    }

    return 0;
}
