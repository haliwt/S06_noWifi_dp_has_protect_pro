#include "run.h"
#include "smg.h"
#include "gpio.h"
#include "cmd_link.h"
#include "led.h"
#include "display.h"
#include "single_mode.h"

RUN_T run_t;

uint8_t power_on_off_flag;

void Power_Off(void);

static void Receive_Wifi_Cmd(uint8_t cmd);



/**********************************************************************************************************
**
*Function Name:static void notifyStatusToHost(uint8_t lightNum,uint8_t filterNum,uint8_t unionNum)
*Function : 
*Input Ref:lightNum--LED ,filterNum -filter number, unionNum - smart menu number
*Return Ref:NO
*
*********************************************************************************************************/
void Decode_Handler(void)
{
   if(run_t.decodeFlag ==1){
   run_t.decodeFlag =0;
   run_t.process_run_guarantee_flag =1;
   if(run_t.gPower_On ==RUN_POWER_OFF){
        run_t.step_run_power_on_tag=0;
        run_t.step_run_power_off_tag=1;
   }
   else{
       run_t.step_run_power_on_tag=1;
       run_t.step_run_power_off_tag=0;
   }
   Receive_MainBoard_Data_Handler(run_t.rx_mb_data_tag);


   }
  
}



/**********************************************************************
*
*Functin Name: void Display_DHT11_Value(void)
*Function : Timer of key be pressed handle
*Input Ref:  key of value
*Return Ref: NO
*
**********************************************************************/
void Power_Off(void)
{
    	 if(run_t.gPower_On ==RUN_POWER_OFF){

			Smg_AllOff();
            SMG_POWER_OFF()	;
			run_t.gPlasma=0;
			run_t.gDry=0;
			run_t.gUltrasonic =0;
			

				
		}
	
		
		
		
	      
}


/**********************************************************************
*
*Functin Name: void Receive_MainBoard_Data_Handler(run_t.single_data)
*Function : run order from main command 
*Input Ref:  run_t.single_dat is command 
*Return Ref: NO
*
**********************************************************************/
void Receive_MainBoard_Data_Handler(uint8_t cmd)
{
	static uint8_t m,n,p,q,i;
	static uint8_t hum1,hum2,temp1,temp2; 
	
    switch(cmd){

      case PANEL_DATA://0X02
	   
        if(run_t.gPower_On ==RUN_POWER_ON){
        hum1 =  run_t.gReal_humtemp[0]/10 %10;  //Humidity 
        hum2 =  run_t.gReal_humtemp[0]%10;
        
        temp1 = run_t.gReal_humtemp[1]/10 %10;  // temperature
        temp2 = run_t.gReal_humtemp[1]%10;

         //temperature 
          TM1639_Write_2bit_TempData(temp1,temp2);
	      TM1639_Write_2bit_HumData(hum1,hum2);
	      
        }


       cmd =0xff;
       run_t.rx_mb_data_tag = 0xff;
      break;

	  case ANSWER_DATA:
          
       if(parse_buf[5] == parse_buf[0]+parse_buf[1]+parse_buf[2]+parse_buf[3] +parse_buf[4]){
		 
	  	if(run_t.gPower_On ==RUN_POWER_ON){
            
	  	   run_t.step_run_power_on_tag=1;
           run_t.step_run_power_off_tag=0;
        }
		else{
		  run_t.step_run_power_off_tag=1;
             run_t.step_run_power_on_tag=0;
        }
        
       }
          
       

	  cmd =0xff;
      run_t.rx_mb_data_tag = 0xff;

	  break;

      

     

	}


}
/**********************************************************************
*
*Functin Name: void Receive_ManiBoard_Cmd(uint8_t cmd)
*Function :  wifi recieve data
*Input Ref:  receive wifi send order
*Return Ref: NO
*
**********************************************************************/
void Power_On_Fun(void)
{
                
   static uint8_t hour_decade,hour_unit,minutes_one,minutes_two;

  
	
		run_t.gPlasma=1;
		run_t.gDry =1;
		run_t.gBug =1;
	   	run_t.gUltrasonic =1;

  
    run_t.gPower_On=RUN_POWER_ON;

    run_t.time_led_flag=1;
	Power_ON_Led();

	
         
        

	  hour_decade=(run_t.dispTime_hours ) /10;
	  hour_unit=(run_t.dispTime_hours ) %10;
    
	  minutes_one = run_t.dispTime_minutes /10;
      minutes_two = run_t.dispTime_minutes %10;
	  
	   SMG_POWER_ON(); //WT.EDIT 2023.03.02
      


	  run_t.hours_two_bit = hour_unit;
	  run_t.minutes_one_bit =  minutes_one;
      
	 TM1639_Write_4Bit_Time(hour_decade,run_t.hours_two_bit,run_t.minutes_one_bit,minutes_two,0);
      Display_DHT11_Value();
    
}


/************************************************************************
	*
	*Function Name: static void Power_Off_Fun(void)
	*
	*
	*
	*
************************************************************************/
void Power_Off_Fun(void)
{
	
 
		run_t.gPlasma=0;
		run_t.gDry =0;
		run_t.gBug =0;
		
        run_t.gPower_On =RUN_POWER_OFF;
		run_t.gTimer_set_temp_times=0; //conflict with send temperatur value 
       Power_Off_Led_Off();

  
} 

