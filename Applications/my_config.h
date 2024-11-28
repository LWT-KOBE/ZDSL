/*************************************************************************************************************
 * �ļ���:			Device_Config.h
 * ����:			�豸�������
 * ����:			cp1300@139.com
 * ����ʱ��:		2018-01-13
 * ����޸�ʱ��:	2018-01-13
 * ��ϸ:			�豸�������
*************************************************************************************************************/
#ifndef _MY_CONFIG_H_
#define _MY_CONFIG_H_

#include "board.h"
#define TRUE 1
#define FALSE 0
 
//�������,�����ڲ�flash
//���뱣֤Ϊ4�ֽڶ��䣬���Ҵ�С������4��������
#define DEVICE_BOARD_CONFIG_ID		0xA5A87A6E 			//���ID
typedef struct
{
	/****************************************ͷ����ע�������Ϣ������ʲô�豸�����ÿ�ͷ���������¸�ʽ************************************************************************************/
	u32 			ID;							//ID,����Ƿ���й�����
	char 			SN[16];						//Ψһ���к�-���ܽ��������޸�
	/*********************************************************************************************************************************************************************/
	u32				HorizAngle;					//�豸��װ��ˮƽ�нǣ�����3λС��
	u32				ThresholdEnergy;			//��ֵ������������ֵ�Ľ�����й���
	u32 			WaterLevelHeight;			//ˮλ�ư�װ�߶�
	u32 			WaterCorrParame;			//ˮλ����������ֵ��4λС����
	s32				WaterCorrectionLe;			//ˮλ����ֵ,�з���,��λmm
	u32 			Reserved32[4];				//Ԥ��
	u16  			WarmTime;					//Ԥ��ʱ�䣬��λS��0-999
	u16 			FlowSpeedCorrParame;		//������������ֵ��4λС����
	s16				FlowSpeedCorrectionLe;		//��������ֵ,�з���,��λmm/s
	u16				AcqCycle;					//�ɼ����ڣ���λ��
	u16				Filter[16][2];				//�˲�����Ԥ��16���˲��������Խ��и���Ƶ�ʹ���
	u16 			Reserved16[4];				//Ԥ��
	u8				SlaveAddr;					//�豸ͨ�ŵ�ַ
	u8				WaterLevelAddr;				//ˮλ��ͨ�ŵ�ַ
	u8				WaterLevelSelectIndex;		//ˮλ��ѡ��
	u8				FreqFilter;					//Ƶ�ʲ�ֵ�˲���IQͨ����Ƶ�ʲ�ֵ������Χ������˲�
	u8				FlowSpeedFilterCnt;			//�����˲�����
	u8				FlowRateFilterCnt;			//�����˲�����
	u8				FlowDireFilterCnt;			//���������˲�����
	s8				ProtectTempH;				//�豸�����¶ȸ��¶�ֵ
	s8				ProtectTempL;				//�豸�����¶ȵ��¶�ֵ
	u8				Reserve8[2];				//����
	
	u8 bbb;
}CONFIG_TYPE;


typedef struct
{
//	/****************************************ͷ����ע�������Ϣ������ʲô�豸�����ÿ�ͷ���������¸�ʽ************************************************************************************/
//	u32 			ID;							//ID,����Ƿ���й�����
//	char 			SN[16];						//Ψһ���к�-���ܽ��������޸�
//	/*********************************************************************************************************************************************************************/
	float ahk;
	
	float cks;
	float ops;
}CONFIG_TYPE_FLOAT;


extern CONFIG_TYPE 	g_SYS_Config;							//ϵͳ����
 
bool CONFIG_WriteSN(char pSN[16]);							//�޸�SN;
void CONFIG_Default(CONFIG_TYPE *pConfig, bool isAdmin);	//�ָ�����Ϊ����ģʽ
bool CONFIG_Check(CONFIG_TYPE *pConfig,bool isCheckID);		//������ò����Ƿ�Ϸ�
void CONFIG_Init(void);										//��ʼ������
bool CONFIG_WriteConfig(CONFIG_TYPE *pConfig);				//д�����ã�����Զ�̻���λ����������
bool CONFIG_CheckSN(char pSN[16]);							//���SN����Ч��
 
 
 
#endif /*_DEVICE_CONFIG_H_*/