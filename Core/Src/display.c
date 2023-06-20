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

	hum1 =  run_t.gReal_humtemp[0]/10;  //Humidity 
	hum2 =  run_t.gReal_humtemp[0]%10;

	temp1 = run_t.gReal_humtemp[1]/10 ;  // temperature
	temp2 = run_t.gReal_humtemp[1]%10;

    if(run_t.set_temperature_flag==0){
	  TM1639_Write_2bit_TempData(temp1,temp2);
    }
	TM1639_Write_2bit_HumData(hum1,hum2);
	


}  


/**********************************************************************
*
*Functin Name: void Display_GMT(uint8_t hours,uint8_t minutes)
*Function : Timer of key be pressed handle
*Input Ref:  key of value
*Return Ref: NO
*
**********************************************************************/
void Display_GMT(uint8_t hours,uint8_t minutes)
{ 
    static uint8_t m,q;
	m = hours /10 ;
	run_t.hours_two_unit_bit =	hours%10; 
	run_t.minutes_one_decade_bit= minutes/10 ;
	q=  minutes%10;
	TM1639_Write_4Bit_Time(m,run_t.hours_two_unit_bit,run_t.minutes_one_decade_bit,q,0) ; //timer is default 12 hours "12:00"


}

/********************************************************************************
*
*Functin Name: void Display_Error_Digital(uint8_t errnumbers,uint8_t sel)
*Function : Timer of key be pressed handle
*Input Ref:  error digital 
*Return Ref: NO
*
********************************************************************************/
void Display_Error_Digital(uint8_t errnumbers,uint8_t sel)
{ 
    static uint8_t m,q;
	m = 0x0E;
	
	run_t.hours_two_unit_bit= 0x0d;
	
	run_t.minutes_one_decade_bit= errnumbers/10;
	q=errnumbers%10;
	TM1639_Write_4Bit_Time(m,run_t.hours_two_unit_bit,run_t.minutes_one_decade_bit,q,sel) ; //timer is default 12 hours "12:00"


}

/********************************************************************************
*
*Functin Name: static void TimeColon_Smg_Blink_Fun(void)
*Function : Timer of key be pressed handle
*Input Ref:  NO
*Return Ref: NO
*
********************************************************************************/
static void TimeColon_Smg_Blink_Fun(void)
{
	if(run_t.gTimer_colon < 2){
		  SmgBlink_Colon_Function(run_t.hours_two_unit_bit ,run_t.minutes_one_decade_bit,0);
	   }
	   else if(run_t.gTimer_colon >  1	&&	run_t.gTimer_colon < 3){
		   SmgBlink_Colon_Function(run_t.hours_two_unit_bit ,run_t.minutes_one_decade_bit,1);

	  }
	  else{
		 run_t.gTimer_colon =0;

	  }
}


void Display_TimeColon_Blink_Fun(void)
{

   if(run_t.gTimer_time_colon >80){ //10*20ms=300ms

	   run_t.gTimer_time_colon =0;
	   TimeColon_Smg_Blink_Fun();
	}
}


