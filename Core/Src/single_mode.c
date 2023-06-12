#include "single_mode.h"
#include "run.h"
#include "smg.h"
#include "gpio.h"
#include "cmd_link.h"
#include "led.h"
#include "key.h"
#include "display.h"




void (*single_ai_fun)(uint8_t cmd);
void (*single_add_fun)(void);
void (*single_buzzer_fun)(void);
void (*sendAi_usart_fun)(uint8_t senddat);
void (*dispose_key)(uint8_t dsdat);
static void Setup_Timer_Times(void);

static void Timing_Handler(void);
static void RunLocal_Smg_Process(void);

static void DisplayPanel_DHT11_Value(void);
static void SetTemperature_Function(void);
static void Display_Works_Time_Fun(void);
static void Send_WorksTime_ToApp_DonotDisplay_Fun(void);



/******************************************************************************
*
*Function Name:static void Setup_Timer_Times(void)
*Funcion:display setup timer times  //__asm("NOP");//等待1个指令周期，系统主频24M
*Iinput Ref:NO
*Return Ref:NO
*
******************************************************************************/
static void Timing_Handler(void)
{

  
	switch(run_t.timer_timing_define_flag){

	case timing_success:
	   if(run_t.gTimer_Counter > 59){
	    run_t.gTimer_Counter =0;
		run_t.dispTime_minutes -- ;
		run_t.send_app_timer_total_minutes_data--;
	    if(run_t.dispTime_minutes <  0 ){
			 
		   run_t.dispTime_hours -- ;
		   run_t.dispTime_minutes =59;
         }

		
		
		 if(run_t.dispTime_hours < 0 ){
		 
				
				run_t.gTimer_Counter = 57 ;

            
				run_t.dispTime_hours=0;
				
				run_t.dispTime_minutes=0;

			  
	
			}
		    
        }
      
  
	break;


	case timing_power_off:
        
       
		
		SendData_PowerOff(0);
		HAL_Delay(200);
		
	  run_t.power_on_recoder_times++; //this is data must be change if not don't "breath led"
	  run_t.gRunCommand_label = RUN_POWER_OFF;//POWER_OFF_PROCESS; //POWER_OFF_PROCESS ;
	  run_t.timer_timing_define_flag = 0xff;

	break;


	case timing_donot:

	     Display_Works_Time_Fun();
	     

	break;

	

	}
    
   


	
}
   
/******************************************************************************
*
*Function Name:void Single_RunCmd(void)
*Funcion: handle of receive by usart data
*
*
******************************************************************************/
static void DisplayPanel_DHT11_Value(void)
{
  if(run_t.first_power_on_times==1){
      run_t.first_power_on_times++;
       Display_DHT11_Value();
  
  }
  if(run_t.gTimer_display_dht11 > 10 && run_t.set_temperature_flag==0){
	    run_t.gTimer_display_dht11=0;
       	Display_DHT11_Value();
     
	}
}

/******************************************************************************
	*
	*Function Name:void RunPocess_Command_Handler(void)
	*Funcion: display pannel run of process 
	*Input Ref: NO
	*Return Ref:NO
	*
******************************************************************************/
void RunPocess_Command_Handler(void)
{
   static uint8_t power_off_set_flag;
   switch(run_t.gRunCommand_label){

      case RUN_POWER_ON:
	  	    run_t.step_run_power_off_tag=0;
           
           switch(run_t.step_run_power_on_tag){

			case 0:
           
			
           // SendData_PowerOff(1);


			if(run_t.power_on_send_to_mb_times > 9){
            
            	run_t.step_run_power_on_tag=1;
            }
			else if(run_t.power_on_send_to_mb_times< 10 && run_t.step_run_power_on_tag==0){
				run_t.power_on_send_to_mb_times++;
              SendData_PowerOff(1);
			  HAL_Delay(100);


			}
           

			break;

			case 1:
            run_t.step_run_power_off_tag=0;
            run_t.power_on_send_to_mb_times=36;
			Power_On_Fun();
			run_t.gRunCommand_label= UPDATE_DATA;

            break;

            }
	  break;

	  case RUN_POWER_OFF://2
	  	 run_t.step_run_power_on_tag=0;
		 switch(run_t.step_run_power_off_tag){

			case 0:

		    run_t.ptc_warning =0;
		    run_t.fan_warning =0;
			
            power_off_set_flag=0;
		   
		  if(run_t.power_off_send_to_mb_times >9){
            
                run_t.step_run_power_off_tag=1;
            
            
           }
		   else if(run_t.power_off_send_to_mb_times< 10 ){
				run_t.power_off_send_to_mb_times++;

				
	           SendData_PowerOff(0);
	           HAL_Delay(100);
		   }
           
            
         
	      	break;

			case 1:
            run_t.step_run_power_on_tag=0;
			run_t.power_off_send_to_mb_times= 20;
			 run_t.gPower_On=RUN_POWER_OFF;
			
		     Power_Off_Fun();

		  
		    run_t.gRunCommand_label =POWER_OFF_PROCESS;
            break;
        }
		break;
	  break;

	  case UPDATE_DATA: //3

	   
       
       if(run_t.step_run_power_off_tag==1){
           run_t.step_run_power_off_tag=0;
         run_t.gRunCommand_label =POWER_OFF_PROCESS;
         Power_Off_Fun();
       
       }else{
          if(POWER_KEY_VALUE() == KEY_UP){

		   RunLocal_Smg_Process();
	     
		   Timing_Handler();
		  
	       SetTemperature_Function(); 
		   
	   	   SetTimer_Temperature_Number_Blink();
	      
	       Display_TimeColon_Blink_Fun();

	   }
       
       
       }
	   

	  break;

	  case POWER_OFF_PROCESS://4

	   if(run_t.gPower_On ==RUN_POWER_OFF  && POWER_KEY_VALUE() == KEY_UP){
	   	        if(power_off_set_flag==0){
					power_off_set_flag++;
                   Power_Off_Fun();
	   	        }
              
				Breath_Led();
		 
		
       }

	  break;

  }
}
/*******************************************************
	*
	*Function Name: static void RunLocal_Smg_Process(void)
	*Function : display pannel display conetent
	*
	*
	*
*******************************************************/
static void RunLocal_Smg_Process(void)
{

     
	 Panel_Led_OnOff_Function() ;//Lcd_PowerOn_Fun();
	
	 DisplayPanel_DHT11_Value();


}
/*******************************************************
	*
	*Function Name: static void RunLocal_Smg_Process(void)
	*Function : display pannel display conetent
	*
	*
	*
*******************************************************/
static void SetTemperature_Function(void)
{
	 if(run_t.temperature_set_flag ==1 && run_t.gTimer_temp_delay >60){
               run_t.gTimer_temp_delay =0;
		 
		  
		  if(run_t.wifi_set_temperature <= run_t.gReal_humtemp[1] || run_t.gReal_humtemp[1] >39){//envirment temperature
	  
				run_t.gDry = 0;

		        SendData_Set_Command(DRY_OFF_NO_BUZZER);//PTC turn off
			    
			    
                
		  }
		  else if((run_t.wifi_set_temperature -3) > run_t.gReal_humtemp[1] ||  run_t.gReal_humtemp[1] < 37){
	  
		     run_t.gDry = 1;
	         SendData_Set_Command(DRY_ON_NO_BUZZER); //PTC turn On
				 
		  }
	  
	    
	  }
      else{ //no define set up temperature value 
          if(run_t.gReal_humtemp[1] >39 && run_t.gTimer_temp_delay >119){//envirment temperature
	            run_t.gTimer_temp_delay =0;
				run_t.gDry = 0;
                run_t.auto_model_shut_off_ptc_flag =1;
			    SendData_Set_Command(DRY_OFF_NO_BUZZER);

             }
             
             if(run_t.gReal_humtemp[1] < 36 && run_t.auto_model_shut_off_ptc_flag ==1 &&  run_t.gTimer_temp_delay >119){
                  run_t.gTimer_temp_delay =0;
                  run_t.gDry = 1;
	              SendData_Set_Command(DRY_ON_NO_BUZZER); //PTC turn On
             
             
             }
			    
                
		  }



}


static void Display_Works_Time_Fun(void)
{

     if(run_t.gTimes_time_seconds > 59 ){
            run_t.gTimes_time_seconds=0;
            run_t.send_works_times_to_app=1;
			run_t.works_dispTime_minutes++; //1 minute 
			run_t.send_app_wokes_total_minutes_data++;
            run_t.send_app_wokes_minutes_two++;
			if(run_t.works_dispTime_minutes> 59){ //1 hour
			run_t.works_dispTime_minutes=0;
			run_t.works_dispTime_hours++;
			if(run_t.works_dispTime_hours > 24){
			run_t.works_dispTime_hours =0;
			}
        
            if(run_t.send_app_wokes_total_minutes_data >255){
               run_t.send_app_wokes_minutes_one++;
               run_t.send_app_wokes_minutes_two=0;
               run_t.send_app_wokes_total_minutes_data=0;
            }
		
    
          
            }

		Display_GMT(run_t.works_dispTime_hours,run_t.works_dispTime_minutes);
	  
        }

	
     while(run_t.send_works_times_to_app==1){
            run_t.send_works_times_to_app=0;
        SendData_Works_Time(run_t.send_app_wokes_minutes_one ,run_t.send_app_wokes_minutes_two);
        }

}

static void Send_WorksTime_ToApp_DonotDisplay_Fun(void)
{
//send to APP works times every minute onece
   if(run_t.gTimes_time_seconds > 59 && run_t.timer_timing_define_flag ==timing_success && run_t.temp_set_timer_timing_flag ==0){
		   run_t.gTimes_time_seconds=0;
		   run_t.send_works_times_to_app=1;
		   run_t.works_dispTime_minutes++; //1 minute 
		   run_t.send_app_wokes_total_minutes_data++;
		   run_t.send_app_wokes_minutes_two++;
		   if(run_t.works_dispTime_minutes> 59){ //1 hour
		   run_t.works_dispTime_minutes=0;
		   run_t.works_dispTime_hours++;
		   if(run_t.works_dispTime_hours > 24){
		   run_t.works_dispTime_hours =0;
		   }
	   
		   if(run_t.send_app_wokes_total_minutes_data >255){
			  run_t.send_app_wokes_minutes_one++;
			  run_t.send_app_wokes_minutes_two=0;
			  run_t.send_app_wokes_total_minutes_data=0;
		   }
	   
   
		 
		   }
	 
	   }
	while(run_t.send_works_times_to_app==1){
		   run_t.send_works_times_to_app=0;
	   SendData_Works_Time(run_t.send_app_wokes_minutes_one ,run_t.send_app_wokes_minutes_two);
	   }
}
/****************************************************************
 * 
 * Function Name:
 * Function :function of pointer 
 * 
 * 
****************************************************************/
void Single_Add_RunCmd(void(*addHandler)(void))
{
    single_add_fun = addHandler;   

}

void Single_SendBuzzer_RunCmd(void(*buzzerHandler)(void))
{
	single_buzzer_fun = buzzerHandler;

}
void Single_SendAi_Usart_RunCmd(void(*sendaiHandler)(uint8_t seddat))
{
    sendAi_usart_fun = sendaiHandler;

}

void SplitDispose_Key_RunCmd(void(*keyHandler)(uint8_t dat))
{

  dispose_key = keyHandler;

}


