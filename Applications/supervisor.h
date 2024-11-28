#ifndef __SUPERVISOR_H
#define __SUPERVISOR_H

#include "bsp.h"
#include "FreeRTOS_board.h"
#include "board.h"

/*--------------------------------------------- 
-----------------------------------------------
//		警告：公共代码区域，故障责任重大				   //
//			除特定区域可以自由可以更改外，		  		 //
//	其余部分更改，请与公共区域负责人取得更改权限  // 
-----------------------------------------------
---------------------------------------------*/

#define SPUER_PRIORITY   13
#define SPUER_STACK_SIZE 512
#define SUPER_STACK_PERIOD 5

enum {
	IMU_TASK = 0,
	SUPERVISOR_TASK,
	AHRS_TASK,
	CONTROL_TASK,
	UKF_INIT_TASK,
	WIRELESS_TASK,	
	LCD_TASK,
	ODRIVE_TASK,
	XHC_Task,
	CAN1_Task,
	LIST_OF_TASK,
	
};

#define IMU_SENSOR_ERROR	 15
#define SHOOT_ERROR        1
#define UART_NO_RECEIVE		 2
#define ALL_DEVICE_IS_WELL 9

#define LED_IMU_HARDWORK_FALSE 20			//20 浅蓝灯快闪 

#define LED_VISION_HARDWORK_FALSE	1		//红灯慢闪
#define LED_ODRIVE_FALSE 15				//红灯快闪
#define LED_IMU_CALI_FALSE	16				//绿灯快闪
#define LED_RADIO_LOSS	18						//蓝灯快闪
#define LED_STATE_NORMAL	9						//绿灯正常速度闪

/*
3 黄灯慢闪
4 深蓝灯慢闪
5 紫灯慢闪
6 浅蓝灯慢闪
7 浅浅蓝灯慢闪
8 红灯正常闪
10 黄灯正常闪
11 蓝灯正常闪
12 粉灯正常闪
13 浅蓝灯正常闪
14 浅浅蓝灯正常闪
17 黄灯快闪 
19 紫灯快闪 
20 浅蓝灯快闪 
21 浅浅蓝灯快闪 

*/





enum supervisorStates {
	STATE_INITIALIZING	= 0x0000,														//初始化状态
	STATE_MAGCALI	= 0x0001,																	//磁力计校准状态
	STATE_DISARMED	= 0x0002,																//上锁
	STATE_ARMED		= 0x0004,																	//解锁
	STATE_IMUCALI	= 0x0008,																	//IMU校准
	STATE_RADIO_LOSS	= 0x0010,															//遥控丢失
	STATE_SENSOR_ERROR = 0x0020,														//传感器错误
	STATE_LOW_BATTERY	= 0x0040,															//电池
	STATE_VISION_ERROR = 0x0100,														//视觉端错误
	STATE_ODRIVE_ERROR = 0x0200,												//与ODRIVE通信错误
	STATE_IMU_LOSS = 0x0400,																//陀螺仪丢失
};

enum{
	MODULAR_BULLET_MONITR = 0x01,
	MODULAR_IMU_LOSS = 0x02,
	MODULAR_IMU_CALI = 0x04,
};

typedef struct {
	TaskHandle_t xHandleTask;
	BaseType_t taskEvent[LIST_OF_TASK];
	uint8_t taskState[LIST_OF_TASK];
	uint16_t state;
	bool imuTempState;
	bool busyState;
	uint8_t flashSave;
	u8 eraseflash;
	volatile uint8_t imuTempFinish;
	uint16_t rgbState;
    uint16_t beepState;
	uint32_t loops;
	uint8_t modularState;
    uint32_t intervalNum;	    	
    uint32_t Lobot_ErrorCount;
    uint32_t Lobot_LastErrorCount;    
    uint16_t gimbalCaliReset ;       
} supervisorStruct_t;

supervisorStruct_t* getsupervisorData(void);
extern supervisorStruct_t supervisorData;
void supervisorInit(void);
void shootCheckDevice(void);
void supervisrCheckModule(void);
void supervisorGimbalCali(void);
void Odrive_CheckDevice(void);

#endif
