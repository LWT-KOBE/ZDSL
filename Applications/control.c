#include "application.h"
#include "usartx.h"
#include "board.h"
controlStruct_t controlData;
int Menu=1,Menu1=0,Menu2=0;
float Set_Cur=0,Set_Pos=0,Set_Vel=0;

uint8_t temp[8]={0};

controlStruct_t* getcontrolData(){
    return &controlData;
}

void congtrolGlobalInit(void){
	//uart1_init(115200);
	
//	uart2_init(9600);//JY60
//	//uart2_init(115200);
//	uart5_init(115200);
//	cigan_Init();
}

void controlUpdateTask(void *Parameters){
	TickType_t xLastWakeTime = xTaskGetTickCount();
	digitalLo(&getcontrolData()->dataInitFlag);
	while(true){
		vTaskDelayUntil(&xLastWakeTime,CONTROL_NORMAL_PERIOD);
        //防止重复初始化
		if(!controlData.dataInitFlag){	
            //所有控制全部初始化            
			congtrolGlobalInit();																																							
			digitalHi(&getcontrolData()->dataInitFlag);
			
		}
		
		temp[0] = 0xfa;
		temp[1] = 0x00;
		temp[2] = OdReceivedData.vel_estimate[1].u8_temp[0];
		temp[3] = OdReceivedData.vel_estimate[1].u8_temp[1];
		temp[4] = OdReceivedData.vel_estimate[1].u8_temp[2];
		temp[5] = OdReceivedData.vel_estimate[1].u8_temp[3];
		temp[6] = Motor_SpeedB_Goal.finish;
		
		u3_SendArray(temp,8);
		
		memset(temp, 0, sizeof(temp));
		
		digitalIncreasing(&getcontrolData()->loops);        

	}
}

void controlInit(void){
	getsupervisorData()->taskEvent[CONTROL_TASK] = xTaskCreate(controlUpdateTask,"CONTROL",CONTROL_STACK_SIZE,NULL,CONTROL_PRIORITY,&controlData.xHandleTask);
    //usbVCP_Printf("ControlInit Successfully \r\n");
    
}
