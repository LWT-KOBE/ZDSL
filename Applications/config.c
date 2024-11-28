#include "driver_flash.h"
#include "config.h"
#include "stdio.h"
#include "string.h"
#include "math.h"
#include "board.h"
systemConfig_t  systemConfigData __attribute__((at(0x10006000)));
systemConfig_t *getConfigData(void){
    return &systemConfigData;
}




void configLoadDefault(void){	
	systemConfigData.configVersion = DEFAULT_CONFIG_VERSION;
	
	systemConfigData.SM_Enable[0] = 1;
	systemConfigData.SM_Enable[1] = 1;
	systemConfigData.SM_Enable[2] = 1;
	systemConfigData.SM_Enable[3] = 1;
}


configToken_t *configTokenFindEmpty(void) {
	configToken_t *p = (configToken_t *)(FLASH_END_ADDR + 1);
	do {
		p--;
	} while (p->key != 0xffffffff);
	return p;
}

void configTokenStore(configToken_t *token) {
  flashAddress((uint32_t)configTokenFindEmpty(), (uint32_t *)token, sizeof(configToken_t)/sizeof(uint32_t));
}

configToken_t *configTokenGet(uint32_t key) {
	configToken_t *p, *t;
	p = (configToken_t *)(FLASH_END_ADDR + 1);
	t = 0;
	do {
		p--;
		if (p->key == key)
			t = p;
	} while (p->key != 0xffffffff);
	return t;
}
/*------------------- 配置FLASH读取 ---------------------*/
void configFlashRead(void) {
    float *recs;
    int i;
    //从扇区首地址开始读取
    recs = (void *)flashStartAddr();
    for (i = 0; i < sizeof(systemConfig_t); i++) {
        *((&systemConfigData.configVersion) + i) = recs[i];
    }    
    
    
}

configToken_t *configTokenIterate(configToken_t *t) {
	if (t == 0)
		t = (configToken_t *)(FLASH_END_ADDR + 1);
	t--;
	if (t->key != 0xffffffff)
		return t;
	else
		return 0;
}
/*------------------- 配置FLASH写入 ---------------------*/
uint8_t configFlashWrite(void) {

    float *recs;
	uint8_t ret = 0;
	int i;
    
    //申请一段动态内存用于储存临时信息
	recs = (void *)aqCalloc(sizeof(systemConfig_t), sizeof(float));
    //如果成功申请到内存地址就执行写入操作
	if (recs) {
        //从扇区初始地址开始擦除flash
		ret = flashErase(flashStartAddr(), sizeof(systemConfig_t) * sizeof(float) / sizeof(uint32_t));
        //使闪存数据缓存无效	        
		FLASH_DataCacheCmd(DISABLE);
        //重置数据缓存
		FLASH_DataCacheReset();
        //启用数据缓存功能
		FLASH_DataCacheCmd(ENABLE);
        //如果擦除成功，执行写入操作
		if (ret) {
            //在内存中创建参数列表
			for (i = 0; i < sizeof(systemConfig_t); i++) {
                recs[i] = *((&systemConfigData.configVersion) + i);
			}
            //存flash数据，从扇区起始地址往前存
			ret = flashAddress(flashStartAddr(), (uint32_t *)recs, \
                                sizeof(systemConfig_t) * sizeof(float) / sizeof(uint32_t));
		}
        else {
            //usbVCP_Printf("Flash erase failed! \r\n");
        }
        //释放内存
		aqFree(recs, sizeof(systemConfig_t), sizeof(float));
	}
	else {
        //usbVCP_Printf("Flash failed to apply for memory! \r\n");
    }
	return ret;




}

//恢复出厂设置
void erase_configuration(void){
	configLoadDefault();
	configFlashWrite();	
}

void configInit(void) {

	float ver;	
    //从Flash中开始
	//configFlashRead();
	configLoadDefault();
	CONFIG_Init();
    //读取当前flash版本
//	ver = *(float *)(flashStartAddr());																
//	if (isnan(ver))
//		ver = 0.0f;
//    //如果编译的默认值大于flash版本和加载版本																											
//	if (DEFAULT_CONFIG_VERSION > ver || DEFAULT_CONFIG_VERSION > systemConfigData.configVersion){
//        //加载默认值
//		configLoadDefault();																					
//		digitalHi(&getsupervisorData()->flashSave);	
//	}
//    //如果flash版本大于当前或等于当前版本
//	else if (ver >= systemConfigData.configVersion){                                                              
//        //读取flash   
//        configFlashRead();																								
//	}																			
//    //如果加载的版本大于flash版本
//	else if (systemConfigData.configVersion > ver){                                                                          
//        //写入flash,这个情况只存在于有SD卡时，且SD卡中的版本高于flash中的版本才会发生        
//        configFlashWrite();	
//	}																											
//    usbVCP_Printf("ConfigInit Successfully \r\n");    
    
    
    
    
}


