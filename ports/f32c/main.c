#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include <sys/file.h>
#include <sys/tty.h>

#ifdef F32C
#include <fatfs/ff.h>
#endif

#include "py/builtin.h"
#include "py/compile.h"
#include "py/runtime.h"
#include "py/repl.h"
#include "py/gc.h"
#include "py/mphal.h"
#include "py/mperrno.h"
#include "shared/readline/readline.h"
#include "shared/runtime/pyexec.h"

#include "extmod/vfs.h"
#include "extmod/vfs_posix.h"

#if MICROPY_ENABLE_COMPILER
void do_str(const char *src, mp_parse_input_kind_t input_kind) {
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_lexer_t *lex = mp_lexer_new_from_str_len(MP_QSTR__lt_stdin_gt_, src, strlen(src), 0);
        qstr source_name = lex->source_name;
        mp_parse_tree_t parse_tree = mp_parse(lex, input_kind);
        mp_obj_t module_fun = mp_compile(&parse_tree, source_name, true);
        mp_call_function_0(module_fun);
        nlr_pop();
    } else {
        // uncaught exception
        mp_obj_print_exception(&mp_plat_print, (mp_obj_t)nlr.ret_val);
    }
}
#endif

static char *stack_top;

#if MICROPY_ENABLE_GC
static char *heap;
#endif

int main(int argc, char **argv) {
    struct termios nterm;
    int stack_dummy;
    stack_top = (char *)&stack_dummy;

    tcgetattr(0, &nterm);
    nterm.c_lflag &= ~(ECHO|ECHOK|ECHONL|ICANON);
    nterm.c_iflag &= ~(IGNCR|INLCR|ICRNL);
    nterm.c_iflag |= ISTRIP;
    nterm.c_oflag &= ~(ONLCR);
    tcsetattr(0, TCSADRAIN, &nterm);

    #if MICROPY_VFS_POSIX
    {
	char buf[FF_MAX_SS];

	/* XXX automount fatfs: otherwise "C:" won't be visible */
	getcwd(buf, sizeof(buf));

	// Format the RAM disk
	f_mkfs("r:", 0, buf, FF_MIN_SS);
    }
    #endif

    #if MICROPY_ENABLE_GC
    heap = malloc(MICROPY_HEAP_SIZE);
    #endif

soft_reset:
    #if MICROPY_ENABLE_GC
    gc_init(&heap[0], &heap[MICROPY_HEAP_SIZE]);
    #endif
    mp_init();
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_lib));
    readline_init0();

    #if MICROPY_VFS_POSIX
    {
	// Mount the host FS at the root of our internal VFS
	mp_obj_t args[2] = {
	    MP_OBJ_TYPE_GET_SLOT(
	      &mp_type_vfs_posix, make_new)(&mp_type_vfs_posix, 0, 0, NULL),
	    MP_OBJ_NEW_QSTR(MP_QSTR__slash_),
	};
	mp_vfs_mount(2, args, (mp_map_t *)&mp_const_empty_map);
	MP_STATE_VM(vfs_cur) = MP_STATE_VM(vfs_mount_table);
    }
    #endif

    // run boot-up scripts
    int ret = pyexec_file_if_exists("boot.py");
    if (ret & PYEXEC_FORCED_EXIT)
	goto soft_reset_exit;

    if (pyexec_mode_kind == PYEXEC_MODE_FRIENDLY_REPL && ret != 0) {
	int ret = pyexec_file_if_exists("main.py");
	if (ret & PYEXEC_FORCED_EXIT)
	    goto soft_reset_exit;
    }

    for (;;) {
	if (pyexec_mode_kind == PYEXEC_MODE_RAW_REPL) {
	    if (pyexec_raw_repl() != 0)
		break;
	} else {
	    if (pyexec_friendly_repl() != 0)
		break;
	}
    }

soft_reset_exit:

    gc_sweep_all();

    mp_hal_stdout_tx_str("MPY: soft reboot\r\n");

    mp_deinit();
    goto soft_reset;
}

#if MICROPY_ENABLE_GC
void gc_collect(void) {
    // WARNING: This gc_collect implementation doesn't try to get root
    // pointers from CPU registers, and thus may function incorrectly.
    void *dummy;
    gc_collect_start();
    gc_collect_root(&dummy, ((mp_uint_t)stack_top - (mp_uint_t)&dummy) / sizeof(mp_uint_t));
    gc_collect_end();
    gc_dump_info(&mp_plat_print);
}
#endif

void nlr_jump_fail(void *val) {
    printf("%s %d\n", __FUNCTION__, __LINE__);
    exit(1);
}

void NORETURN __fatal_error(const char *msg) {
    printf("%s %d\n", __FUNCTION__, __LINE__);
    exit(1);
}

#ifndef NDEBUG
void MP_WEAK __assert_func(const char *file, int line, const char *func, const char *expr) {
    printf("Assertion '%s' failed, at file %s:%d\n", expr, file, line);
    __fatal_error("Assertion failed");
}
#endif
