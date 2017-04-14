#include "process_control.h"
#include <stdio.h>
#include <string.h>
#include <sys/types.h> /* about files */
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <iostream>

#define ARRAYSIZE(a) (sizeof(a) / sizeof(*(a)))
#define MAX_PATH_LEN    1024
#define MAX_FILENAME_LEN 256

static int WriteLock(int fd, off_t offset, int whence, off_t len)
{

	struct flock lock;

	lock.l_type = F_WRLCK; /* F_RDLCK, F_WRLCK, F_UNLCK */

	lock.l_start = offset; /* byte offset, relative to l_whence */

	lock.l_whence = whence; /* SEEK_SET, SEEK_CUR, SEEK_END */

	lock.l_len = len; /* #bytes (0 means to EOF) */

	return (fcntl(fd, F_SETLK, &lock));
}


/// @brief 获取当前程序名
static const char* GetSelfName()
{
    static char filename[MAX_FILENAME_LEN] = {0};
    if (0 == filename[0]) {
        char path[64]   = {0};
        char link[MAX_PATH_LEN] = {0};

        snprintf(path, ARRAYSIZE(path), "/proc/%d/exe", getpid());
        readlink(path, link, ARRAYSIZE(link));

        strncpy(filename, strrchr(link, '/') + 1, ARRAYSIZE(filename));
    }

    return filename;
}


bool ProcessControl::AlreadyRunning()
{
    int fd;
    char buf[32] = {0};
    char lock_file[100] = {0};

    sprintf(lock_file, "./run/%s.pid", GetSelfName());

    fd = open(lock_file, O_RDWR|O_CREAT, 0644);
    if (fd < 0) {
        //std::cout << "Iridium license daemon start fail!");
        std::cerr << "Iridium license daemon start fail!" << std::endl;
        return false;
    }

    /* 加建议性写锁 */
    if (WriteLock(fd,  0, SEEK_SET, 0) < 0) {
        close(fd);
        return false;
    }

    ftruncate(fd, 0); /* 文件清零，因为只写一次pid号 */

    snprintf(buf, 32, "%ld", (long)getpid());

    write(fd, buf, strlen(buf)+1);

    return true;
}




/* fcntl函数的包装，带w表示阻塞调用*/
/*
#define read_lock(fd, offset, whence, len) lock_reg((fd), F_SETLK, F_RDLCK, (offset), (whence), (len))

#define readw_lock(fd, offset, whence, len) lock_reg((fd), F_SETLKW, F_RDLCK, (offset), (whence), (len))

#define write_lock(fd, offset, whence, len) lock_reg((fd), F_SETLK, F_WRLCK, (offset), (whence), (len))

#define writew_lock(fd, offset, whence, len) lock_reg((fd), F_SETLKW, F_WRLCK, (offset), (whence), (len))

#define un_lock(fd, offset, whence, len) lock_reg((fd), F_SETLK, F_UNLCK, (offset), (whence), (len))

*/
