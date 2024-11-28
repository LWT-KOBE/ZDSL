/*************************************************************************************************************
 * �ļ���:			stm32f4_flash.c
 * ����:			STM32F4 �ڲ�FLASH�����������
 * ����:			cp1300@139.com
 * ����ʱ��:		2013-10-20
 * ����޸�ʱ��:	2018-01-13
 * ��ϸ:			����STM32F4�ڲ�flash��д����
					2018-01-13:��ʼ�汾��Ĭ��λ��Ϊ32λ��д������ݱ���32λ����,���������2.7-3.6V֮�䣬����ʹ��3.3V����
					2018-01-16:�޸���������ʱû�н�������
*************************************************************************************************************/
#include "my_flash.h"
//#include "system.h"
#include "bsp_flash.h"
 
//FLASH ��������ʼ��ַ
//#define ADDR_FLASH_SECTOR_0     ((u32)0x08000000) 	//����0��ʼ��ַ, 16 Kbytes  
//#define ADDR_FLASH_SECTOR_1     ((u32)0x08004000) 	//����1��ʼ��ַ, 16 Kbytes  
//#define ADDR_FLASH_SECTOR_2     ((u32)0x08008000) 	//����2��ʼ��ַ, 16 Kbytes  
//#define ADDR_FLASH_SECTOR_3     ((u32)0x0800C000) 	//����3��ʼ��ַ, 16 Kbytes  
//#define ADDR_FLASH_SECTOR_4     ((u32)0x08010000) 	//����4��ʼ��ַ, 64 Kbytes  
//#define ADDR_FLASH_SECTOR_5     ((u32)0x08020000) 	//����5��ʼ��ַ, 128 Kbytes  
//#define ADDR_FLASH_SECTOR_6     ((u32)0x08040000) 	//����6��ʼ��ַ, 128 Kbytes  
//#define ADDR_FLASH_SECTOR_7     ((u32)0x08060000) 	//����7��ʼ��ַ, 128 Kbytes  
//#define ADDR_FLASH_SECTOR_8     ((u32)0x08080000) 	//����8��ʼ��ַ, 128 Kbytes  
//#define ADDR_FLASH_SECTOR_9     ((u32)0x080A0000) 	//����9��ʼ��ַ, 128 Kbytes  
//#define ADDR_FLASH_SECTOR_10    ((u32)0x080C0000) 	//����10��ʼ��ַ,128 Kbytes  
//#define ADDR_FLASH_SECTOR_11    ((u32)0x080E0000) 	//����11��ʼ��ַ,128 Kbytes  
 
 
 
/*************************************************************************************************************************
* ����			:	void STM32FLASH_Unlock(void)
* ����			:	����STM32��FLASH
* ����			:	��
* ����			:	��
* ����			:	�ײ�
* ����			:	cp1300@139.com
* ʱ��			:	2013-10-20
* ����޸�ʱ�� 	: 	2013-10-20
* ˵��			: 	
*************************************************************************************************************************/
void STM32FLASH_Unlock(void)
{
	FLASH->KEYR=FLASH_KEY1;	//д���������.
	FLASH->KEYR=FLASH_KEY2;
}
 
 
 
/*************************************************************************************************************************
* ����			:	void STM32FLASH_Lock(void)
* ����			:	����STM32��FLASH
* ����			:	��
* ����			:	��
* ����			:	�ײ�
* ����			:	cp1300@139.com
* ʱ��			:	2013-10-20
* ����޸�ʱ�� 	: 	2018-01-13
* ˵��			: 	
*************************************************************************************************************************/
void STM32FLASH_Lock(void)
{
	FLASH->CR|=(u32)1<<31;			//����
}
 
 
 
 
/*************************************************************************************************************************
* ����			:	STM32FLASH_STATUS STM32FLASH_GetStatus(void)
* ����			:	�õ�FLASH״̬
* ����			:	��
* ����			:	STM32FLASH_STATUS
* ����			:	�ײ�
* ����			:	cp1300@139.com
* ʱ��			:	2013-10-20
* ����޸�ʱ�� 	: 	2018-01-03
* ˵��			: 	
*************************************************************************************************************************/
STM32FLASH_STATUS STM32FLASH_GetStatus(void)
{	
	u32 temp=FLASH->SR; 
	
	if(temp&(1<<16))return STM32FLASH_BUSY;			//æ
	else if(temp&(1<<4))return STM32FLASH_WPRERR;	//д��������
	else if(temp&(1<<5))return STM32FLASH_PGAERR;	//��̶������
	else if(temp&(1<<6))return STM32FLASH_PGPERR;	//����λ������
	else if(temp&(1<<7))return STM32FLASH_PGSERR;	//���˳�����
	return STM32FLASH_OK;							//�������
}
 
 
 
/*************************************************************************************************************************
* ����			:	STM32FLASH_STATUS STM32FLASH_WaitDone(u32 time)
* ����			:	�ȴ��������
* ����			:	time:Ҫ��ʱ�ĳ���,��λus
* ����			:	STM32FLASH_STATUS
* ����			:	�ײ�
* ����			:	cp1300@139.com
* ʱ��			:	2013-10-20
* ����޸�ʱ�� 	: 	2018-01-03
* ˵��			: 	
*************************************************************************************************************************/
STM32FLASH_STATUS STM32FLASH_WaitDone(u32 time)
{
	STM32FLASH_STATUS status;
	
	if(time < 1) time = 1;
	do
	{
		status=STM32FLASH_GetStatus();			//��ȡ״̬
		if(status!=STM32FLASH_BUSY)break;		//��æ,����ȴ���,ֱ���˳�.
		//delay_US(1);
		time--;
	 }while(time);
 
	 return status;
}
 
 
 
/*************************************************************************************************************************
* ����			:	STM32FLASH_STATUS STM32FLASH_EraseSector(u8 sectoraddr)
* ����			:	��������
* ����			:	sectoraddr��������ַ��0-11
* ����			:	STM32FLASH_STATUS
* ����			:	�ײ�
* ����			:	cp1300@139.com
* ʱ��			:	2018-01-03
* ����޸�ʱ�� 	: 	2018-01-03
* ˵��			: 	������С��0~3��16KB����;4��64KB����;5~11��128KB����
					128KB�������������2��
					2018-01-16:�޸���������ʱû�н�������
*************************************************************************************************************************/
STM32FLASH_STATUS STM32FLASH_EraseSector(u8 sectoraddr)
{
	STM32FLASH_STATUS status;
	
	status=STM32FLASH_WaitDone(200000);		//�ȴ��ϴβ�����ɣ����ʱ��2�룬����Ѿ���������˻�ֱ������
	
	STM32FLASH_Unlock();					//���� 
	if(status==STM32FLASH_OK)				//û�д���
	{ 
		FLASH->CR&=~(3<<8);					//���PSIZEԭ��������
		FLASH->CR|=2<<8;					//����Ϊ32bit��,ȷ��VCC=2.7~3.6V֮��!!
		FLASH->CR&=~(0X1F<<3);				//���ԭ��������
		FLASH->CR|=sectoraddr<<3;			//����Ҫ���������� 
		FLASH->CR|=1<<1;					//�������� 
		FLASH->CR|=1<<16;					//��ʼ����			
		status=STM32FLASH_WaitDone(200000);	//�ȴ���������,���2s  
		if(status!=STM32FLASH_BUSY)			//��æ
		{
			FLASH->CR&=~(1<<1);				//�������������־.
		}
	}
	STM32FLASH_Lock();						//����
	
	return status;							//����״̬
}
 
 
 
 
/*************************************************************************************************************************
* ����			:	STM32FLASH_STATUS STM32FLASH_WriteWord(u32 addr, u32 data)
* ����			:	��FLASHָ����ַд��һ���ֵ����ݣ�32bitд�룩
* ����			:	faddr:ָ����ַ(�˵�ַ����Ϊ4�ı���!!);data:Ҫд�������
* ����			:	STM32FLASH_STATUS
* ����			:	�ײ�
* ����			:	cp1300@139.com
* ʱ��			:	2018-01-03
* ����޸�ʱ�� 	: 	2018-01-03
* ˵��			: 	���棬��ַ����Ϊ4�ı���
*************************************************************************************************************************/
STM32FLASH_STATUS STM32FLASH_WriteWord(u32 faddr, u32 data)
{
	STM32FLASH_STATUS status;
		
	status=STM32FLASH_WaitDone(200);	 	//�ж�״̬
	if(status==STM32FLASH_OK)				//û�д���
	{
		FLASH->CR&=~(3<<8);					//���PSIZEԭ��������
		FLASH->CR|=2<<8;					//����Ϊ32bit��,ȷ��VCC=2.7~3.6V֮��!!
 		FLASH->CR|=1<<0;					//���ʹ��
		*(vu32*)faddr=data;					//д������
		status=STM32FLASH_WaitDone(200);	//�ȴ��������,һ���ֱ��,���100us.
		if(status!=STM32FLASH_BUSY)			//��æ
		{
			FLASH->CR&=~(1<<0);				//���PGλ.
		}
	} 
	return status;
} 
 
 
 
/*************************************************************************************************************************
* ����			:	u32 STM32FLASH_ReadWord(u32 faddr)
* ����			:	��ȡָ����ַ��һ����(32λ����) 
* ����			:	faddr:ָ����ַ(�˵�ַ����Ϊ4�ı���!!);
* ����			:	����
* ����			:	�ײ�
* ����			:	cp1300@139.com
* ʱ��			:	2018-01-03
* ����޸�ʱ�� 	: 	2018-01-03
* ˵��			: 	���棺��ַ������4��������
*************************************************************************************************************************/
u32 STM32FLASH_ReadWord(u32 faddr)
{
	return *(vu32*)faddr; 
}
 
 
/*************************************************************************************************************************
* ����			:	u8 STM32FLASH_GetFlashSector(u32 faddr)
* ����			:	��ȡĳ����ַ���ڵ�����
* ����			:	faddr:ָ����ַ
* ����			:	ɾ����ţ�0-11�����������Ҳ�����Ƶ�11
* ����			:	�ײ�
* ����			:	cp1300@139.com
* ʱ��			:	2018-01-03
* ����޸�ʱ�� 	: 	2018-01-03
* ˵��			: 	��ַ�벻Ҫ������Χ
*************************************************************************************************************************/
u8 STM32FLASH_GetFlashSector(u32 faddr)
{
	if(faddr<ADDR_FLASH_SECTOR_1)		return 0;
	else if(faddr<ADDR_FLASH_SECTOR_2)	return 1;
	else if(faddr<ADDR_FLASH_SECTOR_3)	return 2;
	else if(faddr<ADDR_FLASH_SECTOR_4)	return 3;
	else if(faddr<ADDR_FLASH_SECTOR_5)	return 4;
	else if(faddr<ADDR_FLASH_SECTOR_6)	return 5;
	else if(faddr<ADDR_FLASH_SECTOR_7)	return 6;
	else if(faddr<ADDR_FLASH_SECTOR_8)	return 7;
	else if(faddr<ADDR_FLASH_SECTOR_9)	return 8;
	else if(faddr<ADDR_FLASH_SECTOR_10)	return 9;
	else if(faddr<ADDR_FLASH_SECTOR_11)	return 10; 
	else return 11;	
}
 
 
/*************************************************************************************************************************
* ����			:	STM32FLASH_STATUS STM32FLASH_Write_NoCheck(u32 WriteAddr,u32 *pBuffer,u16 NumToWrite)  
* ����			:	������д��
* ����			:	WriteAddr:��ʼ��ַ(������4��������);pBuffer:����ָ��;NumToWrite:��(32λ)�� 
* ����			:	״̬
* ����			:	�ײ�
* ����			:	cp1300@139.com
* ʱ��			:	2013-10-20
* ����޸�ʱ�� 	: 	2018-01-03
* ˵��			: 	��ַ����Ϊ4���룬��������˷�FFд�����ᱨ�����أ�����STM32Ӳ�����д��������м�飬��FF��������д�룩
*************************************************************************************************************************/
STM32FLASH_STATUS STM32FLASH_Write_NoCheck(u32 WriteAddr,u32 *pBuffer,u16 NumToWrite)   
{ 			 		 
	u16 i;
	STM32FLASH_STATUS status;
	
	if(WriteAddr<STM32_FLASH_BASE||WriteAddr%4)return STM32FLASH_PGPERR ;	//�Ƿ���ַ,�������4�ı���,���ز��д���
	STM32FLASH_Unlock();						//���� 
	for(i=0;i<NumToWrite;i++)
	{
		status = STM32FLASH_WriteWord(WriteAddr,pBuffer[i]);
		if(status != STM32FLASH_OK) break;		//д�����ֱ�ӷ���
	    WriteAddr+=4;							//��ַ����4.
	}
	STM32FLASH_Lock();							//����
	return status;
} 
 
 
 
 
/*************************************************************************************************************************
* ����			:	STM32FLASH_STATUS STM32FLASH_Write(u32 WriteAddr,u32 *pBuffer,u16 NumToWrite)	
* ����			:	��ָ����ַ��ʼд��ָ�����ȵ����ݣ����ݳ�����Ϊ��λ��
* ����			:	WriteAddr:��ʼ��ַ(�˵�ַ����Ϊ4�ı���!!);pBuffer:����ָ��;NumToWrite:��(32λ)��(����Ҫд���32λ���ݵĸ���.)
* ����			:	0:���;1:æ;2:����;3:д����
* ����			:	�ײ�
* ����			:	cp1300@139.com
* ʱ��			:	2013-10-20
* ����޸�ʱ�� 	: 	2016-01-07
* ˵��			: 	���棺����STM32F4������̫���޷�ʹ���ڴ���л��棬���Ҫд���λ���з�FF����ֱ�ӽ������������в���������ʧ����������
*************************************************************************************************************************/ 
STM32FLASH_STATUS STM32FLASH_Write(u32 WriteAddr,u32 *pBuffer,u16 NumToWrite)	
{
	STM32FLASH_STATUS status;
	u32 addrx=0;
	u32 endaddr=0;	
  	
	if(WriteAddr<STM32_FLASH_BASE||WriteAddr%4)return STM32FLASH_PGPERR;	//�Ƿ���ַ
	
 	FLASH->ACR&=~(1<<10);								//FLASH�����ڼ�,�����ֹ���ݻ���!!!�����������ϲŷ����������!
	addrx=WriteAddr;									//д�����ʼ��ַ
	endaddr=WriteAddr+NumToWrite*4;						//д��Ľ�����ַ
	
	if(addrx<0X1FFF0000)								//ֻ�����洢��,����Ҫִ�в�������!!
	{
		while(addrx<endaddr)							//ɨ��һ���ϰ�.(�Է�FFFFFFFF�ĵط�,�Ȳ���)
		{
			if(STM32FLASH_ReadWord(addrx)!=0XFFFFFFFF)	//�з�0XFFFFFFFF�ĵط�,Ҫ�����������
			{   
				status=STM32FLASH_EraseSector(STM32FLASH_GetFlashSector(addrx));
				if(status != STM32FLASH_OK)break;		//����������
			}else addrx+=4;
		} 
	}
	if(status==STM32FLASH_OK)
	{
		STM32FLASH_Unlock();							//���� 
		while(WriteAddr<endaddr)						//д����
		{
			status = STM32FLASH_WriteWord(WriteAddr,*pBuffer);
			if(status != STM32FLASH_OK) break;			//д�����ֱ�ӷ���
			WriteAddr+=4;
			pBuffer++;
		} 
		STM32FLASH_Lock();								//����
	}
	FLASH->ACR|=1<<10;									//FLASH��������,��������fetch
	
	
	return status;
}
 
 
 
 
 
/*************************************************************************************************************************
* ����			:	u16 STM32FLASH_Read(u32 ReadAddr,u32 *pBuffer,u16 NumToRead) 
* ����			:	��ָ����ַ��ʼ����ָ�����ȵ�����
* ����			:	ReadAddr:��ʼ��ַ;pBuffer:����ָ��;NumToWrite:��(32λ)��
* ����			:	��ȡ�����ݳ��ȣ���Ϊ��λ��
* ����			:	�ײ�
* ����			:	cp1300@139.com
* ʱ��			:	2013-10-20
* ����޸�ʱ�� 	: 	2016-01-07
* ˵��			: 	��ַ����Ϊ4����
*************************************************************************************************************************/
u16 STM32FLASH_Read(u32 ReadAddr,u32 *pBuffer,u16 NumToRead)   	
{
	u16 i;
	u16 cnt = 0;
	
	if(ReadAddr<STM32_FLASH_BASE||ReadAddr%4)return 0;	//�Ƿ���ַ
	
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=STM32FLASH_ReadWord(ReadAddr);		//��ȡ4���ֽ�.
		cnt++;
		ReadAddr+=4;									//ƫ��4���ֽ�.	
	}
	return cnt;
}

