

set(CMAKE_TOOLCHAIN_FILE avr_crosscompile.cmake)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(i2cextender)

INCLUDE(avr_crosscompile.cmake)
IF(NOT CMAKE_CROSSCOMPILING)
#	ERROR()
ENDIF()

INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}
	#${CMAKE_CURRENT_SOURCE_DIR}/src
	#/usr/share/arduino/hardware/arduino/avr/cores/arduino/
	#/usr/share/arduino/hardware/arduino/avr/libraries/Wire/src/
)

ADD_EXECUTABLE(i2cextender.elf
	src/main.cpp
	src//Servo8Bit.cpp
	src/TinyWireS.cpp
	src/usiTwiSlave.c

)

ADD_CUSTOM_COMMAND(TARGET i2cextender.elf POST_BUILD
	COMMAND ${CMAKE_OBJCOPY} -O ihex -R .eeprom i2cextender.elf i2cextender.hex
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# for fuse settings see http://www.engbedded.com/fusecalc/
SET(LFUSE 0xe1)
SET(HFUSE 0xdd)
#SET(HFUSE 0x5d)  # reset disabled -> use with care
SET(EFUSE 0xff)

ADD_CUSTOM_TARGET(flash
			avrdude -P ${AVRDUDE_PORT} -p ${AVRDUDE_DEVICE} -c stk500v2 -Uflash:w:i2cextender.elf -Ulfuse:w:${LFUSE}:m -Uhfuse:w:${HFUSE}:m -Uefuse:w:${EFUSE}:m
	DEPENDS i2cextender.elf
)
