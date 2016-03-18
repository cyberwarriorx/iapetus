project(iapetus C)
include(ExternalProject)

if (IAP_WANT_SNDDRV)
	# Build 68K Sound Driver code
	ExternalProject_Add(sound_driver
		DOWNLOAD_COMMAND ""
		SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/sound/driver
		CMAKE_GENERATOR "Unix Makefiles"
        CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE=${68K_CMAKE_PLATFORM}
                   -D68K_PATHS=${68K_PATHS}
                   -DCMAKE_BIN2C=${CMAKE_BIN2C} 
		INSTALL_COMMAND ${CMAKE_BIN2C} ${CMAKE_CURRENT_BINARY_DIR}/sound/driver/sound_driver.bin ${CMAKE_CURRENT_SOURCE_DIR}/sound/driver.c sound_driver
		BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/sound/driver)
endif()

enable_language(ASM)
set(iapetus_SOURCES
	ar/arcomm.c ar/commlink.c ar/crc.c ar/usbdev.c
	cd/cd.c cd/cdfs.c cd/mpeg.c cd/cdsect.c
	debug/debug.c debug/remoteex.s debug/localex.s
	file/pcx.c file/image.c
	scu/dsp.c scu/scudma.c scu/extram.c
	peripherals/smpc.c
	sh2/sci.c sh2/sh2dma.c sh2/sh2int.c sh2/timer.c
	sound/sound.c
	sys/mem/free.c sys/mem/free_r.c
	sys/mem/malloc.c sys/mem/malloc_r.c
	sys/mem/memb.c
	sys/mem/realloc.c sys/mem/realloc_r.c
	sys/mem/slob.c sys/mem/slob.h
	sys/stdin.c sys/stdout.c sys/stderr.c sys/cd.c
	sys/bup.c sys/open.c sys/close.c
	sys/write.c sys/read.c sys/fstat.c sys/lseek.c 
	sys/sys.c
	ui/font.c ui/font8x8.c ui/font8x16.c ui/gui.c ui/text.c
	video/vdpinit.c video/coloffset.c video/scrscr.c video/rotscr.c video/vdp1.c video/pal.c
	iapetus.c)

if (IAP_WANT_SNDDRV)
	set(iapetus_SOURCES ${iapetus_SOURCES}
		sound/driver.c)
endif()

set(CMAKE_C_FLAGS "-O2 -Wall -m2")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_library(${PROJECT_NAME} STATIC ${iapetus_SOURCES})

if (IAP_WANT_SNDDRV)
    add_dependencies(${PROJECT_NAME} sound_driver)
endif()

install(TARGETS ${PROJECT_NAME} DESTINATION "lib")
install(DIRECTORY "" DESTINATION "include" FILES_MATCHING PATTERN "*.h")

add_subdirectory(examples)