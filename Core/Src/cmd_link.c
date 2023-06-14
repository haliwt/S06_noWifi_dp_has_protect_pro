#include "cmd_link.h"
#include "usart.h"
#include "gpio.h"
#include "run.h"
#include "display.h"
#include "led.h"

volatile static uint8_t transOngoingFlag; //interrupt Transmit flag bit , 1---stop,0--run
uint8_t outputBuf[8];
static uint8_t transferSize;
static uint8_t state;
uint8_t inputBuf[MAX_BUFFER_SIZE];
uint8_t parse_buf[MAX_BUFFER_BUF];


/****************************************************************************************************
**
*Function Name:static void selectLight(uint8_t index)
*Function: UART2 transmit interrupt process ---4D 58 4C 53 32 30 32 
*Input Ref: LED number 
*Return Ref:NO
*
****************************************************************************************************/
void SendData_PowerOff(uint8_t index)
{
	
   //crc=0x55;
	outputBuf[0]='T'; //4D //Touch Key
	outputBuf[1]='K'; //58
	outputBuf[2]='P'; //4C	// 'P' power
	outputBuf[3]=index; //53	// 'S' select light command, 'C' close all light command
	
    transferSize=4;
	if(transferSize)
	{
		while(transOngoingFlag);
		transOngoingFlag=1;
		HAL_UART_Transmit_IT(&huart1,outputBuf,transferSize);
	}
	
}
void SendData_Buzzer(void)
{
	
   //crc=0x55;
	outputBuf[0]='T'; //4D
	outputBuf[1]='K'; //58
	outputBuf[2]='Z'; //4C	// buzzer
	outputBuf[3]='Z'; //53	// buzzer
	
    transferSize=4;
	if(transferSize)
	{
		while(transOngoingFlag);
		transOngoingFlag=1;
		HAL_UART_Transmit_IT(&huart1,outputBuf,transferSize);
	}
	
}
/*********************************************************
 * 
 * Function Name:void SendData_Temp_Data(uint8_t tdata)
 * Function:send 
 * 
*********************************************************/
void SendData_Set_Wifi(uint8_t hdata)
{
	    outputBuf[0]='T'; //
		outputBuf[1]='K'; //58
		outputBuf[2]='W'; //W ->WIFI
		outputBuf[3]=hdata; 
		
		transferSize=4;
		if(transferSize)
		{
			while(transOngoingFlag);
			transOngoingFlag=1;
			HAL_UART_Transmit_IT(&huart1,outputBuf,transferSize);
		}
	
}
/*********************************************************
 * 
 * Function Name:void SendData_Temp_Data(uint8_t tdata)
 * Function:send temperature value 
 * 
*********************************************************/
void SendData_Set_Command(uint8_t hdata)

{
	    outputBuf[0]='T'; //4D
		outputBuf[1]='K'; //58
		outputBuf[2]='C'; //"C" ->command
		outputBuf[3]=hdata; //53	//
		
		transferSize=4;
		if(transferSize)
		{
			while(transOngoingFlag);
			transOngoingFlag=1;
			HAL_UART_Transmit_IT(&huart1,outputBuf,transferSize);
		}
	
}
/*********************************************************
 * 
 * Function Name:void SendData_Temp_Data(uint8_t tdata)
 * Function:send temperature value 
 * 
*********************************************************/
void SendData_Temp_Data(uint8_t tdata)
{

        outputBuf[0]='T'; //4D
		outputBuf[1]='K'; //58
		outputBuf[2]='M'; //"T"->temperature
		outputBuf[3]=tdata; //53	//
		
		transferSize=4;
		if(transferSize)
		{
			while(transOngoingFlag);
			transOngoingFlag=1;
			HAL_UART_Transmit_IT(&huart1,outputBuf,transferSize);
		}

}
/*********************************************************
 * 
 * Function Name:void SendData_Time_Data(uint8_t tdata)
 * Function:send to set  timer timing value
 * 
*********************************************************/
void SendData_Time_Data(uint8_t tdata)
{

        outputBuf[0]='T'; //4D
		outputBuf[1]='K'; //58
		outputBuf[2]='T'; //"T"->timer timing
		outputBuf[3]=tdata; //53	//
		
		transferSize=4;
		if(transferSize)
		{
			while(transOngoingFlag);
			transOngoingFlag=1;
			HAL_UART_Transmit_IT(&huart1,outputBuf,transferSize);
		}

}
/*********************************************************
 * 
 * Function Name:void SendData_Temp_Data(uint8_t tdata)
 * Function:send temperature value 
 * 
*********************************************************/
void SendData_Works_Time(uint8_t tdata,uint8_t tdata_2)
{

        outputBuf[0]='T'; //4D
		outputBuf[1]='O'; //"T"->temperature
		outputBuf[2]=tdata; //53	//
		outputBuf[3]=tdata_2;
		
		transferSize=4;
		if(transferSize)
		{
			while(transOngoingFlag);
			transOngoingFlag=1;
			HAL_UART_Transmit_IT(&huart1,outputBuf,transferSize);
		}

}


void SendData_Remaining_Time(uint8_t tdata,uint8_t tdata_2)
{

        outputBuf[0]='T'; //4D
		outputBuf[1]='R'; //"T"->temperature
		outputBuf[2]=tdata; //53	//
		outputBuf[3]=tdata_2;
		
		transferSize=4;
		if(transferSize)
		{
			while(transOngoingFlag);
			transOngoingFlag=1;
			HAL_UART_Transmit_IT(&huart1,outputBuf,transferSize);
		}

}



/********************************************************************************
	**
	*Function Name:void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
	*Function :UART callback function  for UART interrupt for receive data
	*Input Ref: structure UART_HandleTypeDef pointer
	*Return Ref:NO
	*
*******************************************************************************/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
     uint8_t total_value ;
	if(huart==&huart1) // Motor Board receive data (filter)
	{
		switch(state)
		{
		case 0:  //#0
			if(inputBuf[0]=='M' || inputBuf[0]==0x55)  {//hex :4D - "M" -fixed mainboard
                if(inputBuf[0]==0x55){

                    parse_buf[0] =inputBuf[0];
					run_t.rx_mb_answer_tag =1;

                }

				state=1; //=1

		    }
			else{
			state=0; //=1

			run_t.rx_mb_answer_tag=0;

			}
			break;
		case 1: //#1
			if(inputBuf[0]=='A' ||inputBuf[0]==0 || inputBuf[0]==0x01) //hex : 41 -'A'  -fixed master
			{
				if(inputBuf[0]!='A')parse_buf[1] =inputBuf[0];
                
				state=2; 
			}
			else{
				
				state=0;
				run_t.rx_mb_answer_tag=0;


				}
			break;
		case 2://#2
			if(inputBuf[0]=='D' || inputBuf[0]==0x1 )
			 {
				
				if(inputBuf[0]=='D')run_t.rx_mb_data_tag=PANEL_DATA; //receive data is single data
                else parse_buf[2] =inputBuf[0];
			    state=3;
			}
            else if(inputBuf[0]=='E'){
                run_t.rx_mb_data_tag=ORDER_DATA;
                state=3;
            }
			else{
				state=0;
				run_t.rx_mb_answer_tag=0;


				}
			break;
            
        case 3:

            if(run_t.rx_mb_data_tag==PANEL_DATA){
            
                 run_t.gReal_humtemp[0]=inputBuf[0]; //Humidity value 
                  state = 4; 
                
             }
			else if(run_t.rx_mb_data_tag==ORDER_DATA){

			   outputBuf[0]=  inputBuf[0];  
			state=0;
			run_t.decodeFlag=1;
			run_t.rx_mb_answer_tag=0;


			}
            else {

				parse_buf[3] =inputBuf[0];
				 state = 4; 
            }
			

         
		    
			 

         	

            
        break;
        
		case 4: //

		 if(run_t.rx_mb_data_tag==PANEL_DATA){
              run_t.gReal_humtemp[1]=inputBuf[0]; //temperature value
			
		     state=0;
		     run_t.decodeFlag=1;
			 run_t.rx_mb_answer_tag=0;
              
          }
         else {

			parse_buf[4] =inputBuf[0];
			state =5;

         }
		
		 
		 break;


		 case 5:
		  parse_buf[5] =inputBuf[0];
         

		      state=0;
			  run_t.decodeFlag=1;
		      run_t.rx_mb_data_tag=0;
			  run_t.rx_mb_data_tag=ANSWER_DATA;
          
              


		 

		 break;
           
       
		
		
		default:
			
		break;

		}
        __HAL_UART_CLEAR_OREFLAG(&huart1);
		HAL_UART_Receive_IT(&huart1,inputBuf,1);//UART receive data interrupt 1 byte
	}
}
void USART1_Cmd_Error_Handler(void)
{
   uint32_t temp;
   if(run_t.gTimer_usart_error >8){
	  	run_t.gTimer_usart_error=0;
	    
         
        __HAL_UART_CLEAR_OREFLAG(&huart1);
        temp = USART1->RDR;
		UART_Start_Receive_IT(&huart1,inputBuf,1);
       
		}
      
         
} 
        
   

/********************************************************************************
**
*Function Name:void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
*Function :UART callback function  for UART interrupt for transmit data
*Input Ref: structure UART_HandleTypeDef pointer
*Return Ref:NO
*
*******************************************************************************/
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart==&huart1)
	{
		transOngoingFlag=0; //UART Transmit interrupt flag =0 ,RUN
	}
	
}

