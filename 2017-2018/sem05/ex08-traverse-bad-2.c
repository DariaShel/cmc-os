#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>

/*
 * Рекурсивный обход файловой системы с помощью telldir/seekdir
 * Недостаток: позиция, полученная с помощью telldir, может использоваться
 * для seekdir, пока каталог не был закрыт. Корректность пары telldir и seekdir
 * не гарантируется, если в промежутке каталог закрывается и открывается.
 * Поэтому эта программа не является правильной.
 */

int traverse(const char *dir)
{
    DIR *d = opendir(dir);
    if (!d) {
        fprintf(stderr, "error: %s\n", strerror(errno));
        return -1;
    }
    struct dirent *dd;
    char path[PATH_MAX];
    while ((dd = readdir(d))) {
        if (!strcmp(dd->d_name, ".") || !strcmp(dd->d_name, ".."))
            continue;
        int slen = snprintf(path, sizeof(path), "%s/%s", dir, dd->d_name);
        if (slen + 1 > sizeof(path)) {
            continue;
        }

        printf("%s %d %lu %s\n", path, slen, dd->d_ino, dd->d_name);
        struct stat stb;
        if (lstat(path, &stb) < 0) continue;
        printf("stat: %lu %llu\n", stb.st_ino, stb.st_dev);
        if (S_ISDIR(stb.st_mode)) {
            // так, как ниже, делать нельзя
            long cur = telldir(d);
            closedir(d);
            traverse(path);
            d = opendir(dir);
            seekdir(d, cur);
        }
    }
    closedir(d);
    return 0;
}

int main(int argc, char *argv[])
{
    traverse(argv[1]);
}
