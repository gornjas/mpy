#include <time.h>

static void
mp_machine_idle(void)
{
}

NORETURN static void
mp_machine_reset(void)
{
	exit (0);
}

static mp_int_t
mp_machine_reset_cause(void)
{

	return 0;
}

static mp_obj_t
mp_machine_unique_id(void)
{
	uint8_t id = 0;

	return mp_obj_new_bytes(&id, 1);
}

static mp_obj_t
mp_machine_get_freq(void)
{
	return mp_obj_new_int(get_cpu_freq());
}

static void
mp_machine_set_freq(size_t n_args, const mp_obj_t *args)
{

	mp_raise_ValueError(MP_ERROR_TEXT("frequency is read-only"));
}

static void
mp_machine_lightsleep(size_t n_args, const mp_obj_t *args)
{
}

NORETURN static void
mp_machine_deepsleep(size_t n_args, const mp_obj_t *args)
{

	mp_machine_reset();
}
