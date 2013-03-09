//Board = Arduino Uno
#define ARDUINO 103
#define __AVR_ATmega328P__
#define F_CPU 16000000L
#define __AVR__
#define __cplusplus
#define __attribute__(x)
#define __inline__
#define __asm__(x)
#define __extension__
#define __ATTR_PURE__
#define __ATTR_CONST__
#define __inline__
#define __asm__ 
#define __volatile__
#define __builtin_va_list
#define __builtin_va_start
#define __builtin_va_end
#define __DOXYGEN__
#define prog_void
#define PGM_VOID_P int
#define NOINLINE __attribute__((noinline))

typedef unsigned char byte;
extern "C" void __cxa_pure_virtual() {}

unsigned int GetTableDistance();
void EnterDimmingMode();
void EnterColorTempMode();
void ShutdownLight();
//already defined in arduno.h
//already defined in arduno.h
int BH1750_Read(int address);
void BH1750_Init(int address);

#include "D:\arduino-1.0.3\hardware\arduino\variants\standard\pins_arduino.h" 
#include "D:\arduino-1.0.3\hardware\arduino\cores\arduino\arduino.h"
#include "D:\arduino-1.0.3\prj\sonar_light\sonar_light.ino"
