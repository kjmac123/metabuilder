#ifndef REALPATH_H
#define REALPATH_H

namespace FreeBSD
{

/*
 * Find the real name of path, by removing all ".", ".." and symlink
 * components.  Returns (resolved) on success, or (NULL) on failure,
 * in which case the path which caused trouble is left in (resolved).
 */
char * realpath(const char * __restrict path, char * __restrict resolved);

}

#endif
