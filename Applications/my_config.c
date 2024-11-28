/*************************************************************************************************************
 * 文件名:			Device_Config.c
 * 功能:			设备配置相关
 * 作者:			cp1300@139.com
 * 创建时间:		2018-01-13
 * 最后修改时间:	2018-01-13
 * 详细:			设备配置相关-由于使用了串口打印，一定要在串口初始化后再调用
					基本思路：	使用64KB的扇区，每次配置占用1KB，每次初始化的时候从后向前判断配置是否有效，如果最后一个为有效，则下次写入
								会擦除扇区，并将配置写入到第0个1KB，下次写入到第1个1KB，依次类推，这样可以保证写入64次配置才擦除一次扇区
								1来保证稳定性，二来降低每次擦除写入的时间。
								
*************************************************************************************************************/

#include "usartx.h"
#include "main.h"
#include "board.h"
#include "my_config.h"
#include "my_flash.h"
#include <stdlib.h>
 
 
//调试开关
#define CONFIG_DBUG	0
#if CONFIG_DBUG
	#include "system.h"
	#define config_debug(format,...)	uart_printf(format,##__VA_ARGS__)
#else
	#define config_debug(format,...)	/\
/
#endif	//CONFIG_DBUG
 
 
CONFIG_TYPE 	g_SYS_Config;					//全局系统配置
static u16 gs_ConfigSaveIndex;					//系统配置存储索引（用于记录当前存储在这64KB中的位置，每存储一次就会向后增长一次，碰到非FF的时候才进行擦除，循环存储，无需保存之前的数据）
 
#define STM32_CONFIG_PAGE_INDEX		4			//配置扇区索引：扇区4,64KB
#define STM32_CONFIG_PAGE_ADDR		0x08010000	//配置文件页地址（字节地址），0x0801 0000 - 0x0801 FFFF
#define STM32_CONFIG_PAGE_SIZE		(64*1024)	//配置文件页大小
#define CONFIG_MAX_SIZE				256			//配置文件最大大小
 
 
static bool CONFIG_SaveConfig(CONFIG_TYPE *pConfig);				//存储配置数据到flash
 
 
/*************************************************************************************************************************
* 函数				:	void CONFIG_Init(void)
* 功能				:	上电初始化配置
* 参数				:	无
* 返回				:	无
* 依赖				:	flash操作函数
* 作者				:	cp1300@139.com
* 时间				:	2014-10-18
* 最后修改时间 		: 	2017-03-22
* 说明				: 	从配置页面中加载配置文件所在偏移，从后向前查找,会加载配置到全局：g_SYS_Config中
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
		//DEBUG("错误，配置文件大小超出限制\r\n");
		while(1);
	}
 
	count = STM32_CONFIG_PAGE_SIZE/CONFIG_MAX_SIZE;														//计算配置扇区可以存放配置数据的条数
	for(index = (count-1);index >= 0;index --)															//索引从后向前寻找，索引最大的一次为最后的配置
	{
		addr = STM32_CONFIG_PAGE_ADDR+index*CONFIG_MAX_SIZE;											//计算需要读取的地址
		STM32FLASH_Read(addr, &ConfigId, 4/sizeof(u32));												//从指定地址开始读出指定长度的数据-读取配置区域最前面的4B，也就是配置id
		if(ConfigId == DEVICE_BOARD_CONFIG_ID)															//有效的ID，意味着找到了配置缓冲区索引位置
		{
			gs_ConfigSaveIndex = index;
			//uart_printf("找到了配置索引：%d\r\n",index);
			break;
		}
	}
	if(index < 0)	//没有找到，那么加载默认的配置，格式化并将配置数据写入到第一个扇区中
	{
		//先擦除
		status = STM32FLASH_EraseSector(STM32_CONFIG_PAGE_INDEX);			  							//擦除扇区
		if(status == STM32FLASH_OK)
		{
			//uart_printf("擦除配置扇区%d成功\r\n",STM32_CONFIG_PAGE_INDEX);
		}
		else
		{
			//uart_printf("擦除配置扇区%d失败\r\n",STM32_CONFIG_PAGE_INDEX);
		}
		gs_ConfigSaveIndex = 0;																			//默认从索引0开始存储
		//uart_printf("系统未配置过，加载默认配置！\r\n");
		CONFIG_Default(&g_SYS_Config, TRUE);															//所有的配置恢复出厂配置
		addr = STM32_CONFIG_PAGE_ADDR+gs_ConfigSaveIndex*CONFIG_MAX_SIZE;								//计算需要写入的地址
		status = STM32FLASH_Write_NoCheck(addr,(u32 *)&g_SYS_Config,sizeof(CONFIG_TYPE)/sizeof(u32)) ; 	//不检查的写入-写入配置到flash
		if(status != STM32FLASH_OK)
		{
			//DEBUG("写入配置扇区%d失败\r\n",STM32_CONFIG_PAGE_INDEX);
		}
	}
	else //找到配置了，加载配置
	{
		addr = STM32_CONFIG_PAGE_ADDR+gs_ConfigSaveIndex*CONFIG_MAX_SIZE;								//计算需要写入的地址
		STM32FLASH_Read(addr, (u32 *)&g_SYS_Config,sizeof(CONFIG_TYPE)/sizeof(u32));					//加载配置
		if(CONFIG_Check(&g_SYS_Config, TRUE) == FALSE)
		{
			//uart_printf("加载配置文件失败，配置无效，重新加载默认配置\r\n");
			CONFIG_Default(&g_SYS_Config, TRUE);														//所有的配置恢复出厂配置
		}
		else
		{
			//uart_printf("加载配置文件成功，配置大小：%dB\r\n",sizeof(CONFIG_TYPE));
		}
	}
	CONFIG_Check(&g_SYS_Config,FALSE);																	//检查配置
	if(CONFIG_CheckSN(g_SYS_Config.SN) == FALSE)
	{
		g_SYS_Config.SN[0] = 0;
		//DEBUG("警告，没有设置SN\r\n");
	}
}
 
 
 
/*************************************************************************************************************************
* 函数				:	bool CONFIG_Check(CONFIG_TYPE *pConfig,bool isCheckID)
* 功能				:	检查配置参数是否合法
* 参数				:	isCheckID:是否检查配置ID,如果检查配置ID不正确直接返回错误,否则将会对错误配置进行限制,最终依旧会返回TRUE;pConfig:配置数据指针
* 返回				:	无
* 依赖				:	flash操作函数
* 作者				:	cp1300@139.com
* 时间				:	2014-10-18
* 最后修改时间 		: 	2017-03-22
* 说明				: 	用于检查配置
*************************************************************************************************************************/
bool CONFIG_Check(CONFIG_TYPE *pConfig,bool isCheckID)
{
	if(isCheckID)	//需要检查配置ID
	{
		if(pConfig->ID != DEVICE_BOARD_CONFIG_ID) return FALSE;	//配置ID无效
	}
	//为每一个配置进行检查
	pConfig->SN[15]  = 0;						//唯一序列号-不能进行随意修改
	/*********************************************************************************************************************************************************************/
	if(pConfig->HorizAngle > (90*1000)) pConfig->HorizAngle = 90*1000;					//设备安装的水平夹角，保留3位小数
	if(pConfig->ThresholdEnergy > 100000) pConfig->ThresholdEnergy = 100000;			//阈值能量，低于阈值的将会进行过滤
	if(pConfig->WaterLevelHeight > 9999999) pConfig->WaterLevelHeight= 999999;			//水位计安装高度
	if(pConfig->WaterCorrParame > 999999) pConfig->WaterCorrParame = 999999;			//水位计线性修正值，4位小数点
	if(pConfig->WaterCorrectionLe > 9999999) pConfig->WaterCorrectionLe = 9999999;		//水位修正值,有符号,单位mm
	if(pConfig->WaterCorrectionLe < -9999999) pConfig->WaterCorrectionLe = -9999999;	//水位修正值,有符号,单位mm
	if(pConfig->WarmTime > 999) pConfig->WarmTime = 999;								//预热时间，单位S，0-999
	if(pConfig->FlowSpeedCorrParame < 1) pConfig->FlowSpeedCorrParame = 1;				//流速线性修正值，4位小数点
	if(pConfig->FlowSpeedCorrectionLe > 29999) pConfig->FlowSpeedCorrectionLe = 29999;	//流速修正值,有符号,单位mm/s
	if(pConfig->FlowSpeedCorrectionLe < -29999) pConfig->FlowSpeedCorrectionLe = -29999;//流速修正值,有符号,单位mm/s
	//u16				AcqCycle;					//采集周期，单位秒
	//u16				Filter[16][2];				//滤波器，预留16个滤波器，可以进行干扰频率过滤
	//u8				SlaveAddr;					//设备通信地址
	//u8				WaterLevelAddr;				//水位计通信地址
	//u8				WaterLevelSelectIndex;		//水位计选择
	
	if(pConfig->FreqFilter < 20) pConfig->FreqFilter = 20;								//频率差值滤波，IQ通道的频率差值超出范围后进行滤波
	if(pConfig->FlowSpeedFilterCnt > 100) pConfig->FlowSpeedFilterCnt = 100;			//流速滤波次数
	if(pConfig->FlowRateFilterCnt > 100) pConfig->FlowRateFilterCnt = 100;				//流量滤波次数
	if(pConfig->FlowDireFilterCnt > 100) pConfig->FlowDireFilterCnt = 100;				//流量方向滤波次数
	if(pConfig->ProtectTempL < -99) pConfig->ProtectTempL = 99;							//设备保护温度低温度值
	if(pConfig->ProtectTempL > 0) pConfig->ProtectTempL = 0;							//设备保护温度低温度值
	if(pConfig->ProtectTempH < 20) pConfig->ProtectTempH = 20;							//设备保护温度高温度值
	if(pConfig->ProtectTempH > 99) pConfig->ProtectTempH = 99;							//设备保护温度高温度值
	
	return TRUE;
}
	
	
/*************************************************************************************************************************
* 函数				:	void CONFIG_Default(CONFIG_TYPE *pConfig, bool isAdmin)
* 功能				:	恢复配置为出厂模式
* 参数				:	isAdmin：是否为管理员模式
* 返回				:	无
* 依赖				:	flash操作函数
* 作者				:	cp1300@139.com
* 时间				:	2014-10-18
* 最后修改时间 		: 	2017-03-21
* 说明				: 	不会进行存储
*************************************************************************************************************************/
void CONFIG_Default(CONFIG_TYPE *pConfig, bool isAdmin)
{
	int i;
		
	pConfig->ID = DEVICE_BOARD_CONFIG_ID;		//首先设置配置ID
	if(isAdmin == TRUE) 
	{
		pConfig->SN[0] = 0;						//清除SN
	}
	else
	{
		pConfig->SN[15] = 0;					//唯一序列号-不能进行随意修改
	}
	
	/*********************************************************************************************************************************************************************/
	pConfig->HorizAngle = 0;					//设备安装的水平夹角，保留3位小数-0为自动
	pConfig->ThresholdEnergy = 12000;			//阈值能量，低于阈值的将会进行过滤
	pConfig->WaterLevelHeight = 0;				//水位计安装高度
	pConfig->WaterCorrParame = 1.0*10000;		//水位计线性修正值，4位小数点
	pConfig->WaterCorrectionLe = 0;				//水位修正值,有符号,单位mm
	
	pConfig->WarmTime = 5;						//预热时间，单位S，0-999
	pConfig->FlowSpeedCorrParame = 1.0*10000;	//流速线性修正值，4位小数点
	pConfig->FlowSpeedCorrectionLe = 0;			//流速修正值,有符号,单位mm/s
	pConfig->AcqCycle = 0;						//采集周期，单位秒0：连续采集
	//滤波器，预留16个滤波器，可以进行干扰频率过滤
	for(i = 0;i < 16;i ++)
	{
		pConfig->Filter[i][0] = 0;
		pConfig->Filter[i][1] = 0;
	}
	pConfig->SlaveAddr = 1;						//设备默认通信地址
	pConfig->WaterLevelAddr = 1;				//水位计通信地址
	pConfig->WaterLevelSelectIndex = 0;			//水位计选择
	pConfig->FreqFilter = 60;					//频率差值滤波，IQ通道的频率差值超出范围后进行滤波
	pConfig->FlowSpeedFilterCnt = 20;			//流速滤波次数
	pConfig->FlowRateFilterCnt = 20;			//流量滤波次数
	pConfig->FlowDireFilterCnt = 10;			//流量方向滤波次数
	pConfig->ProtectTempH = 90;					//设备保护温度高温度值
	pConfig->ProtectTempL = -30;				//设备保护温度低温度值
}
 
 
 
/*************************************************************************************************************************
* 函数				:	bool CONFIG_SaveConfig(CONFIG_TYPE *pConfig)
* 功能				:	存储配置数据到flash
* 参数				:	pConfig:配置存储缓冲区
* 返回				:	FALSE:失败;TRUE:成功
* 依赖				:	flash操作函数
* 作者				:	cp1300@139.com
* 时间				:	2013-10-22
* 最后修改时间 		: 	2018-01-17
* 说明				: 	只能一次存储所有的配置数据
						配置数据应该小于1个页大小
						STM32F407配置写入基本原理：将一个64KB的扇区分为多个写入区域，比如64个，索引为0-63，如果当前存储的索引为63，则直接擦除扇区，索引变为0
						下次写入时，索引+1，同时检查待写入区域是否全部为FF，否则执行擦除，并将索引变为0，重复此循环，理论上可以写64次配置才擦除一次，可以有效的提高扇区利用率
						以及降低flash擦除次数，提高flash寿命。
*************************************************************************************************************************/
static bool CONFIG_SaveConfig(CONFIG_TYPE *pConfig)
{
	u32 addr;
	u16 count;
	STM32FLASH_STATUS status;
	u32 temp;
	bool isFistIndex = FALSE;																			//是否需要从索引0开始，并擦除当前扇区
	int i;
	
	count = STM32_CONFIG_PAGE_SIZE/CONFIG_MAX_SIZE;														//计算配置扇区可以存放配置数据的条数
	if(gs_ConfigSaveIndex == (count-1))																	//当前存储为最后一个索引了，从0开始存储
	{
		isFistIndex = TRUE;																				//需要写入到首个扇区
	}
	else //需要判断是否为FF
	{
		gs_ConfigSaveIndex ++;																			//存储索引位置自增
		for(i = 0;i < (sizeof(CONFIG_TYPE)/sizeof(u32));i ++)
		{
			addr = STM32_CONFIG_PAGE_ADDR+gs_ConfigSaveIndex*CONFIG_MAX_SIZE+i*4;						//计算需要读取的地址
			STM32FLASH_Read(addr, &temp, 4/sizeof(u32));
			if(temp != 0xFFFFFFFF)
			{
				//uart_printf("存储区域发现非0部分，需要擦除，当前索引：%d\r\n",gs_ConfigSaveIndex);
				gs_ConfigSaveIndex = 0;																	//索引变为0
				break;
			}
		}
		if(gs_ConfigSaveIndex == 0)																		//需要擦除
		{
			isFistIndex = TRUE;																			//需要写入到首个扇区
		}
	}
	
	if(isFistIndex== TRUE)	//需要写入到首个扇区
	{
        gs_ConfigSaveIndex = 0;		    //2019-11-21 修复，先将扇区索引清零
		//先擦除
		status = STM32FLASH_EraseSector(STM32_CONFIG_PAGE_INDEX);			  							//擦除扇区
		if(status == STM32FLASH_OK)
		{
			//uart_printf("擦除配置扇区%d成功\r\n",STM32_CONFIG_PAGE_INDEX);
		}
		else
		{
			//uart_printf("擦除配置扇区%d失败\r\n",STM32_CONFIG_PAGE_INDEX);
		}
		//写入数据
		addr = STM32_CONFIG_PAGE_ADDR+gs_ConfigSaveIndex*CONFIG_MAX_SIZE;								//计算需要写入的地址
		status = STM32FLASH_Write_NoCheck(addr,(u32 *)&g_SYS_Config,sizeof(CONFIG_TYPE)/sizeof(u32)) ; 	//不检查的写入-写入配置到flash
																			//索引变为0
	}
	else //非首个扇区写入-并且无需擦除，前面已经做好了检查，直接写入即可
	{
		addr = STM32_CONFIG_PAGE_ADDR+gs_ConfigSaveIndex*CONFIG_MAX_SIZE;								//计算需要写入的地址
		status = STM32FLASH_Write_NoCheck(addr,(u32 *)&g_SYS_Config,sizeof(CONFIG_TYPE)/sizeof(u32)) ; 	//不检查的写入-写入配置到flash
	}
	//判断是否写入成功
	if(status == STM32FLASH_OK)
	{
		//uart_printf("存储配置成功，索引：%d\r\n",gs_ConfigSaveIndex);
		return TRUE;
	}
	else
	{
		//uart_printf("存储配置失败，索引：%d(错误：%d)\r\n",gs_ConfigSaveIndex, status);
		return FALSE;
	}
}
 
 
 
 
 
/*************************************************************************************************************************
* 函数				:	bool CONFIG_WriteConfig(CONFIG_TYPE *pConfig)
* 功能				:	更新写入配置(不会修改SN)
* 参数				:	pConfig:配置存储缓冲区
* 返回				:	FALSE:失败;TRUE:成功
* 依赖				:	flash操作函数
* 作者				:	cp1300@139.com
* 时间				:	2017-04-05
* 最后修改时间 		: 	2017-04-05
* 说明				: 	用于更新系统配置，并且写入配置到flash,不检查配置，需要提前进行验证配置
*************************************************************************************************************************/
bool CONFIG_WriteConfig(CONFIG_TYPE *pConfig)
{
	int i;
	
	/****************************************头部的注册相关信息，不管什么设备的配置开头必须是以下格式************************************************************************************/
	g_SYS_Config.ID = DEVICE_BOARD_CONFIG_ID;							//ID,标记是否进行过配置
	g_SYS_Config.SN[15] = 0;											//唯一序列号-不能进行随意修改							
	/*********************************************************************************************************************************************************************/
	g_SYS_Config.HorizAngle 						= pConfig->HorizAngle ;					//设备安装的水平夹角，保留3位小数
	g_SYS_Config.ThresholdEnergy 					= pConfig->ThresholdEnergy;				//阈值能量，低于阈值的将会进行过滤
	g_SYS_Config.WaterLevelHeight 					= pConfig->WaterLevelHeight;			//水位计安装高度
	g_SYS_Config.WaterCorrParame 					= pConfig->WaterCorrParame;				//水位计线性修正值，4位小数点
	g_SYS_Config.WaterCorrectionLe 					= pConfig->WaterCorrectionLe;			//水位修正值,有符号,单位mm
	
	g_SYS_Config.WarmTime 							= pConfig->WarmTime;					//预热时间，单位S，0-999
	g_SYS_Config.FlowSpeedCorrParame 				= pConfig->FlowSpeedCorrParame;			//流速线性修正值，4位小数点
	g_SYS_Config.FlowSpeedCorrectionLe 				= pConfig->FlowSpeedCorrectionLe;		//流速修正值,有符号,单位mm
	g_SYS_Config.AcqCycle 							= pConfig->AcqCycle;					//采集周期，单位秒
	for(i = 0;i < 16;i ++)
	{
		g_SYS_Config.Filter[i][0] 					= pConfig->Filter[i][0];				//滤波器，预留16个滤波器，可以进行干扰频率过滤
		g_SYS_Config.Filter[i][1] 					= pConfig->Filter[i][1];				//滤波器，预留16个滤波器，可以进行干扰频率过滤
	}
	
	
	g_SYS_Config.SlaveAddr 							= pConfig->SlaveAddr;					//设备通信地址
	g_SYS_Config.WaterLevelAddr 					= pConfig->WaterLevelAddr;				//水位计通信地址
	g_SYS_Config.WaterLevelSelectIndex 				= pConfig->WaterLevelSelectIndex;		//水位计选择
	g_SYS_Config.FreqFilter 						= pConfig->FreqFilter;					//频率差值滤波，IQ通道的频率差值超出范围后进行滤波
	g_SYS_Config.FlowSpeedFilterCnt 				= pConfig->FlowSpeedFilterCnt;			//流速滤波次数
	g_SYS_Config.FlowRateFilterCnt 					= pConfig->FlowRateFilterCnt;			//流量滤波次数
	g_SYS_Config.FlowDireFilterCnt 					= pConfig->FlowDireFilterCnt;			//流量方向滤波次数
	g_SYS_Config.ProtectTempH 						= pConfig->ProtectTempH;				//设备保护温度高温度值
	g_SYS_Config.ProtectTempL 						= pConfig->ProtectTempL;				//设备保护温度低温度值
	
 
	return CONFIG_SaveConfig(&g_SYS_Config);
}
 
 
 
/*************************************************************************************************************************
* 函数				:	bool CONFIG_WriteSN(char pSN[16])
* 功能				:	修改SN
* 参数				:	pSN：SN
* 返回				:	FALSE:失败;TRUE:成功
* 依赖				:	flash操作函数
* 作者				:	cp1300@139.com
* 时间				:	2017-04-05
* 最后修改时间 		: 	2018-01-31
* 说明				: 	不会检查SN合法性
*************************************************************************************************************************/
bool CONFIG_WriteSN(char pSN[16])
{
	g_SYS_Config.ID = DEVICE_BOARD_CONFIG_ID;							//ID,标记是否进行过配置
	pSN[15] = 0;						
	strcpy(g_SYS_Config.SN, pSN);										//唯一序列号-不能进行随意修改
	
	return CONFIG_SaveConfig(&g_SYS_Config);
}
 
 
 
 
/*************************************************************************************************************************
* 函数				:	bool CONFIG_CheckSN(char pSN[16])
* 功能				:	检查SN的有效性
* 参数				:	pSN:序列号存储缓冲区
* 返回				:	TRUE:有效；FALSE:无效，没有配置SN
* 依赖				:	flash操作函数
* 作者				:	cp1300@139.com
* 时间				:	2014-10-18
* 最后修改时间 		: 	2018-01-17
* 说明				: 	检查SN的有效性
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
 
 
 
 
 
 
 
 
 
 
 
 