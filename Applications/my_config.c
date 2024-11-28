/*************************************************************************************************************
 * �ļ���:			Device_Config.c
 * ����:			�豸�������
 * ����:			cp1300@139.com
 * ����ʱ��:		2018-01-13
 * ����޸�ʱ��:	2018-01-13
 * ��ϸ:			�豸�������-����ʹ���˴��ڴ�ӡ��һ��Ҫ�ڴ��ڳ�ʼ�����ٵ���
					����˼·��	ʹ��64KB��������ÿ������ռ��1KB��ÿ�γ�ʼ����ʱ��Ӻ���ǰ�ж������Ƿ���Ч��������һ��Ϊ��Ч�����´�д��
								�������������������д�뵽��0��1KB���´�д�뵽��1��1KB���������ƣ��������Ա�֤д��64�����òŲ���һ������
								1����֤�ȶ��ԣ���������ÿ�β���д���ʱ�䡣
								
*************************************************************************************************************/

#include "usartx.h"
#include "main.h"
#include "board.h"
#include "my_config.h"
#include "my_flash.h"
#include <stdlib.h>
 
 
//���Կ���
#define CONFIG_DBUG	0
#if CONFIG_DBUG
	#include "system.h"
	#define config_debug(format,...)	uart_printf(format,##__VA_ARGS__)
#else
	#define config_debug(format,...)	/\
/
#endif	//CONFIG_DBUG
 
 
CONFIG_TYPE 	g_SYS_Config;					//ȫ��ϵͳ����
static u16 gs_ConfigSaveIndex;					//ϵͳ���ô洢���������ڼ�¼��ǰ�洢����64KB�е�λ�ã�ÿ�洢һ�ξͻ��������һ�Σ�������FF��ʱ��Ž��в�����ѭ���洢�����豣��֮ǰ�����ݣ�
 
#define STM32_CONFIG_PAGE_INDEX		4			//������������������4,64KB
#define STM32_CONFIG_PAGE_ADDR		0x08010000	//�����ļ�ҳ��ַ���ֽڵ�ַ����0x0801 0000 - 0x0801 FFFF
#define STM32_CONFIG_PAGE_SIZE		(64*1024)	//�����ļ�ҳ��С
#define CONFIG_MAX_SIZE				256			//�����ļ�����С
 
 
static bool CONFIG_SaveConfig(CONFIG_TYPE *pConfig);				//�洢�������ݵ�flash
 
 
/*************************************************************************************************************************
* ����				:	void CONFIG_Init(void)
* ����				:	�ϵ��ʼ������
* ����				:	��
* ����				:	��
* ����				:	flash��������
* ����				:	cp1300@139.com
* ʱ��				:	2014-10-18
* ����޸�ʱ�� 		: 	2017-03-22
* ˵��				: 	������ҳ���м��������ļ�����ƫ�ƣ��Ӻ���ǰ����,��������õ�ȫ�֣�g_SYS_Config��
*************************************************************************************************************************/
void CONFIG_Init(void)
{
	int index;
	u32 addr;
	u32 ConfigId;
	u16 count;
	STM32FLASH_STATUS status;
	
	if(sizeof(CONFIG_TYPE)>CONFIG_MAX_SIZE)
	{
		//DEBUG("���������ļ���С��������\r\n");
		while(1);
	}
 
	count = STM32_CONFIG_PAGE_SIZE/CONFIG_MAX_SIZE;														//���������������Դ���������ݵ�����
	for(index = (count-1);index >= 0;index --)															//�����Ӻ���ǰѰ�ң���������һ��Ϊ��������
	{
		addr = STM32_CONFIG_PAGE_ADDR+index*CONFIG_MAX_SIZE;											//������Ҫ��ȡ�ĵ�ַ
		STM32FLASH_Read(addr, &ConfigId, 4/sizeof(u32));												//��ָ����ַ��ʼ����ָ�����ȵ�����-��ȡ����������ǰ���4B��Ҳ��������id
		if(ConfigId == DEVICE_BOARD_CONFIG_ID)															//��Ч��ID����ζ���ҵ������û���������λ��
		{
			gs_ConfigSaveIndex = index;
			//uart_printf("�ҵ�������������%d\r\n",index);
			break;
		}
	}
	if(index < 0)	//û���ҵ�����ô����Ĭ�ϵ����ã���ʽ��������������д�뵽��һ��������
	{
		//�Ȳ���
		status = STM32FLASH_EraseSector(STM32_CONFIG_PAGE_INDEX);			  							//��������
		if(status == STM32FLASH_OK)
		{
			//uart_printf("������������%d�ɹ�\r\n",STM32_CONFIG_PAGE_INDEX);
		}
		else
		{
			//uart_printf("������������%dʧ��\r\n",STM32_CONFIG_PAGE_INDEX);
		}
		gs_ConfigSaveIndex = 0;																			//Ĭ�ϴ�����0��ʼ�洢
		//uart_printf("ϵͳδ���ù�������Ĭ�����ã�\r\n");
		CONFIG_Default(&g_SYS_Config, TRUE);															//���е����ûָ���������
		addr = STM32_CONFIG_PAGE_ADDR+gs_ConfigSaveIndex*CONFIG_MAX_SIZE;								//������Ҫд��ĵ�ַ
		status = STM32FLASH_Write_NoCheck(addr,(u32 *)&g_SYS_Config,sizeof(CONFIG_TYPE)/sizeof(u32)) ; 	//������д��-д�����õ�flash
		if(status != STM32FLASH_OK)
		{
			//DEBUG("д����������%dʧ��\r\n",STM32_CONFIG_PAGE_INDEX);
		}
	}
	else //�ҵ������ˣ���������
	{
		addr = STM32_CONFIG_PAGE_ADDR+gs_ConfigSaveIndex*CONFIG_MAX_SIZE;								//������Ҫд��ĵ�ַ
		STM32FLASH_Read(addr, (u32 *)&g_SYS_Config,sizeof(CONFIG_TYPE)/sizeof(u32));					//��������
		if(CONFIG_Check(&g_SYS_Config, TRUE) == FALSE)
		{
			//uart_printf("���������ļ�ʧ�ܣ�������Ч�����¼���Ĭ������\r\n");
			CONFIG_Default(&g_SYS_Config, TRUE);														//���е����ûָ���������
		}
		else
		{
			//uart_printf("���������ļ��ɹ������ô�С��%dB\r\n",sizeof(CONFIG_TYPE));
		}
	}
	CONFIG_Check(&g_SYS_Config,FALSE);																	//�������
	if(CONFIG_CheckSN(g_SYS_Config.SN) == FALSE)
	{
		g_SYS_Config.SN[0] = 0;
		//DEBUG("���棬û������SN\r\n");
	}
}
 
 
 
/*************************************************************************************************************************
* ����				:	bool CONFIG_Check(CONFIG_TYPE *pConfig,bool isCheckID)
* ����				:	������ò����Ƿ�Ϸ�
* ����				:	isCheckID:�Ƿ�������ID,����������ID����ȷֱ�ӷ��ش���,���򽫻�Դ������ý�������,�������ɻ᷵��TRUE;pConfig:��������ָ��
* ����				:	��
* ����				:	flash��������
* ����				:	cp1300@139.com
* ʱ��				:	2014-10-18
* ����޸�ʱ�� 		: 	2017-03-22
* ˵��				: 	���ڼ������
*************************************************************************************************************************/
bool CONFIG_Check(CONFIG_TYPE *pConfig,bool isCheckID)
{
	if(isCheckID)	//��Ҫ�������ID
	{
		if(pConfig->ID != DEVICE_BOARD_CONFIG_ID) return FALSE;	//����ID��Ч
	}
	//Ϊÿһ�����ý��м��
	pConfig->SN[15]  = 0;						//Ψһ���к�-���ܽ��������޸�
	/*********************************************************************************************************************************************************************/
	if(pConfig->HorizAngle > (90*1000)) pConfig->HorizAngle = 90*1000;					//�豸��װ��ˮƽ�нǣ�����3λС��
	if(pConfig->ThresholdEnergy > 100000) pConfig->ThresholdEnergy = 100000;			//��ֵ������������ֵ�Ľ�����й���
	if(pConfig->WaterLevelHeight > 9999999) pConfig->WaterLevelHeight= 999999;			//ˮλ�ư�װ�߶�
	if(pConfig->WaterCorrParame > 999999) pConfig->WaterCorrParame = 999999;			//ˮλ����������ֵ��4λС����
	if(pConfig->WaterCorrectionLe > 9999999) pConfig->WaterCorrectionLe = 9999999;		//ˮλ����ֵ,�з���,��λmm
	if(pConfig->WaterCorrectionLe < -9999999) pConfig->WaterCorrectionLe = -9999999;	//ˮλ����ֵ,�з���,��λmm
	if(pConfig->WarmTime > 999) pConfig->WarmTime = 999;								//Ԥ��ʱ�䣬��λS��0-999
	if(pConfig->FlowSpeedCorrParame < 1) pConfig->FlowSpeedCorrParame = 1;				//������������ֵ��4λС����
	if(pConfig->FlowSpeedCorrectionLe > 29999) pConfig->FlowSpeedCorrectionLe = 29999;	//��������ֵ,�з���,��λmm/s
	if(pConfig->FlowSpeedCorrectionLe < -29999) pConfig->FlowSpeedCorrectionLe = -29999;//��������ֵ,�з���,��λmm/s
	//u16				AcqCycle;					//�ɼ����ڣ���λ��
	//u16				Filter[16][2];				//�˲�����Ԥ��16���˲��������Խ��и���Ƶ�ʹ���
	//u8				SlaveAddr;					//�豸ͨ�ŵ�ַ
	//u8				WaterLevelAddr;				//ˮλ��ͨ�ŵ�ַ
	//u8				WaterLevelSelectIndex;		//ˮλ��ѡ��
	
	if(pConfig->FreqFilter < 20) pConfig->FreqFilter = 20;								//Ƶ�ʲ�ֵ�˲���IQͨ����Ƶ�ʲ�ֵ������Χ������˲�
	if(pConfig->FlowSpeedFilterCnt > 100) pConfig->FlowSpeedFilterCnt = 100;			//�����˲�����
	if(pConfig->FlowRateFilterCnt > 100) pConfig->FlowRateFilterCnt = 100;				//�����˲�����
	if(pConfig->FlowDireFilterCnt > 100) pConfig->FlowDireFilterCnt = 100;				//���������˲�����
	if(pConfig->ProtectTempL < -99) pConfig->ProtectTempL = 99;							//�豸�����¶ȵ��¶�ֵ
	if(pConfig->ProtectTempL > 0) pConfig->ProtectTempL = 0;							//�豸�����¶ȵ��¶�ֵ
	if(pConfig->ProtectTempH < 20) pConfig->ProtectTempH = 20;							//�豸�����¶ȸ��¶�ֵ
	if(pConfig->ProtectTempH > 99) pConfig->ProtectTempH = 99;							//�豸�����¶ȸ��¶�ֵ
	
	return TRUE;
}
	
	
/*************************************************************************************************************************
* ����				:	void CONFIG_Default(CONFIG_TYPE *pConfig, bool isAdmin)
* ����				:	�ָ�����Ϊ����ģʽ
* ����				:	isAdmin���Ƿ�Ϊ����Աģʽ
* ����				:	��
* ����				:	flash��������
* ����				:	cp1300@139.com
* ʱ��				:	2014-10-18
* ����޸�ʱ�� 		: 	2017-03-21
* ˵��				: 	������д洢
*************************************************************************************************************************/
void CONFIG_Default(CONFIG_TYPE *pConfig, bool isAdmin)
{
	int i;
		
	pConfig->ID = DEVICE_BOARD_CONFIG_ID;		//������������ID
	if(isAdmin == TRUE) 
	{
		pConfig->SN[0] = 0;						//���SN
	}
	else
	{
		pConfig->SN[15] = 0;					//Ψһ���к�-���ܽ��������޸�
	}
	
	/*********************************************************************************************************************************************************************/
	pConfig->HorizAngle = 0;					//�豸��װ��ˮƽ�нǣ�����3λС��-0Ϊ�Զ�
	pConfig->ThresholdEnergy = 12000;			//��ֵ������������ֵ�Ľ�����й���
	pConfig->WaterLevelHeight = 0;				//ˮλ�ư�װ�߶�
	pConfig->WaterCorrParame = 1.0*10000;		//ˮλ����������ֵ��4λС����
	pConfig->WaterCorrectionLe = 0;				//ˮλ����ֵ,�з���,��λmm
	
	pConfig->WarmTime = 5;						//Ԥ��ʱ�䣬��λS��0-999
	pConfig->FlowSpeedCorrParame = 1.0*10000;	//������������ֵ��4λС����
	pConfig->FlowSpeedCorrectionLe = 0;			//��������ֵ,�з���,��λmm/s
	pConfig->AcqCycle = 0;						//�ɼ����ڣ���λ��0�������ɼ�
	//�˲�����Ԥ��16���˲��������Խ��и���Ƶ�ʹ���
	for(i = 0;i < 16;i ++)
	{
		pConfig->Filter[i][0] = 0;
		pConfig->Filter[i][1] = 0;
	}
	pConfig->SlaveAddr = 1;						//�豸Ĭ��ͨ�ŵ�ַ
	pConfig->WaterLevelAddr = 1;				//ˮλ��ͨ�ŵ�ַ
	pConfig->WaterLevelSelectIndex = 0;			//ˮλ��ѡ��
	pConfig->FreqFilter = 60;					//Ƶ�ʲ�ֵ�˲���IQͨ����Ƶ�ʲ�ֵ������Χ������˲�
	pConfig->FlowSpeedFilterCnt = 20;			//�����˲�����
	pConfig->FlowRateFilterCnt = 20;			//�����˲�����
	pConfig->FlowDireFilterCnt = 10;			//���������˲�����
	pConfig->ProtectTempH = 90;					//�豸�����¶ȸ��¶�ֵ
	pConfig->ProtectTempL = -30;				//�豸�����¶ȵ��¶�ֵ
}
 
 
 
/*************************************************************************************************************************
* ����				:	bool CONFIG_SaveConfig(CONFIG_TYPE *pConfig)
* ����				:	�洢�������ݵ�flash
* ����				:	pConfig:���ô洢������
* ����				:	FALSE:ʧ��;TRUE:�ɹ�
* ����				:	flash��������
* ����				:	cp1300@139.com
* ʱ��				:	2013-10-22
* ����޸�ʱ�� 		: 	2018-01-17
* ˵��				: 	ֻ��һ�δ洢���е���������
						��������Ӧ��С��1��ҳ��С
						STM32F407����д�����ԭ����һ��64KB��������Ϊ���д�����򣬱���64��������Ϊ0-63�������ǰ�洢������Ϊ63����ֱ�Ӳ���������������Ϊ0
						�´�д��ʱ������+1��ͬʱ����д�������Ƿ�ȫ��ΪFF������ִ�в���������������Ϊ0���ظ���ѭ���������Ͽ���д64�����òŲ���һ�Σ�������Ч���������������
						�Լ�����flash�������������flash������
*************************************************************************************************************************/
static bool CONFIG_SaveConfig(CONFIG_TYPE *pConfig)
{
	u32 addr;
	u16 count;
	STM32FLASH_STATUS status;
	u32 temp;
	bool isFistIndex = FALSE;																			//�Ƿ���Ҫ������0��ʼ����������ǰ����
	int i;
	
	count = STM32_CONFIG_PAGE_SIZE/CONFIG_MAX_SIZE;														//���������������Դ���������ݵ�����
	if(gs_ConfigSaveIndex == (count-1))																	//��ǰ�洢Ϊ���һ�������ˣ���0��ʼ�洢
	{
		isFistIndex = TRUE;																				//��Ҫд�뵽�׸�����
	}
	else //��Ҫ�ж��Ƿ�ΪFF
	{
		gs_ConfigSaveIndex ++;																			//�洢����λ������
		for(i = 0;i < (sizeof(CONFIG_TYPE)/sizeof(u32));i ++)
		{
			addr = STM32_CONFIG_PAGE_ADDR+gs_ConfigSaveIndex*CONFIG_MAX_SIZE+i*4;						//������Ҫ��ȡ�ĵ�ַ
			STM32FLASH_Read(addr, &temp, 4/sizeof(u32));
			if(temp != 0xFFFFFFFF)
			{
				//uart_printf("�洢�����ַ�0���֣���Ҫ��������ǰ������%d\r\n",gs_ConfigSaveIndex);
				gs_ConfigSaveIndex = 0;																	//������Ϊ0
				break;
			}
		}
		if(gs_ConfigSaveIndex == 0)																		//��Ҫ����
		{
			isFistIndex = TRUE;																			//��Ҫд�뵽�׸�����
		}
	}
	
	if(isFistIndex== TRUE)	//��Ҫд�뵽�׸�����
	{
        gs_ConfigSaveIndex = 0;		    //2019-11-21 �޸����Ƚ�������������
		//�Ȳ���
		status = STM32FLASH_EraseSector(STM32_CONFIG_PAGE_INDEX);			  							//��������
		if(status == STM32FLASH_OK)
		{
			//uart_printf("������������%d�ɹ�\r\n",STM32_CONFIG_PAGE_INDEX);
		}
		else
		{
			//uart_printf("������������%dʧ��\r\n",STM32_CONFIG_PAGE_INDEX);
		}
		//д������
		addr = STM32_CONFIG_PAGE_ADDR+gs_ConfigSaveIndex*CONFIG_MAX_SIZE;								//������Ҫд��ĵ�ַ
		status = STM32FLASH_Write_NoCheck(addr,(u32 *)&g_SYS_Config,sizeof(CONFIG_TYPE)/sizeof(u32)) ; 	//������д��-д�����õ�flash
																			//������Ϊ0
	}
	else //���׸�����д��-�������������ǰ���Ѿ������˼�飬ֱ��д�뼴��
	{
		addr = STM32_CONFIG_PAGE_ADDR+gs_ConfigSaveIndex*CONFIG_MAX_SIZE;								//������Ҫд��ĵ�ַ
		status = STM32FLASH_Write_NoCheck(addr,(u32 *)&g_SYS_Config,sizeof(CONFIG_TYPE)/sizeof(u32)) ; 	//������д��-д�����õ�flash
	}
	//�ж��Ƿ�д��ɹ�
	if(status == STM32FLASH_OK)
	{
		//uart_printf("�洢���óɹ���������%d\r\n",gs_ConfigSaveIndex);
		return TRUE;
	}
	else
	{
		//uart_printf("�洢����ʧ�ܣ�������%d(����%d)\r\n",gs_ConfigSaveIndex, status);
		return FALSE;
	}
}
 
 
 
 
 
/*************************************************************************************************************************
* ����				:	bool CONFIG_WriteConfig(CONFIG_TYPE *pConfig)
* ����				:	����д������(�����޸�SN)
* ����				:	pConfig:���ô洢������
* ����				:	FALSE:ʧ��;TRUE:�ɹ�
* ����				:	flash��������
* ����				:	cp1300@139.com
* ʱ��				:	2017-04-05
* ����޸�ʱ�� 		: 	2017-04-05
* ˵��				: 	���ڸ���ϵͳ���ã�����д�����õ�flash,��������ã���Ҫ��ǰ������֤����
*************************************************************************************************************************/
bool CONFIG_WriteConfig(CONFIG_TYPE *pConfig)
{
	int i;
	
	/****************************************ͷ����ע�������Ϣ������ʲô�豸�����ÿ�ͷ���������¸�ʽ************************************************************************************/
	g_SYS_Config.ID = DEVICE_BOARD_CONFIG_ID;							//ID,����Ƿ���й�����
	g_SYS_Config.SN[15] = 0;											//Ψһ���к�-���ܽ��������޸�							
	/*********************************************************************************************************************************************************************/
	g_SYS_Config.HorizAngle 						= pConfig->HorizAngle ;					//�豸��װ��ˮƽ�нǣ�����3λС��
	g_SYS_Config.ThresholdEnergy 					= pConfig->ThresholdEnergy;				//��ֵ������������ֵ�Ľ�����й���
	g_SYS_Config.WaterLevelHeight 					= pConfig->WaterLevelHeight;			//ˮλ�ư�װ�߶�
	g_SYS_Config.WaterCorrParame 					= pConfig->WaterCorrParame;				//ˮλ����������ֵ��4λС����
	g_SYS_Config.WaterCorrectionLe 					= pConfig->WaterCorrectionLe;			//ˮλ����ֵ,�з���,��λmm
	
	g_SYS_Config.WarmTime 							= pConfig->WarmTime;					//Ԥ��ʱ�䣬��λS��0-999
	g_SYS_Config.FlowSpeedCorrParame 				= pConfig->FlowSpeedCorrParame;			//������������ֵ��4λС����
	g_SYS_Config.FlowSpeedCorrectionLe 				= pConfig->FlowSpeedCorrectionLe;		//��������ֵ,�з���,��λmm
	g_SYS_Config.AcqCycle 							= pConfig->AcqCycle;					//�ɼ����ڣ���λ��
	for(i = 0;i < 16;i ++)
	{
		g_SYS_Config.Filter[i][0] 					= pConfig->Filter[i][0];				//�˲�����Ԥ��16���˲��������Խ��и���Ƶ�ʹ���
		g_SYS_Config.Filter[i][1] 					= pConfig->Filter[i][1];				//�˲�����Ԥ��16���˲��������Խ��и���Ƶ�ʹ���
	}
	
	
	g_SYS_Config.SlaveAddr 							= pConfig->SlaveAddr;					//�豸ͨ�ŵ�ַ
	g_SYS_Config.WaterLevelAddr 					= pConfig->WaterLevelAddr;				//ˮλ��ͨ�ŵ�ַ
	g_SYS_Config.WaterLevelSelectIndex 				= pConfig->WaterLevelSelectIndex;		//ˮλ��ѡ��
	g_SYS_Config.FreqFilter 						= pConfig->FreqFilter;					//Ƶ�ʲ�ֵ�˲���IQͨ����Ƶ�ʲ�ֵ������Χ������˲�
	g_SYS_Config.FlowSpeedFilterCnt 				= pConfig->FlowSpeedFilterCnt;			//�����˲�����
	g_SYS_Config.FlowRateFilterCnt 					= pConfig->FlowRateFilterCnt;			//�����˲�����
	g_SYS_Config.FlowDireFilterCnt 					= pConfig->FlowDireFilterCnt;			//���������˲�����
	g_SYS_Config.ProtectTempH 						= pConfig->ProtectTempH;				//�豸�����¶ȸ��¶�ֵ
	g_SYS_Config.ProtectTempL 						= pConfig->ProtectTempL;				//�豸�����¶ȵ��¶�ֵ
	
 
	return CONFIG_SaveConfig(&g_SYS_Config);
}
 
 
 
/*************************************************************************************************************************
* ����				:	bool CONFIG_WriteSN(char pSN[16])
* ����				:	�޸�SN
* ����				:	pSN��SN
* ����				:	FALSE:ʧ��;TRUE:�ɹ�
* ����				:	flash��������
* ����				:	cp1300@139.com
* ʱ��				:	2017-04-05
* ����޸�ʱ�� 		: 	2018-01-31
* ˵��				: 	������SN�Ϸ���
*************************************************************************************************************************/
bool CONFIG_WriteSN(char pSN[16])
{
	g_SYS_Config.ID = DEVICE_BOARD_CONFIG_ID;							//ID,����Ƿ���й�����
	pSN[15] = 0;						
	strcpy(g_SYS_Config.SN, pSN);										//Ψһ���к�-���ܽ��������޸�
	
	return CONFIG_SaveConfig(&g_SYS_Config);
}
 
 
 
 
/*************************************************************************************************************************
* ����				:	bool CONFIG_CheckSN(char pSN[16])
* ����				:	���SN����Ч��
* ����				:	pSN:���кŴ洢������
* ����				:	TRUE:��Ч��FALSE:��Ч��û������SN
* ����				:	flash��������
* ����				:	cp1300@139.com
* ʱ��				:	2014-10-18
* ����޸�ʱ�� 		: 	2018-01-17
* ˵��				: 	���SN����Ч��
*************************************************************************************************************************/
bool CONFIG_CheckSN(char pSN[16])
{
	u8 i;
	
	pSN[15] = '\0';
	if(strlen(pSN) != 15) return FALSE;
	for(i = 5;i < 15;i ++)
	{
		if((pSN[i] < '0') || (pSN[i] > '9'))
		{
			return FALSE;
		}
	}
	return TRUE;
}
 
 
 
 
 
 
 
 
 
 
 
 