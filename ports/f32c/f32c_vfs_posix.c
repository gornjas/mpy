#include <sys/statvfs.h>

#include <fatfs/ff.h>


DIR *
opendir(const char *path) {
	DIR *dp = NULL;

	f_opendir(dp, path);
	return dp;
};

struct dirent *
readdir(DIR *dirp) {
        FILINFO fno;
	struct dirent *de = NULL;

	f_readdir(dirp, &fno);
	return de; /* XXX FIXME */
};

int
closedir(DIR *dirp) {

	return f_closedir(dirp);
};

int
fsync(int fd) {

	return 0; /* XXX FIXME! */
};

int
statvfs(const char * restrict path, struct statvfs * restrict buf) {

	return 0; /* XXX FIXME! */
};
