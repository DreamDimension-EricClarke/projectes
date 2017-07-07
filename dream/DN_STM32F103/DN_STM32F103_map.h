#ifndef __DN_STM32F103_MAP_H__
#define __DN_STM32F103_MAP_H__

/*STM32F103�洢����,�ڴ�ռ����ʼ��ַ*/
#define MAP_SRAM_BASE 			0x20000000
/*STM32F103�洢����,�ڴ�ռ�λ������ʼ��ַ*/
#define MAP_SRAM_BITBAND_BASE   0x22000000

/*STM32F103�洢����,����Ĵ�������ʼ��ַ*/
#define MAP_PERIPH_BASE 		0x40000000
/*STM32F103�洢����,����Ĵ���λ������ʼ��ַ*/
#define MAP_PERIPH_BITBAND_BASE 0x42000000

/*ͨ��λ������SRAM�ռ����ԭ�Ӳ���*/
#define GetSRAMBit( by, bi ) 		(*((u32*)(MAP_SRAM_BITBAND_BASE + (by*8 + bi)*32)))
#define SetSRAMBit( by, bi ) 		(*((u32*)(MAP_SRAM_BITBAND_BASE + (by*8 + bi)*32)) = 1)
#define RstSRAMBit( by, bi ) 		(*((u32*)(MAP_SRAM_BITBAND_BASE + (by*8 + bi)*32)) = 0)

/*ͨ��λ����������Ĵ����ռ����ԭ�Ӳ���*/
#define GetPeriphBit( by, bi ) 		(*((u32*)(MAP_PERIPH_BITBAND_BASE + (by*8 + bi)*4)))
#define SetPeriphBit( by, bi ) 		(*((u32*)(MAP_PERIPH_BITBAND_BASE + (by*8 + bi)*4)) = 1)
#define RstPeriphBit( by, bi ) 		(*((u32*)(MAP_PERIPH_BITBAND_BASE + (by*8 + bi)*4)) = 0)

#endif
