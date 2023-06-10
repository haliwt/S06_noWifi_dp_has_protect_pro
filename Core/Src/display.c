#include "display.h"
#include "smg.h"
#include "cmd_link.h"
#include "run.h"
#include "key.h"
#include "led.h"
#include "single_mode.h"



static void TimeColon_Smg_Blink_Fun(void);


/**********************************************************************
*
*Functin Name: void Display_DHT11_Value(void)
*Function : Timer of key be pressed handle
*Input Ref:  key of value
*Return Ref: NO
*
**********************************************************************/
void Display_DHT11_Value(void)
{
    
  static uint8_t hum1,hum2; 
  static uint8_t temp1,temp2;

	hum1 =  run_t.gReal_humtemp[0]/10 %10;  //Humidity 
	hum2 =  run_t.gReal_humtemp[0]%10;

	temp1 = run_t.gReal_humtemp[1]/10 %10;  // temperature
	temp2 = run_t.gReal_humtemp[1]%10;

    if(run_t.set_temperature_flag==0){
	     TM1639_Write_2bit_TempData(temp1,temp2);
    }
	TM1639_Write_2bit_HumData(hum1,hum2);


}  


/**********************************************************************
*
*Functin Name: void DisplayTiming_KEY_Add_Subtract_Fun(void)
*Function : Timer of key be pressed handle
*Input Ref:  key of value
*Return Ref: NO
*
**********************************************************************/
void DisplayTiming_KEY_Add_Subtract_Fun(void)
{
      
	  static uint8_t m,n,p,q;

	if(run_t.gPower_On==RUN_POWER_ON){
    
		p=run_t.dispTime_hours  /10%10;
		run_t.hours_two_bit =run_t.dispTime_hours  %10;//q=run_t.dispTime_hours  %10;  //
		run_t.minutes_one_bit= run_t.dispTime_minutes  /10%10;//m = run_t.dispTime_minutes  /10%10;
		n=	run_t.dispTime_minutes %10;

		TM1639_Write_4Bit_Time(p,run_t.hours_two_bit,run_t.minutes_one_bit,n,0) ; // timer   mode  "H0: xx"


		run_t.add_key_times =0;
		run_t.dec_key_times =0;
		
	}
 }
void Display_GMT(uint8_t hours,uint8_t minutes)
{ 
    static uint8_t m,n,p,q;
	m = hours /10 %10;
	run_t.hours_two_bit =	hours%10; 
	run_t.minutes_one_bit = minutes/10 %10;
	q=  minutes%10;
	TM1639_Write_4Bit_Time(m,run_t.hours_two_bit,run_t.minutes_one_bit,q,0) ; //timer is default 12 hours "12:00"


}



static void TimeColon_Smg_Blink_Fun(void)
{
	if(run_t.gTimer_colon < 2){
		  SmgBlink_Colon_Function(run_t.hours_two_bit ,run_t.minutes_one_bit,0);
	   }
	   else if(run_t.gTimer_colon >  1	&&	run_t.gTimer_colon < 3){
		   SmgBlink_Colon_Function(run_t.hours_two_bit ,run_t.minutes_one_bit,1);

	  }
	  else{
		 run_t.gTimer_colon =0;

	  }
}


void Display_TimeColon_Blink_Fun(void)
{
    static uint8_t  continue_flag=0, continue_2_flag=0;


 
   if(run_t.gTimer_time_colon >27){ //10*20ms=300ms

	   run_t.gTimer_time_colon =0;

	if(FAN_KEY_VALUE()  !=1 && PLASMA_KEY_VALUE()  !=1 && DRY_KEY_VALUE()  !=1 && WIFI_KEY_VALUE()!=1){

          continue_flag = 1;
	}
	

	
	if(POWER_KEY_VALUE() !=1 && MODEL_KEY_VALUE()!=1 && DEC_KEY_VALUE() !=1 && ADD_KEY_VALUE()!=1){

	                 continue_2_flag = 1;

	}
	

   if(run_t.set_temperature_flag==0 && run_t.set_temperature_special_value==0 && run_t.set_timer_special_value==0 && run_t.temp_set_timer_timing_flag==0\
   	    && continue_flag==1 && continue_2_flag==1){	
				 	
                     continue_2_flag = 0;
			         continue_flag=0;
					TimeColon_Smg_Blink_Fun();
					

	}
	}
	

}


