#include <stdio.h>
#include <unistd.h>
#include "py/mpconfig.h"

// Receive single character
int mp_hal_stdin_rx_chr(void) {
    unsigned char c;
    c = getchar();
    if (c < 0)
	exit(0);
    return c;
}

// Send string of given length
void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
    while (len--) {
	putchar(*str++);
    }
}
