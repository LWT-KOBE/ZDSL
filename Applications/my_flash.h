/*************************************************************************************************************
 * 文件名:			stm32f4_flash.h
 * 功能:			STM32F4 内部FLASH编程驱动函数
 * 作者:			cp1300@139.com
 * 创建时间:		2013-10-20
 * 最后修改时间:	2018-01-13
 * 详细:			用于STM32F4内部flash读写驱动
					2018-01-13:初始版本
*************************************************************************************************************/
#ifndef __MY_FLASH_H__
#define __MY_FLASH_H__
//#include "system.h"  
#include "board.h"
 
//FLASH起始地址
#define STM32_FLASH_BASE 0x08000000 		//STM32 FLASH的起始地址
//FLASH解锁键值
#define flash_KEY1               0X45670123
#define flash_KEY2               0XCDEF89AB
 
//STM32F4 Flash操作状态
typedef enum
{
	STM32FLASH_OK			=	0,	//操作完成
	STM32FLASH_BUSY			=	1,	//忙
	STM32FLASH_WPRERR		=	2,	//写保护错误
	STM32FLASH_PGAERR		=	3,	//编程对齐错误，必须128位对齐
	STM32FLASH_PGPERR		=	4,	//并行位数错误
	STM32FLASH_PGSERR		=	5,	//编程顺序错误
}STM32FLASH_STATUS;
 
 
 
//相关接口函数
void STM32FLASH_Unlock(void);															//FLASH解锁
void STM32FLASH_Lock(void);					  											//FLASH上锁
STM32FLASH_STATUS STM32FLASH_GetStatus(void);				  							//获得状态
STM32FLASH_STATUS STM32FLASH_EraseSector(u8 sectoraddr);			  					//擦除扇区
STM32FLASH_STATUS STM32FLASH_WriteWord(u32 faddr, u32 data);							//在FLASH指定地址写入一个字的数据（32bit写入） 【警告：地址必须是4的整数倍】
u32 STM32FLASH_ReadWord(u32 faddr);														//读取指定地址的一个字(32位数据) 【警告：地址必须是4的整数倍】
STM32FLASH_STATUS STM32FLASH_Write_NoCheck(u32 WriteAddr,u32 *pBuffer,u16 NumToWrite) ; //不检查的写入 
STM32FLASH_STATUS STM32FLASH_Write(u32 WriteAddr,u32 *pBuffer,u16 NumToWrite);			//从指定地址开始写入指定长度的数据（数据长度字为单位）
u16 STM32FLASH_Read(u32 ReadAddr,u32 *pBuffer,u16 NumToRead);							//从指定地址开始读出指定长度的数据
 
 
 
#endif	//__STM32F4_FLASH_H__
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 