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
/*------------------- ����FLASH��ȡ ---------------------*/
void configFlashRead(void) {
    float *recs;
    int i;
    //�������׵�ַ��ʼ��ȡ
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
/*------------------- ����FLASHд�� ---------------------*/
uint8_t configFlashWrite(void) {

    float *recs;
	uint8_t ret = 0;
	int i;
    
    //����һ�ζ�̬�ڴ����ڴ�����ʱ��Ϣ
	recs = (void *)aqCalloc(sizeof(systemConfig_t), sizeof(float));
    //����ɹ����뵽�ڴ��ַ��ִ��д�����
	if (recs) {
        //��������ʼ��ַ��ʼ����flash
		ret = flashErase(flashStartAddr(), sizeof(systemConfig_t) * sizeof(float) / sizeof(uint32_t));
        //ʹ�������ݻ�����Ч	        
		FLASH_DataCacheCmd(DISABLE);
        //�������ݻ���
		FLASH_DataCacheReset();
        //�������ݻ��湦��
		FLASH_DataCacheCmd(ENABLE);
        //��������ɹ���ִ��д�����
		if (ret) {
            //���ڴ��д��������б�
			for (i = 0; i < sizeof(systemConfig_t); i++) {
                recs[i] = *((&systemConfigData.configVersion) + i);
			}
            //��flash���ݣ���������ʼ��ַ��ǰ��
			ret = flashAddress(flashStartAddr(), (uint32_t *)recs, \
                                sizeof(systemConfig_t) * sizeof(float) / sizeof(uint32_t));
		}
        else {
            //usbVCP_Printf("Flash erase failed! \r\n");
        }
        //�ͷ��ڴ�
		aqFree(recs, sizeof(systemConfig_t), sizeof(float));
	}
	else {
        //usbVCP_Printf("Flash failed to apply for memory! \r\n");
    }
	return ret;




}

//�ָ���������
void erase_configuration(void){
	configLoadDefault();
	configFlashWrite();	
}

void configInit(void) {

	float ver;	
    //��Flash�п�ʼ
	//configFlashRead();
	configLoadDefault();
	CONFIG_Init();
    //��ȡ��ǰflash�汾
//	ver = *(float *)(flashStartAddr());																
//	if (isnan(ver))
//		ver = 0.0f;
//    //��������Ĭ��ֵ����flash�汾�ͼ��ذ汾																											
//	if (DEFAULT_CONFIG_VERSION > ver || DEFAULT_CONFIG_VERSION > systemConfigData.configVersion){
//        //����Ĭ��ֵ
//		configLoadDefault();																					
//		digitalHi(&getsupervisorData()->flashSave);	
//	}
//    //���flash�汾���ڵ�ǰ����ڵ�ǰ�汾
//	else if (ver >= systemConfigData.configVersion){                                                              
//        //��ȡflash   
//        configFlashRead();																								
//	}																			
//    //������صİ汾����flash�汾
//	else if (systemConfigData.configVersion > ver){                                                                          
//        //д��flash,������ֻ��������SD��ʱ����SD���еİ汾����flash�еİ汾�Żᷢ��        
//        configFlashWrite();	
//	}																											
//    usbVCP_Printf("ConfigInit Successfully \r\n");    
    
    
    
    
}


