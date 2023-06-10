#ifndef __LED_H_
#define __LED_H_
#include "main.h"


#define LED_POWER_TOGGLE()            	HAL_GPIO_TogglePin(LED_POWER_GPIO_Port, LED_POWER_Pin)
#define LED_POWER_ON()					HAL_GPIO_WritePin(LED_POWER_GPIO_Port, LED_POWER_Pin,GPIO_PIN_SET)
#define LED_POWER_OFF()            		HAL_GPIO_WritePin(LED_POWER_GPIO_Port, LED_POWER_Pin,GPIO_PIN_RESET)


#define LED_WIFI_TOGGLE()          HAL_GPIO_TogglePin(WIFI_LED_EN_GPIO_Port, WIFI_LED_EN_Pin)

#define LED_AI_ON()			  HAL_GPIO_WritePin(WIFI_LED_EN_GPIO_Port , WIFI_LED_EN_Pin,GPIO_PIN_SET)
#define LED_AI_OFF()    	      HAL_GPIO_WritePin(WIFI_LED_EN_GPIO_Port , WIFI_LED_EN_Pin,GPIO_PIN_RESET)

#define LED_DRY_ON()			  HAL_GPIO_WritePin(DRY_LED_EN_GPIO_Port, DRY_LED_EN_Pin,GPIO_PIN_SET)
#define LED_DRY_OFF()             HAL_GPIO_WritePin(DRY_LED_EN_GPIO_Port, DRY_LED_EN_Pin,GPIO_PIN_RESET)

#define LED_TIME_ON()			   HAL_GPIO_WritePin(TIME_LED_EN_GPIO_Port, TIME_LED_EN_Pin ,GPIO_PIN_SET)
#define LED_TIME_OFF()             HAL_GPIO_WritePin(TIME_LED_EN_GPIO_Port, TIME_LED_EN_Pin ,GPIO_PIN_RESET)

#define LED_PLASMA_ON()				HAL_GPIO_WritePin(PLASMA_LED_EN_GPIO_Port, PLASMA_LED_EN_Pin,GPIO_PIN_SET)
#define LED_PLASMA_OFF()            HAL_GPIO_WritePin(PLASMA_LED_EN_GPIO_Port, PLASMA_LED_EN_Pin,GPIO_PIN_RESET)

#define LED_FAN_ON()  				HAL_GPIO_WritePin(FAN_LED_EN_GPIO_Port, FAN_LED_EN_Pin,GPIO_PIN_SET)
#define LED_FAN_OFF()				HAL_GPIO_WritePin(FAN_LED_EN_GPIO_Port, FAN_LED_EN_Pin,GPIO_PIN_RESET)


//control SMG supply power 
#define SMG_POWER_ON()				HAL_GPIO_WritePin(SMG_POWER_GPIO_Port , SMG_POWER_Pin,GPIO_PIN_SET)
#define SMG_POWER_OFF()             HAL_GPIO_WritePin(SMG_POWER_GPIO_Port , SMG_POWER_Pin,GPIO_PIN_RESET)







extern void (*panel_led_fun)(void);

void Panel_Led_OnOff_RunCmd(void (*panelledHandler)(void));

//void KeyLed_Power_On(void);


//void ALL_LED_Off(void);

void Panel_Led_OnOff_Function(void);

void AI_Auto_Stop(void);
void Breath_Led(void);

void Power_Off_Led_Off(void);


void Power_ON_Led(void);





#endif 
