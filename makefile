CC = sh-$(KPITOUTPUT)-gcc
AS = sh-$(KPITOUTPUT)-as
AR = sh-$(KPITOUTPUT)-ar

PIC = false

LIBDIR  = .
INCDIR  = .
CFLAGS  = -O2 -Wall -m2 -I$(INCDIR) -DREENTRANT_SYSCALLS_PROVIDED
ifeq ($(PIC),true)
   CFLAGS += -fpic
   LIBEXT = -pic
else
   LIBEXT =
endif
ASFLAGS =
ARFLAGS = rv

 OBJS = src/ar/arcomm.o src/ar/commlink.o \
         src/cd/cd.o src/cd/cdfs.o src/cd/mpeg.o \
        src/debug/debug.o src/debug/remoteex.o src/debug/localex.o \
        src/scu/dsp.o src/scu/dmascu.o \
        src/peripherals/smpc.o \
        src/sh2/sh2dma.o src/sh2/sh2int.o src/sh2/timer.o \
        src/sound/sound.o \
        src/sys/stdin.o src/sys/stdout.o src/sys/stderr.o src/sys/cd.o  \
        src/sys/bup.o src/sys/open.o src/sys/close.o src/sys/write.o \
        src/sys/read.o src/sys/fstat.o src/sys/lseek.o src/sys/sys.o \
        src/ui/font.o src/ui/font8x8.o src/ui/font8x16.o src/ui/gui.o src/ui/text.o \
        src/video/vdpinit.o src/video/scrscr.o src/video/rotscr.o src/video/vdp1.o src/video/pal.o \
        src/iapetus.o

TARGET = $(LIBDIR)/libiapetus.a

all: $(TARGET)

$(TARGET): $(OBJS)
	$(AR) $(ARFLAGS) $(_ARFLAGS) $@ $?

.SUFFIXES:
.SUFFIXES: .a .o .src .c

%.o: %.c
	$(CC) -c $(CFLAGS) $(_CFLAGS) -o $@ $<

%.o: %.s
	$(AS) $< $(ASFLAGS) $(_ASFLAGS) -o $@

clean:
	rm -Rf *.o
	rm -f $(TARGET)

install:
	cp libiapetus.a "$(PPATH)\KPIT Cummins\GNUSH$(KPITVER)-$(KPITOUTPUT)\sh-$(KPITOUTPUT)\lib\libiapetus$(LIBEXT).a"
	mkdir "$(PPATH)\KPIT Cummins\GNUSH$(KPITVER)-$(KPITOUTPUT)\sh-$(KPITOUTPUT)\sh-$(KPITOUTPUT)\include\iapetus"
	cp *.h "$(PPATH)\KPIT Cummins\GNUSH$(KPITVER)-$(KPITOUTPUT)\sh-$(KPITOUTPUT)\sh-$(KPITOUTPUT)\include\iapetus"
