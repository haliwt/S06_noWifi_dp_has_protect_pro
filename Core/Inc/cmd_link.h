#ifndef __CMD_LINK_H_
#define __CMD_LINK_H_

#include "main.h"
#define  MAX_BUFFER_SIZE   8
#define MAX_BUFFER_BUF   6

extern uint8_t inputBuf[MAX_BUFFER_SIZE];
extern uint8_t outputBuf[8];
extern uint8_t parse_buf[MAX_BUFFER_BUF];


typedef enum{

     WIFI_CLOUD_FAIL,
	 WIFI_CLOUD_SUCCESS

}wifi_cloud_state;


void SendData_PowerOff(uint8_t index);

void SendData_Set_Wifi(uint8_t hdata);
void SendData_Set_Command(uint8_t hdata);

void SendData_Temp_Data(uint8_t tdata);


void SendData_Time_Data(uint8_t tdata);

void SendData_Buzzer(void);

void SendData_Works_Time(uint8_t tdata,uint8_t tdata_2);

void SendData_Remaining_Time(uint8_t tdata,uint8_t tdata_2);
void USART1_Cmd_Error_Handler(void);


#endif 




