project(iapetus)

enable_language(ASM)
set(iapetus_SOURCES
	ar/arcomm.c ar/commlink.c
	cd/cd.c cd/cdfs.c cd/mpeg.c
	debug/debug.c debug/remoteex.s debug/localex.s
	scu/dsp.c scu/scudma.c
	peripherals/smpc.c
	sh2/sh2dma.c sh2/sh2int.c sh2/timer.c
	sound/sound.c
	sys/stdin.c sys/stdout.c sys/stderr.c sys/cd.c
	sys/bup.c sys/open.c sys/close.c sys/mem.c
	sys/write.c sys/read.c sys/fstat.c sys/lseek.c 
	sys/sys.c
	ui/font.c ui/font8x8.c ui/font8x16.c ui/gui.c ui/text.c
	video/vdpinit.c video/scrscr.c video/rotscr.c video/vdp1.c video/pal.c
	iapetus.c)

set(CMAKE_C_FLAGS "-O2 -Wall -m2 -DREENTRANT_SYSCALLS_PROVIDED")

add_library(${PROJECT_NAME} STATIC ${iapetus_SOURCES})

install(TARGETS ${PROJECT_NAME} DESTINATION "lib")
install(DIRECTORY "" DESTINATION "include" FILES_MATCHING PATTERN "*.h")