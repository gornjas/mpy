
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <netinet/in.h>

#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/tty.h>

#include "py/mpconfig.h"
#include "py/mphal.h"

#include "dprintf.h"

FILE debf = { ._fd = -1 };

/* Telnet commands and options, excerpt from IANA */

#define	TC_SE		240
#define	TC_SB		250
#define	TC_WILL		251
#define	TC_WONT		252
#define	TC_DO		253
#define	TC_DONT		254
#define	TC_IAC		255

#define	TO_ECHO		1
#define	TO_SGA		3
#define	TO_STATUS	5
#define	TO_TERM_TYPE	24
#define	TO_NAWS		31
#define	TO_TERM_SPEED	32
#define	TO_LINEMODE	34
#define	TO_ENVIRON	36
#define	TO_NEW_ENVIRON	39

static uint8_t tc_init[] = {
	TC_IAC, TC_WILL, TO_SGA,
	TC_IAC, TC_DONT, TO_ECHO,
	TC_IAC, TC_WILL, TO_ECHO,
};

static struct termios tc_termios = {
	//.c_lflag = ISIG,
	.c_iflag = IXON,
	.c_oflag = OPOST | ONLCR,
};

static struct file *
fd2fp(int fd)
{
    struct task *ts = TD_TASK(curthread);

    return ((struct file *) ts->ts_files[fd]);
}

static void
telnet_init(int fd)
{

	/* Tell the client to leave character echo and line editing to us */
	write(fd, tc_init, sizeof(tc_init));

	/* debf clones stdin / stdout / stderr */
	if (debf._fd < 0)
		debf._fd = fd_ref(fd2fp(stdin->_fd));

	/* Deref standard file descriptors */
	close(stdin->_fd);
	close(stdout->_fd);
	close(stderr->_fd);

	/* Promote the socket to a controlling TTY, enable termios processing */
	fcntl(fd, F_SETFL, O_TTY_INIT);
	fcntl(fd, IOCTL_TERMIOS | TIOCSETA, &tc_termios);

	/* Redirect stdin, stdout, stderr to the PTCP socket */
	stdin->_fd = fd;
	stdout->_fd = fd_ref(fd2fp(fd));
	stderr->_fd = fd_ref(fd2fp(fd));
}

static void
telnet_close(int fd)
{
	struct file *fp = fd2fp(fd);

	/* Free the tty */
	free(fp->f_tty);
	fp->f_tty = NULL;

	/* Deref PTCP file descriptors; last deref closes the socket */
	close(stdin->_fd);
	close(stdout->_fd);
	close(stderr->_fd);

	/* Redirect stdin, stdout, stderr back to SIO console */
	stdin->_fd = fd_ref(fd2fp(debf._fd));
	stdout->_fd = fd_ref(fd2fp(debf._fd));
	stderr->_fd = fd_ref(fd2fp(debf._fd));
}

const struct sockaddr_in sa_in = {
	.sin_len = sizeof(sa_in),
	.sin_family = PF_INET,
	.sin_port = 2023
};

int lso, cso;

static void
listen_socket(void)
{

	lso = socket(PF_INET, SOCK_STREAM, 0);
	if (lso < 0) {
		printf("socket() failed\n");
		return;
	}

	if (bind(lso, (struct sockaddr *) &sa_in, sa_in.sin_len) < 0) {
		printf("bind() failed\n");
		return;
	}

	if (listen(lso, 1) < 0) {
		printf("listen() failed\n");
		return;
	}

	printf("Listening on port %d\n", sa_in.sin_port);
}

// Receive single character
int
mp_hal_stdin_rx_chr(void) {
	socklen_t len = sizeof(struct sockaddr);
	int c;

	if (lso == 0)
		listen_socket();

	if (cso <= 0) {
		cso = accept(lso, (struct sockaddr *) &sa_in, &len);
		if (cso > 0) {
			telnet_init(cso);
			return(2); /* CTRL + B */
		}
	}

	c = getchar();
	if (c < 0 && cso > 0) {
		telnet_close(cso);
		cso = 0;
		return(3); /* CTRL + C */
	}

	return c;
}

// Send string of given length
mp_uint_t
mp_hal_stdout_tx_strn(const char *str, size_t len) {

    return(write(STDOUT_FILENO, str, len));
}
