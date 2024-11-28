/*************************************************************************************************************
 * �ļ���:			stm32f4_flash.h
 * ����:			STM32F4 �ڲ�FLASH�����������
 * ����:			cp1300@139.com
 * ����ʱ��:		2013-10-20
 * ����޸�ʱ��:	2018-01-13
 * ��ϸ:			����STM32F4�ڲ�flash��д����
					2018-01-13:��ʼ�汾
*************************************************************************************************************/
#ifndef __MY_FLASH_H__
#define __MY_FLASH_H__
//#include "system.h"  
#include "board.h"
 
//FLASH��ʼ��ַ
#define STM32_FLASH_BASE 0x08000000 		//STM32 FLASH����ʼ��ַ
//FLASH������ֵ
#define flash_KEY1               0X45670123
#define flash_KEY2               0XCDEF89AB
 
//STM32F4 Flash����״̬
typedef enum
{
	STM32FLASH_OK			=	0,	//�������
	STM32FLASH_BUSY			=	1,	//æ
	STM32FLASH_WPRERR		=	2,	//д��������
	STM32FLASH_PGAERR		=	3,	//��̶�����󣬱���128λ����
	STM32FLASH_PGPERR		=	4,	//����λ������
	STM32FLASH_PGSERR		=	5,	//���˳�����
}STM32FLASH_STATUS;
 
 
 
//��ؽӿں���
void STM32FLASH_Unlock(void);															//FLASH����
void STM32FLASH_Lock(void);					  											//FLASH����
STM32FLASH_STATUS STM32FLASH_GetStatus(void);				  							//���״̬
STM32FLASH_STATUS STM32FLASH_EraseSector(u8 sectoraddr);			  					//��������
STM32FLASH_STATUS STM32FLASH_WriteWord(u32 faddr, u32 data);							//��FLASHָ����ַд��һ���ֵ����ݣ�32bitд�룩 �����棺��ַ������4����������
u32 STM32FLASH_ReadWord(u32 faddr);														//��ȡָ����ַ��һ����(32λ����) �����棺��ַ������4����������
STM32FLASH_STATUS STM32FLASH_Write_NoCheck(u32 WriteAddr,u32 *pBuffer,u16 NumToWrite) ; //������д�� 
STM32FLASH_STATUS STM32FLASH_Write(u32 WriteAddr,u32 *pBuffer,u16 NumToWrite);			//��ָ����ַ��ʼд��ָ�����ȵ����ݣ����ݳ�����Ϊ��λ��
u16 STM32FLASH_Read(u32 ReadAddr,u32 *pBuffer,u16 NumToRead);							//��ָ����ַ��ʼ����ָ�����ȵ�����
 
 
 
#endif	//__STM32F4_FLASH_H__
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 