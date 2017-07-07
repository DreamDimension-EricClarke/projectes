#ifndef __DN_STM32F103_MAP_H__
#define __DN_STM32F103_MAP_H__

/*STM32F103存储器中,内存空间的起始地址*/
#define MAP_SRAM_BASE 			0x20000000
/*STM32F103存储器中,内存空间位带区起始地址*/
#define MAP_SRAM_BITBAND_BASE   0x22000000

/*STM32F103存储器中,外设寄存器的起始地址*/
#define MAP_PERIPH_BASE 		0x40000000
/*STM32F103存储器中,外设寄存器位带区起始地址*/
#define MAP_PERIPH_BITBAND_BASE 0x42000000

/*通过位带区对SRAM空间进行原子操作*/
#define GetSRAMBit( by, bi ) 		(*((u32*)(MAP_SRAM_BITBAND_BASE + (by*8 + bi)*32)))
#define SetSRAMBit( by, bi ) 		(*((u32*)(MAP_SRAM_BITBAND_BASE + (by*8 + bi)*32)) = 1)
#define RstSRAMBit( by, bi ) 		(*((u32*)(MAP_SRAM_BITBAND_BASE + (by*8 + bi)*32)) = 0)

/*通过位带区对外设寄存器空间进行原子操作*/
#define GetPeriphBit( by, bi ) 		(*((u32*)(MAP_PERIPH_BITBAND_BASE + (by*8 + bi)*4)))
#define SetPeriphBit( by, bi ) 		(*((u32*)(MAP_PERIPH_BITBAND_BASE + (by*8 + bi)*4)) = 1)
#define RstPeriphBit( by, bi ) 		(*((u32*)(MAP_PERIPH_BITBAND_BASE + (by*8 + bi)*4)) = 0)

#endif
