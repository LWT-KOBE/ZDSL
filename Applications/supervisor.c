#include "supervisor.h"
#include "driver.h"
#include "stdbool.h"

supervisorStruct_t supervisorData;

supervisorStruct_t* getsupervisorData(){
    return &supervisorData;
}



/*--------------------------------------------- 
-----------------------------------------------
//		警告：公共代码区域，故障责任重大				   //
//			除特定区域可以自由可以更改外，		  		 //
//	其余部分更改，请与公共区域负责人取得更改权限  // 
-----------------------------------------------
---------------------------------------------*/

void supervisorStateSwitch(uint16_t state,uint8_t valve){							//用于切换监控状态机状态的																												
	if (valve)
		supervisorData.state |= state;
	else
		supervisorData.state &= ~state;
}

void supervisorImuCali(void){		
	
//	//IMU校准指令
//	supervisorStateSwitch(STATE_IMUCALI,ENABLE);		
//	
//	//是否开启加速度计校准	
//	digitalHi(&imuSensorData.accTare);

}

void supervisrCheckModule(void){
//	//检测是否在IMU校准状态
//	if(supervisorData.state & STATE_IMUCALI)
//		supervisorData.modularState |= MODULAR_IMU_CALI;
//	else
//		supervisorData.modularState &= ~MODULAR_IMU_CALI;
//	//检测IMU是否丢失
//	if(supervisorData.state & STATE_IMU_LOSS)
//		supervisorData.modularState |= MODULAR_IMU_LOSS;
//	else
//		supervisorData.modularState &= ~MODULAR_IMU_LOSS;

}


//检测Odrive是否有数据反馈
void Odrive_CheckDevice(void){
	
	OdriveData.OdError.intervalNum = OdriveData.OdError.errorCount - OdriveData.OdError.lastErrorCount;
	OdriveData.OdError.lastErrorCount = OdriveData.OdError.errorCount;
	if(!OdriveData.OdError.intervalNum){
		supervisorStateSwitch(STATE_ODRIVE_ERROR,ENABLE);							
	}
	else{
		supervisorStateSwitch(STATE_ODRIVE_ERROR,DISABLE);           
	}
}






void supervisorLedSwitch(void){
  if(supervisorData.state & STATE_IMU_LOSS){//绿灯快闪
		supervisorData.rgbState = LED_IMU_CALI_FALSE;									//imu传感器错误工作灯
	}
	else if(supervisorData.state & STATE_IMUCALI){
		supervisorData.rgbState = LED_IMU_HARDWORK_FALSE;											//imu校准工作灯
	}
	else if(supervisorData.state & STATE_ODRIVE_ERROR){
		supervisorData.rgbState = LED_ODRIVE_FALSE;										//与Odrive通信错误
	}
	else{
		supervisorData.rgbState = LED_STATE_NORMAL;															//工作正常
	}
}





//Flash储存
void supervisorFlash(void){		
    //如果想要对flash进行操作，直接将supervisorData.flashSave拉高一次即可    
	if(supervisorData.flashSave){                                              
       //检测到需要存入Flash		参数保存提示音
		//supervisorData.beepState = MUSIC_PARAMCALI;		        
		configFlashWrite();                                             	//写入flash
		CONFIG_WriteConfig(&g_SYS_Config);
		digitalLo(&supervisorData.flashSave);         
        
	} 	
}


//恢复出厂设置
void eraseFlash(void){		
    //如果想要对flash进行操作，直接将supervisorData.flashSave拉高一次即可    
	if(supervisorData.eraseflash){                                              
       //检测到需要存入Flash		参数保存提示音
		//supervisorData.beepState = MUSIC_PARAMCALI;		        
		erase_configuration();                                             	//写入flash
		CONFIG_Default(&g_SYS_Config,1);
		digitalLo(&supervisorData.eraseflash);         
        
	} 	
}


bool testImuCali = false,testAccCali = false;
void supervisorUpdateTask(void *Parameters){
	TickType_t xLastWakeTime = xTaskGetTickCount();
	while(true){
		vTaskDelayUntil(&xLastWakeTime,SUPER_STACK_PERIOD);

        
		//RGB更新
		supervisorLedSwitch();
		//检测是否需要更新flash        
		supervisorFlash();
		eraseFlash();
        //蜂鸣器更新
        //LED闪动		主控板上3色LED闪动指示状态										
        appSightClass.led(supervisorData.rgbState);                           
        //蜂鸣器正常响起
        //appSightClass.beep(supervisorData.beepState);	      
		//Odrive_CheckDevice();		
        //状态清零
        digitalClan(&supervisorData.beepState);	        
		digitalIncreasing(&supervisorData.loops);
	}
}

void supervisorInit(void){
    
	//蜂鸣器初始化，和LED一起属于监管状态机
	//sightClass.Init();																												
	supervisorData.taskEvent[SUPERVISOR_TASK] = xTaskCreate(supervisorUpdateTask,"SUPE",SPUER_STACK_SIZE,NULL,SPUER_PRIORITY,&supervisorData.xHandleTask);
}
