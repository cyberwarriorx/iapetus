project(iapetus)

enable_language(ASM)
set(iapetus_SOURCES
	ar/arcomm.c ar/commlink.c ar/crc.c ar/usbdev.c
	cd/cd.c cd/cdfs.c cd/mpeg.c cd/cdsect.c
	debug/debug.c debug/remoteex.s debug/localex.s
	scu/dsp.c scu/scudma.c scu/extram.c
	peripherals/smpc.c
	sh2/sci.c sh2/sh2dma.c sh2/sh2int.c sh2/timer.c
	sound/sound.c
	sys/mem/free.c sys/mem/free_r.c
	sys/mem/malloc.c sys/mem/malloc_r.c
	sys/mem/memb.c
	sys/mem/realloc_r.c
	sys/mem/slob.c sys/mem/slob.h
	sys/stdin.c sys/stdout.c sys/stderr.c sys/cd.c
	sys/bup.c sys/open.c sys/close.c
	sys/write.c sys/read.c sys/fstat.c sys/lseek.c 
	sys/sys.c
	ui/font.c ui/font8x8.c ui/font8x16.c ui/gui.c ui/text.c
	video/vdpinit.c video/scrscr.c video/rotscr.c video/vdp1.c video/pal.c
	iapetus.c)

set(CMAKE_C_FLAGS "-O2 -Wall -m2 -DREENTRANT_SYSCALLS_PROVIDED")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_library(${PROJECT_NAME} STATIC ${iapetus_SOURCES})

install(TARGETS ${PROJECT_NAME} DESTINATION "lib")
install(DIRECTORY "" DESTINATION "include" FILES_MATCHING PATTERN "*.h")

add_subdirectory(examples)