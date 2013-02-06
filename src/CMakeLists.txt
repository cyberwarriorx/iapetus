project(iapetus)

set(iapetus_SOURCES
	ar/arcomm.c ar/commlink.c
	cd/cd.c cd/cdfs.c cd/mpeg.c
	debug/debug.c debug/remoteex.s debug/localex.s
	scu/dsp.c scu/dmascu.c
	peripherals/smpc.c
	sh2/sh2dma.c sh2/sh2int.c sh2/timer.c
	sys/stdin.c sys/stdout.c sys/stderr.c sys/cd.c
	sys/bup.c sys/open.c sys/close.c sys/write.c
	sys/read.c sys/fstat.c sys/lseek.c sys/sys.c
	ui/font.c ui/font8x8.c ui/font8x16.c ui/gui.c ui/text.c
	video/vdpinit.c video/scrscr.c video/rotscr.c video/vdp1.c video/pal.c)


set(CMAKE_C_FLAGS "-O2 -Wall -m2 -DREENTRANT_SYSCALLS_PROVIDED")

ADD_LIBRARY(${PROJECT_NAME} STATIC ${iapetus_SOURCES})

#INSTALL(TARGETS ${PROJECT_NAME} ARCHIVE DESTINATION lib)
