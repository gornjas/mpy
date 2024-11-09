#include <sys/statvfs.h>

#include <fatfs/ff.h>

int
chdir(const char *path) {
	return f_chdir(path);
}

char *
getcwd(char *buf, size_t size) {
	f_getcwd(buf, size);
	return buf;
};

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
mkdir(const char *path, mode_t mode) {
	return f_mkdir(path);
};

int
rmdir(const char *path) {
	return f_rmdir(path);
};

int
rename(const char *from, const char *to) {
	return f_rename(from, to);
};

int
fsync(int fd) {
	return 0; /* XXX FIXME! */
};

int
statvfs(const char * restrict path, struct statvfs * restrict buf) {
	return 0; /* XXX FIXME! */
};
