#include <sys/reent.h>

#include <stdlib.h>

void *_malloc_r(struct _reent *r __attribute__ ((unused)), size_t n)
{
   return NULL;
}

void * _realloc_r(struct _reent *r __attribute__ ((unused)),
           void *old __attribute__ ((unused)),
           size_t new_len __attribute__ ((unused)))
{
   return NULL;
}

void _free_r(struct _reent *r __attribute__ ((unused)), void *addr)
{
}
