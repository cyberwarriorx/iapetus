#include <sys/reent.h>
#include <stddef.h>
#include "../../iapetus.h"

extern font_struct main_font;

void *_realloc_r(struct _reent *r __attribute__ ((unused)), void *old, size_t new_len)
{
	return realloc(old, new_len);
}
