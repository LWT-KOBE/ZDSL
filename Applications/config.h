#ifndef __CONFIG_H
#define __CONFIG_H

#include "stm32f4xx.h"
#include "util.h"

#define CONFIG_FILE_BUF_SIZE	  512
#define CONFIG_LINE_BUF_SIZE	  128


#define DEFAULT_CONFIG_VERSION	128

#define DEFAULT_IMU_ACC_BIAS_X  0.0f
#define DEFAULT_IMU_ACC_BIAS_Y  0.0f
#define DEFAULT_IMU_ACC_BIAS_Z  0.0f
#define DEFAULT_IMU_MAG_BIAS_X  0.0f
#define DEFAULT_IMU_MAG_BIAS_Y  0.0f
#define DEFAULT_IMU_MAG_BIAS_Z  0.0f
#define DEFAULT_IMU_GYO_BIAS_X  0.0f
#define DEFAULT_IMU_GYO_BIAS_Y  0.0f
#define DEFAULT_IMU_GYO_BIAS_Z  0.0f




#define DEFAULT_YAW_ANGLE_P					0.22f
#define DEFAULT_YAW_ANGLE_I					0.01f
#define DEFAULT_YAW_ANGLE_D					0.30f
#define DEFAULT_YAW_ANGLE_F					0.25f
#define DEFAULT_YAW_ANGLE_PM				8.00f
#define DEFAULT_YAW_ANGLE_IM				0.30f
#define DEFAULT_YAW_ANGLE_DM				8.00f
#define DEFAULT_YAW_ANGLE_OM				8.30f   

#define DEFAULT_PITCH_ANGLE_P				0.38f
#define DEFAULT_PITCH_ANGLE_I				0.01f
#define DEFAULT_PITCH_ANGLE_D				0.37f
#define DEFAULT_PITCH_ANGLE_F  			    0.25f
#define DEFAULT_PITCH_ANGLE_PM			    8.00f	
#define DEFAULT_PITCH_ANGLE_IM			    0.30f
#define DEFAULT_PITCH_ANGLE_DM			    8.00f
#define DEFAULT_PITCH_ANGLE_OM			    8.30f

#define DEFAULT_YAW_RATE_P					170.00f
#define DEFAULT_YAW_RATE_I					20.00f
#define DEFAULT_YAW_RATE_D					0.00f
#define DEFAULT_YAW_RATE_F					0.25f
#define DEFAULT_YAW_RATE_PM					10000.0f
#define DEFAULT_YAW_RATE_IM					2000.0f
#define DEFAULT_YAW_RATE_DM					10000.0f
#define DEFAULT_YAW_RATE_OM					10000.0f

#define DEFAULT_PITCH_RATE_P				11.00f
#define DEFAULT_PITCH_RATE_I				100.00f
#define DEFAULT_PITCH_RATE_D				4.60f
#define DEFAULT_PITCH_RATE_F				0.25f
#define DEFAULT_PITCH_RATE_PM				400.00f
#define DEFAULT_PITCH_RATE_IM				400.00f
#define DEFAULT_PITCH_RATE_DM				400.00f
#define DEFAULT_PITCH_RATE_OM				400.00f


#define DEFAULT_ROLL_ANGLE_P				0.22f
#define DEFAULT_ROLL_ANGLE_I				0.01f
#define DEFAULT_ROLL_ANGLE_D				0.30f
#define DEFAULT_ROLL_ANGLE_F				0.25f
#define DEFAULT_ROLL_ANGLE_PM				8.00f
#define DEFAULT_ROLL_ANGLE_IM				0.30f
#define DEFAULT_ROLL_ANGLE_DM				8.00f
#define DEFAULT_ROLL_ANGLE_OM				8.30f    


/*----------------	电机参数		---------------*/
#define DEFAULT_LOCAL_ID				0x0101
#define DEFAULT_WEAPON_TYPE				0
#define DEFAULT_BACK_CENTER_TIME        500.0f
#define DEFAULT_CHASSIS_CURRENT			3000.0f
#define DEFAULT_RC_RESOLUTION			500.0f
#define DEFAULT_YAW_CENTER 				0.0f
#define	DEFAULT_PITCH_CENTER			0.0f
#define	DEFAULT_ROLL_CENTER				0.0f

#define	DEFAULT_YAW_FIX                 2
#define	DEFAULT_YAW_TURN                1
#define	DEFAULT_YAW_TYPE                6
#define DEFAULT_YAW_INSTALL				1


#define	DEFAULT_PITCH_FIX               2 
#define	DEFAULT_PITCH_TURN              1
#define	DEFAULT_PITCH_TYPE              5
#define DEFAULT_PITCH_INSTALL			1





//使用宏定义调用三个轴的电机配置

#define YAW_FIX_CONFIG             getConfigData()->yawSetting.u8_temp[0]
#define YAW_TURN_CONFIG            getConfigData()->yawSetting.u8_temp[1]
#define YAW_TYPE_CONFIG            getConfigData()->yawSetting.u8_temp[2]
#define YAW_INSTALL_CONFIG         getConfigData()->yawSetting.u8_temp[3]


#define PITCH_FIX_CONFIG           getConfigData()->pitchSetting.u8_temp[0]
#define PITCH_TURN_CONFIG          getConfigData()->pitchSetting.u8_temp[1]
#define PITCH_TYPE_CONFIG          getConfigData()->pitchSetting.u8_temp[2]
#define PITCH_INSTALL_CONFIG       getConfigData()->pitchSetting.u8_temp[3]


//#define ROLL_FIX_CONFIG            getConfigData()->rollSetting.u8_temp[0]
//#define ROLL_TURN_CONFIG           getConfigData()->rollSetting.u8_temp[1]
//#define ROLL_TYPE_CONFIG           getConfigData()->rollSetting.u8_temp[2]
//#define ROLL_INSTALL_CONFIG        getConfigData()->rollSetting.u8_temp[3]




typedef struct{
    float PID_P;
    float PID_I;
    float PID_D;
    float PID_F;
    float PID_PM;
    float PID_IM;
    float PID_DM;
    float PID_OM;
}systemConfigPID_t;

typedef struct {
	
	
    float configVersion; 
	u8 SM_Enable[4];
//    //三轴角度环PID
//    systemConfigPID_t yawAnglePID;
//    systemConfigPID_t pitchAnglePID;
//    systemConfigPID_t rollAnglePID;
//    //三轴速度环PID
//    systemConfigPID_t yawRatePID;
//    systemConfigPID_t pitchRatePID;

//    formatTrans32Struct_t yawSetting;
//    formatTrans32Struct_t pitchSetting;
//	
//	f32_t				imu_acc_bias_x;
//	f32_t				imu_acc_bias_y;
//	f32_t				imu_acc_bias_z;
//	
//	f32_t				imu_gyo_bias_x;
//	f32_t				imu_gyo_bias_y;
//	f32_t				imu_gyo_bias_z;	

//	f32_t				imu_mag_bias_x;
//	f32_t				imu_mag_bias_y;
//	f32_t				imu_mag_bias_z;	

//    float pitchCenter;
//    float yawCenter;
//    float backCenterTime;
} systemConfig_t;

systemConfig_t *getConfigData(void);


typedef struct {
    uint32_t key;
    char data[24];
} configToken_t;

typedef struct {
    char name[16];
    float val;
} configRec_t;

extern const char *configParameterStrings[] ;
void configFlashRead(void);
uint8_t configFlashWrite(void);
void configLoadDefault(void);
unsigned int configParameterRead(void *data);
unsigned int configParameterWrite(void *data);
int8_t configReadFile(char *fname);
int8_t configWriteFile(char *fname);
configToken_t *configTokenGet(uint32_t key);
void configTokenStore(configToken_t *token);
void configLoadDefault(void);
void configInit(void);
void erase_configuration(void);


extern systemConfig_t  systemConfigData;

#endif





