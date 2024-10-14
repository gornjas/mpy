#include <unistd.h>

#ifdef F32C
#include <dev/io.h>
#endif

static inline void mp_hal_set_interrupt_char(char c) {
}

#define RAISE_ERRNO(err_flag, error_val) \
    { if (err_flag == -1) \
      { mp_raise_OSError(error_val); } }

static inline void mp_hal_delay_us(mp_uint_t us) {
//    usleep(us);
}

#ifdef F32C
static inline mp_uint_t mp_hal_ticks_cpu(void) {
    mp_uint_t ticks;

    RDTSC(ticks);
    return ticks;
}
#else
#define mp_hal_ticks_cpu() 0
#endif
