/*************************************************************************************************************
 * 文件名:			Device_Config.h
 * 功能:			设备配置相关
 * 作者:			cp1300@139.com
 * 创建时间:		2018-01-13
 * 最后修改时间:	2018-01-13
 * 详细:			设备配置相关
*************************************************************************************************************/
#ifndef _MY_CONFIG_H_
#define _MY_CONFIG_H_

#include "board.h"
#define TRUE 1
#define FALSE 0
 
//配置相关,用于内部flash
//必须保证为4字节对其，并且大小必须是4的整数倍
#define DEVICE_BOARD_CONFIG_ID		0xA5A87A6E 			//标记ID
typedef struct
{
	/****************************************头部的注册相关信息，不管什么设备的配置开头必须是以下格式************************************************************************************/
	u32 			ID;							//ID,标记是否进行过配置
	char 			SN[16];						//唯一序列号-不能进行随意修改
	/*********************************************************************************************************************************************************************/
	u32				HorizAngle;					//设备安装的水平夹角，保留3位小数
	u32				ThresholdEnergy;			//阈值能量，低于阈值的将会进行过滤
	u32 			WaterLevelHeight;			//水位计安装高度
	u32 			WaterCorrParame;			//水位计线性修正值，4位小数点
	s32				WaterCorrectionLe;			//水位修正值,有符号,单位mm
	u32 			Reserved32[4];				//预留
	u16  			WarmTime;					//预热时间，单位S，0-999
	u16 			FlowSpeedCorrParame;		//流速线性修正值，4位小数点
	s16				FlowSpeedCorrectionLe;		//流速修正值,有符号,单位mm/s
	u16				AcqCycle;					//采集周期，单位秒
	u16				Filter[16][2];				//滤波器，预留16个滤波器，可以进行干扰频率过滤
	u16 			Reserved16[4];				//预留
	u8				SlaveAddr;					//设备通信地址
	u8				WaterLevelAddr;				//水位计通信地址
	u8				WaterLevelSelectIndex;		//水位计选择
	u8				FreqFilter;					//频率差值滤波，IQ通道的频率差值超出范围后进行滤波
	u8				FlowSpeedFilterCnt;			//流速滤波次数
	u8				FlowRateFilterCnt;			//流量滤波次数
	u8				FlowDireFilterCnt;			//流量方向滤波次数
	s8				ProtectTempH;				//设备保护温度高温度值
	s8				ProtectTempL;				//设备保护温度低温度值
	u8				Reserve8[2];				//保留
	
	u8 bbb;
}CONFIG_TYPE;


typedef struct
{
//	/****************************************头部的注册相关信息，不管什么设备的配置开头必须是以下格式************************************************************************************/
//	u32 			ID;							//ID,标记是否进行过配置
//	char 			SN[16];						//唯一序列号-不能进行随意修改
//	/*********************************************************************************************************************************************************************/
	float ahk;
	
	float cks;
	float ops;
}CONFIG_TYPE_FLOAT;


extern CONFIG_TYPE 	g_SYS_Config;							//系统配置
 
bool CONFIG_WriteSN(char pSN[16]);							//修改SN;
void CONFIG_Default(CONFIG_TYPE *pConfig, bool isAdmin);	//恢复配置为出厂模式
bool CONFIG_Check(CONFIG_TYPE *pConfig,bool isCheckID);		//检查配置参数是否合法
void CONFIG_Init(void);										//初始化配置
bool CONFIG_WriteConfig(CONFIG_TYPE *pConfig);				//写入配置，用于远程或上位机更新配置
bool CONFIG_CheckSN(char pSN[16]);							//检查SN的有效性
 
 
 
#endif /*_DEVICE_CONFIG_H_*/