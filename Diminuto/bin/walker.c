/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2020 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 *
 * ABSTRACT
 *
 * walker walks the file system tree at the specified root or by default
 * at the current directory and displays the tree on standard output and
 * the attributes of each entry on standard error.
 *
 * USAGE
 *
 * walker [ root [ root ... ] ]
 *
 * EXAMPLES
 *
 * walker
 * walker .
 * walker /
 * walker foo/bar
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <unistd.h>
#include <dirent.h>
#include <limits.h>
#include <errno.h>
#include <assert.h>
#include "com/diag/diminuto/diminuto_fd.h"

static const int DEBUG = 0;

static int walk(const char * name, char * path, size_t total, size_t depth)
{
    DIR * dp = (DIR *)0;
    struct dirent * ep = (struct dirent *)0;
    struct stat status = { 0 };
    int rc = 0;
    size_t dd = 0;
    size_t prior = 0;
    size_t length = 0;

    /*
     * If we're at the root of the tree, initialize the path buffer.
     */

    if (depth == 0) {
        path[0] = '\0';
        path[PATH_MAX - 1] = '\0';
        total = 0;
    }

    /*
     * Insure the path buffer has sufficient room. I'd be surprised
     * if this failed on a modern system, but back when MAXPATHLEN
     * was 512 I have seen file systems for which an absolute path
     * string could not be represented.
     */

    length = strnlen(name, PATH_MAX);
    if (length == 0) {
        errno = EINVAL;
        perror(name);
        return -1;
    } else if ((total + 1 /* '/' */ + length + 1 /* '\0' */) > PATH_MAX) {
        errno = E2BIG;
        perror(path);
        return -1;
    } else {
        /* Do nothing. */
    }

    if (DEBUG) { fprintf(stderr, "%s@%d: \"%s\" [%zu] \"%s\" [%zu]\n", __FILE__, __LINE__, path, total, name, length); }

    /*
     * Contstruct a path (maybe be relative or absolute depending
     * on the root).
     */

    prior = total;
    if (total == 0) {
        /* Do nothing. */
    } else if (path[total - 1] == '/') {
        /* Do nothing. */
    } else {
        path[total++] = '/';
        path[total] = '\0';
    }
    strcat(path, name);
    total += length;

    if (DEBUG) { fprintf(stderr, "%s@%d: \"%s\" [%zu]\n", __FILE__, __LINE__, path, total); }

    /*
     * Get the attributes for the file identified by the path.
     */

    rc = lstat(path, &status);
    if (rc >= 0) {
        /* Do nothing. */
    } else if ((errno == EACCES) || (errno == ENOENT)) {
        perror(path);
        path[prior] = '\0';
        return 0;
    } else {
        perror(path);
        return -2;
    }

    /*
     * Display the file in the tree on stdout, and its attribytes
     * on stderr.
     */

    for (dd = 0; dd < depth; ++dd) {
        fputc(' ', stdout);
    }
    fputs(name, stdout);
    if (name[length - 1] == '/') {
        /* Do nothing. */
    } else if (!S_ISDIR(status.st_mode)) {
        /* Do nothing. */
    } else {
        fputc('/', stdout);
    }
    fputc('\n', stdout);

    /*
     * N.B. Files with the same inode numbers are hardlinked to one
     * another: they are aliases for the same file.
     */

    fprintf(stderr,
        "%lu '%c' 0%o (%u,%u) [%zu] %u:%u <%u,%u> [%zu] [%zu] [%zu] %ld.%09lu %ld.%09lu %ld.%09lu \"%s\"\n"
        , status.st_ino
        , diminuto_fd_mode2type(status.st_mode)
        , (status.st_mode & ~S_IFMT)
        , major(status.st_dev), minor(status.st_dev)
        , (size_t)status.st_nlink
        , status.st_uid
        , status.st_gid
        , major(status.st_rdev), minor(status.st_rdev)
        , (size_t)status.st_size
        , (size_t)status.st_blksize
        , (size_t)(status.st_blocks * 512)
        , status.st_atim.tv_sec, (unsigned long)status.st_atim.tv_nsec
        , status.st_mtim.tv_sec, (unsigned long)status.st_mtim.tv_nsec
        , status.st_ctim.tv_sec, (unsigned long)status.st_ctim.tv_nsec
        , path /* May contain spaces or other problematic characters. */
    );

    /*
     * If a flat file, we're done; if a directory, recurse and descend.
     */

    if (S_ISDIR(status.st_mode)) {

        dp = opendir(path);
        if (dp != (DIR *)0) {
            /* Do nothing. */
        } else if ((errno == EACCES) || (errno == ENOENT)) {
            perror(path);
            path[prior] = '\0';
            return 0;
        } else {
            perror(path);
            return -3;
        }

        depth += 1;

        while (!0) {

            errno = 0;
            if ((ep = readdir(dp)) != (struct dirent *)0) {
                /* Do nothing. */
            } else if (errno == 0) {
                break;
            } else {
                perror(path);
                return -4;
            }

            if (strcmp(ep->d_name, "..") == 0) {
                /* Do ntohing. */
            } else if (strcmp(ep->d_name, ".") == 0) {
                /* Do ntohing. */
            } else if ((rc = walk(ep->d_name, path, total, depth)) == 0) {
                /* Do ntohing. */
            } else {
                return rc;
            }

        }

        if (closedir(dp) < 0) {
            perror(path);
            return -5;
        }

    }

    path[prior] = '\0';
    if (DEBUG) { fprintf(stderr, "%s@%d: \"%s\" [%zu]\n", __FILE__, __LINE__, path, prior); }

    return 0;
}

int main(int argc, char * argv[])
{
    int xc = 0;
    int rc = 0;
    int ii = 0;
    char path[PATH_MAX] = { '\0', };

    if (argc <= 1) {
        xc = walk(".", path, 0, 0);
    } else {
        for (ii = 1; ii < argc; ++ii) {
            rc = walk(argv[ii], path, 0, 0);
            if (xc == 0) {
                xc = rc;
            }
        }
    }

    return (xc < 0) ? -xc : xc;
}
