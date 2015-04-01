#include <sys/reent.h>
#include "slob.h"
#include "../../iapetus.h"

void *realloc(void *old, size_t new_len)
{
	return slob_realloc(old, new_len);
}