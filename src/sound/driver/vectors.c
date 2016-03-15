typedef void (*ISR)();

extern char __stack[];
void _start();
void endfunc();

ISR vecTable[256] __attribute__ ((section ("VECTORS"))) =
{
(ISR)__stack, (ISR)_start, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc,
endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc,
endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc,
endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc,
endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc,
endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc,
endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc,
endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc,
endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc,
endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc,
endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc,
endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc,
endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc,
endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc,
endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc,
endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc,
endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc,
endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc,
endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc,
endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc,
endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc,
endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc,
endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc,
endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc,
endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc,
endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc,
endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc,
endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc,
endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc,
endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc,
endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc,
endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc, endfunc
};
