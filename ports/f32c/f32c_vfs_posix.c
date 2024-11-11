#include <stdlib.h>
#include <stdio.h>

#include <sys/statvfs.h>

#include <fatfs/ff.h>


int
fsync(int fd) {

	return 0; /* XXX FIXME! */
};


int
statvfs(const char * restrict path, struct statvfs * restrict buf) {

	return 0; /* XXX FIXME! */
};
