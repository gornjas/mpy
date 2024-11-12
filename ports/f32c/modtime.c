#include <time.h>

#include "py/mphal.h"
#include "py/runtime.h"

#if MICROPY_PY_TIME_TIME_TIME_NS
static mp_obj_t mp_time_time_get(void) {
    struct timespec tv;

    clock_gettime(CLOCK_MONOTONIC, &tv);

    mp_float_t val = tv.tv_sec + (mp_float_t)tv.tv_nsec / 1000000000;
    return mp_obj_new_float(val);
}
#endif
