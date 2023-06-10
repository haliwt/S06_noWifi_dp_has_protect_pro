#include "key.h"
#include "gpio.h"
#include "run.h"
#include "smg.h"
#include "cmd_link.h"
#include "display.h"
#include "single_mode.h"
#include "led.h"



key_types key_t;
/***********************************************************
*
*
*
*
*
***********************************************************/

uint8_t KEY_Scan(void)
{
  uint8_t  reval = 0;
  key_t.read = _KEY_ALL_OFF; //0xFF 

//    if(POWER_KEY_VALUE() ==1){
//
//        key_t.read &= ~0x01; // 0xFf & 0xfd =  0xFE
//	}
//    else 

	if(MODEL_KEY_VALUE() ==1 )
	{
		   key_t.read &= ~0x02; // 0xFf & 0xfd =  0xFD
	}
   if(DEC_KEY_VALUE()  ==1 ) //DEC_KEY_ID = 0x04
	{
		  key_t.read &= ~0x04; // 0xFf & 0xfB =  0xFB
	}
    else if(ADD_KEY_VALUE() ==1 ) //ADD_KEY_ID = 0x08
	{
		  key_t.read &= ~0x08; // 0x1f & 0xf7 =  0xF7
	 }
	 else if(FAN_KEY_VALUE()   ==1 ) //FAN_KEY_ID = 0x10
	{
		  key_t.read &= ~0x10; // 0xFf & 0xEF =  0xEF
	}
	else if(PLASMA_KEY_VALUE()   ==1 ) //PLASMA_KEY_ID = 0x20
	{
		  key_t.read &= ~0x20; // 0xFf & 0xDF =  0xDF
	}
	else if(DRY_KEY_VALUE()  ==1 ) //DRY_KEY_ID = 0x40
	{
		  key_t.read &= ~0x40; // 0xFf & 0xBF =  0xBF
	}
    else if(WIFI_KEY_VALUE() ==1 ) //WIFI_KEY_ID = 0x80
	{
		key_t.read &= ~0x80; // 0x1f & 0x7F =  0x7F
	}
	
	
	
   
    switch(key_t.state )
	{
		case start:
		{
			if(key_t.read != _KEY_ALL_OFF)
			{
				key_t.buffer   = key_t.read; //??:key.buffer = 0xFE  POWER KEY 
				key_t.state    = first;
				key_t.on_time  = 0;
				key_t.off_time = 0;
            
                
			}
			break;
		}
		case first:
		{
			if(key_t.read == key_t.buffer) //  short  key be down ->continunce be pressed key
			{
				if(++key_t.on_time>25 )// //10000  0.5us
				{
					//run_t.power_times++;
                    key_t.value = key_t.buffer^_KEY_ALL_OFF; // key.value = 0xFE ^ 0xFF = 0x01
					key_t.on_time = 0;                      //key .value = 0xEF ^ 0XFF = 0X10
                    key_t.state   = second;
                   
                    
				}
			  
			}
			else
			{
				key_t.state   = start;
			}
			break;
		}
		case second:
		{
			if(key_t.read == key_t.buffer) //long key key if be pressed down 
			{
				if(++key_t.on_time>95000)// 80000 long key be down
				{
				    key_t.value = key_t.value|0x90; //key.value = 0x01 | 0x80  =0x81  
					
					if(key_t.value == 0x80) key_t.value = 0x90;
                    key_t.on_time = 0;
					key_t.state   = finish;
	               
				}
					
			}
			else if(key_t.read == _KEY_ALL_OFF)  // loose hand 
			{
					if(++key_t.off_time> 0) //20//30 don't holding key dithering
					{
						key_t.value = key_t.buffer^_KEY_ALL_OFF; // key.value = 0x1E ^ 0x1f = 0x01
						
						key_t.state   = finish; // loose hand
					}
			}
		   
			break;
		}
		case finish:
		{
		
			reval = key_t.value; // is short time  TIMER_KEY = 0x01  2. long times TIMER_KEY = 0X81

			key_t.state   = end;
         
			break;
		}
		case end:
		{
			if(key_t.read == _KEY_ALL_OFF)
			{
				if(++key_t.off_time>0)//5//10//50 //100
				{
					key_t.state   = start;
                  
				}
			}
			break;
		}
		default:
		{
			key_t.state   = start;
         
			break;
		}
	}
	return  reval;


}


/************************************************************************
	*
	*Function Name: void Process_Key_Handler(uint8_t keylabel)
	*Function : key by pressed which is key numbers process 
	*Input Ref: key be pressed value 
	*Return Ref:No
	*
************************************************************************/
void Process_Key_Handler(uint8_t keylabel)
{
   static uint8_t m,n,q,power_on_off_flag;
  
    switch(keylabel){

      case POWER_KEY_ID:
	 
           power_on_off_flag = power_on_off_flag ^ 0x01;
	       if(power_on_off_flag ==1){
 			run_t.gTimer_set_temp_times=0; //conflict with send temperatur value
 		
            run_t.gRunCommand_label =RUN_POWER_ON;
              
		 }
		 else{

		    SendData_PowerOff(0);
            HAL_Delay(200);
		    run_t.gRunCommand_label =RUN_POWER_OFF;
	        run_t.power_on_recoder_times++ ;
		   }
	  	 
	   run_t.keyvalue = 0xff;

	  break;

	  case LINK_WIFI_KEY_ID:
	  	if(run_t.gPower_On ==RUN_POWER_ON){

		    SendData_Set_Wifi(0x01);
			HAL_Delay(50);
	
			run_t.gTimer_set_temp_times=0; //conflict with send temperatur value 
         
		
			run_t.ai_model_flag =0;
			run_t.gTimer_wifi_connect_counter=0;
	       
		 }
		 run_t.keyvalue = 0xff;
	  break;

	  case MODEL_KEY_ID://model_key:
		if(run_t.gPower_On ==RUN_POWER_ON){
			run_t.temp_set_timer_timing_flag=1;
			
			SendData_Buzzer();//single_buzzer_fun();
			
			run_t.gTimer_key_timing=0;
			
				
		 }
	   run_t.keyvalue = 0xff;

	  break;

	  case ADD_KEY_ID://add_key:
	  	 if(run_t.gPower_On ==RUN_POWER_ON){
			SendData_Buzzer();//single_buzzer_fun();

		  switch(run_t.temp_set_timer_timing_flag){

		    case 0:  //set temperature value 
                run_t.wifi_set_temperature ++;
	            if(run_t.wifi_set_temperature < 20){
				    run_t.wifi_set_temperature=20;
				}
				
				if(run_t.wifi_set_temperature > 40)run_t.wifi_set_temperature= 20;
				
			    m =  run_t.wifi_set_temperature / 10 ;
				n =  run_t.wifi_set_temperature % 10; //
   
                TM1639_Write_2bit_SetUp_TempData(m,n,0);
				
			
				   run_t.set_temperature_flag=1;
				   run_t.gTimer_key_temp_timing=0;
				   
			
			break;

			case 1: //set timer timing value 
				 run_t.gTimer_key_timing =0;
					 run_t.dispTime_hours ++;
		             if(run_t.dispTime_hours > 24){
						run_t.dispTime_hours=0;
					}
						
							
					m = run_t.dispTime_hours /10 ;
					run_t.hours_two_bit =run_t.dispTime_hours  %10; //n = run_t.dispTime_hours  %10;
					
				    run_t.minutes_one_bit=0;//p=0;
					q=0;
				

					 TM1639_Write_4Bit_Time(m,run_t.hours_two_bit, run_t.minutes_one_bit,q,0) ; //timer is default 12 hours "12:00" 
				//    HAL_Delay(100);
                
				
	  	    }
	  	 }
	  	 run_t.keyvalue = 0xff;
	  break;

	  case DEC_KEY_ID://dec_key:
	   if(run_t.gPower_On ==RUN_POWER_ON){
			SendData_Buzzer();//single_buzzer_fun();
		 switch(run_t.temp_set_timer_timing_flag){

		 	case 0: //set temperature value
	    
			//setup temperature of value,minimum 20,maximum 40
			run_t.wifi_set_temperature--;
			if(run_t.wifi_set_temperature<20) run_t.wifi_set_temperature=40;
	        else if(run_t.wifi_set_temperature >40)run_t.wifi_set_temperature=40;

	        m =  run_t.wifi_set_temperature / 10 ;
			n =  run_t.wifi_set_temperature % 10; //

			
			 TM1639_Write_2bit_SetUp_TempData(m,n,0);
			// HAL_Delay(100);
		      run_t.set_temperature_flag=1;
			  run_t.gTimer_key_temp_timing=0;
			 
	    	
		   break;

		   case 1: //set timer timing value
	    	
			
				run_t.gTimer_key_timing =0;
				run_t.dispTime_hours --;
				if(run_t.dispTime_hours < 0){
					run_t.dispTime_hours=24 ;
				    run_t.dispTime_minutes =0;
					 // run_t.dispTime_minutes = run_t.dispTime_minutes - 30;
					
				}

                  m = run_t.dispTime_hours /10 ;
				  run_t.hours_two_bit = run_t.dispTime_hours  %10;//n = run_t.dispTime_hours  %10;

				  run_t.minutes_one_bit=0;//p = 0;
				  q = 0;

				TM1639_Write_4Bit_Time(m,run_t.hours_two_bit,run_t.minutes_one_bit,q,0) ; //timer is default 12 hours "12:00"    
              //  HAL_Delay(100);
	
			
		  
		  break;
	   	  }
	   	}
		 run_t.keyvalue = 0xff;
	  break;

	   case DRY_KEY_ID://0x02: //CIN6  ->DRY KEY 
          if(run_t.gPower_On ==RUN_POWER_ON){
		
			  if(run_t.gDry== 1){
				    run_t.gDry =0;
					SendData_Set_Command(DRY_OFF);
               }
               else{
                    run_t.gDry =1;
					SendData_Set_Command(DRY_ON);
                 }  
			   }
			keylabel= 0xff;	
         break;

		 case PLASMA_KEY_ID: //0x04: //CIN5  -> plasma ->STERILIZATION KEY 
             if(run_t.gPower_On ==RUN_POWER_ON){
			
              
			   if(run_t.gPlasma ==1){  //turun off kill 
			   	
			       run_t.gPlasma = 0;
				   SendData_Set_Command(PLASMA_OFF);
			   	}  
                else{
                   run_t.gPlasma = 1;
				   SendData_Set_Command(PLASMA_ON);
				}
				   
		       
			 }
           run_t.keyvalue = 0xff;
        break;

		 case ULTRASONIC_KEY_ID: //0x08: //Fan KEY 
              if(run_t.gPower_On ==RUN_POWER_ON){
                   
                if(run_t.gUltrasonic==0){
 					run_t.gUltrasonic =1; //tur ON
 					SendData_Set_Command(ULTRASONIC_ON);
						HAL_Delay(10);
			     }
                else{
               
					run_t.gUltrasonic =0;
					SendData_Set_Command(ULTRASONIC_OFF);
				HAL_Delay(10);
                    
                 }
				  
				 
			}
			 run_t.keyvalue = 0xff;
		 break;


	  default:
          
	  break;

	}
	//
	

}
/****************************************************************
	*
	*Function Name :void SetTimer_Temperature_Number_Blink(void)
	*Function : set timer timing how many ?
	*Input Parameters :NO
	*Retrurn Parameter :NO
	*
*****************************************************************/
void SetTimer_Temperature_Number_Blink(void)
{

    static uint8_t m,n,p,q,counter_times;
    static uint8_t timing_flag,set_timer_flag,set_temp_flag;
	
	//set timer timing value 
	switch(run_t.temp_set_timer_timing_flag){

	case TIMER_TIMING:
	if(run_t.gTimer_key_timing > 4  && set_timer_flag ==0 && run_t.gPower_On==RUN_POWER_ON){
				
		set_timer_flag++;
		run_t.gTimer_key_timing =0;
        run_t.gTimer_Counter=0;
		if(run_t.dispTime_hours ==0 ){
		  
			set_timer_flag=0;
			timing_flag=0;
			run_t.set_timer_special_value = timing_donot;

			run_t.temp_set_timer_timing_flag=0;

		
			run_t.send_app_timer_minutes_one = 0;
		    run_t.send_app_timer_minutes_two = 0;
			
		//	SendData_Time_Data(run_t.dispTime_hours);
		//	HAL_Delay(100);
            
		 	//SendData_Remaining_Time(run_t.send_app_timer_minutes_one, run_t.send_app_timer_minutes_two);
			//HAL_Delay(200);
			
					
			//SendData_Works_Time(run_t.send_app_wokes_minutes_one ,run_t.send_app_wokes_minutes_two);
			//HAL_Delay(100);
	        run_t.dispTime_hours = run_t.works_dispTime_hours;
			run_t.dispTime_minutes = run_t.works_dispTime_minutes;
		//	Display_GMT(run_t.dispTime_hours,run_t.dispTime_minutes);
			

		}
		else{
			
			run_t.set_timer_special_value = timing_success;
			run_t.gTimer_smg_timing =0; //couter time of smg blink timing 
            run_t.gTimer_Counter=0;
			
		}
	
	}

	//set timer timing  smg blink timing 
	if(run_t.set_timer_special_value == timing_success  && run_t.gPower_On==RUN_POWER_ON){
		   
		if(run_t.gTimer_smg_timing < 13){

			m=run_t.dispTime_hours  /10%10;
			run_t.hours_two_bit=run_t.dispTime_hours  %10;//n=run_t.dispTime_hours  %10;
			run_t.minutes_one_bit=0;//p =0;
			q=  0;
			TM1639_Write_4Bit_Time(m,run_t.hours_two_bit,run_t.minutes_one_bit,q,0) ;

		}
		else if(run_t.gTimer_smg_timing > 12 && run_t.gTimer_smg_timing < 27){
			run_t.hours_two_bit=0;
			run_t.minutes_one_bit = 0;
		  TM1639_Write_4Bit_Time(0,run_t.hours_two_bit,run_t.minutes_one_bit,0,1) ;
		}
		else{
			run_t.gTimer_smg_timing=0;

			timing_flag ++;
		}

		if(timing_flag > 3){
			set_timer_flag=0;
			timing_flag=0;
		    
		   	run_t.set_timer_special_value=0 ;
			run_t.dispTime_minutes=0;
			run_t.temp_set_timer_timing_flag=0;
		
			
			run_t.define_initialization_timer_time_hours = run_t.dispTime_hours ;//* 60 
			run_t.send_app_timer_total_minutes_data = run_t.define_initialization_timer_time_hours*60;
			
			run_t.gTimer_Counter=0;
		
			run_t.hours_two_bit=run_t.dispTime_hours  %10;
			run_t.minutes_one_bit = p;
		
			TM1639_Write_4Bit_Time(m,run_t.hours_two_bit,run_t.minutes_one_bit,q,0) ;
		}
	   }
	
	  break;

	}

	switch(run_t.set_temperature_flag){

	  case TEMPERATURE_BE_SETUP:
	  if(run_t.gTimer_key_temp_timing > 4 && run_t.set_temperature_special_value ==0 && run_t.gPower_On==RUN_POWER_ON){
			set_temp_flag++;
			
			run_t.set_temperature_special_value =1;
			run_t.gTimer_set_temp_times =0; //couter time of smg blink timing 

		 }
	  if(run_t.set_temperature_special_value ==1 && run_t.gPower_On==RUN_POWER_ON){
	  	
	  	
		  if(run_t.gTimer_set_temp_times < 15 ){ // 4
		        TM1639_Write_2bit_SetUp_TempData(0,0,1);
          }
		  else if(run_t.gTimer_set_temp_times > 14 && run_t.gTimer_set_temp_times < 29){
		  	
			  m =  run_t.wifi_set_temperature / 10 ;
			  n =  run_t.wifi_set_temperature % 10; //
			  TM1639_Write_2bit_SetUp_TempData(m,n,0);

		  }
		  else{
		  	 run_t.gTimer_set_temp_times=0;
             counter_times++ ;  

		  }


           if(counter_times > 3){
			 run_t.set_temperature_special_value=0;
			 set_temp_flag=0;
		     counter_times=0;
		      run_t.set_temperature_flag =0;
			  run_t.temperature_set_flag =1;
			  
			  run_t.gTimer_temp_delay = 70; //at once shut down ptc  funciton
			  m =  run_t.wifi_set_temperature / 10 ;
			  n =  run_t.wifi_set_temperature % 10; //
			  TM1639_Write_2bit_SetUp_TempData(m,n,0);
	         // SendData_Temp_Data(run_t.wifi_set_temperature);
              // HAL_Delay(50);
	       }
	  }
	  break;
		}

}

/**********************************************************
***********************************************************/
void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
   #if 1
  
   
   switch(GPIO_Pin){

     HAL_Delay(40);
     case POWER_KEY_Pin:
	   
	 	if(POWER_KEY_VALUE()  ==KEY_DOWN && run_t.power_times==1){
			

		   // power_on_off_flag = power_on_off_flag^ 0x01;
            if(run_t.gPower_On ==RUN_POWER_OFF){
				
                run_t.gRunCommand_label = RUN_POWER_ON;
				SendData_PowerOff(1);
		   }
           else{
		   	run_t.gRunCommand_label = RUN_POWER_OFF;
		   // run_t.gPower_On =RUN_POWER_OFF;
			//run_t.gRunCommand_label =POWER_OFF_PROCESS;
			SendData_PowerOff(0);
			Power_Off_Fun();
		     
				  
            
		
            }
		

		}

     break;

    }
 #endif 
}

    #if 0

	 case KEY_ADD_Pin:
	 

	 	if(ADD_KEY_VALUE() ==KEY_DOWN && run_t.power_times==1 ){
          run_t.recoder_start_conuter_flag=0;
		run_t.gKey_command_tag = ADD_KEY_ITEM;


		}

	 break;

	 case KEY_DEC_Pin:
		
		if(DEC_KEY_VALUE() ==KEY_DOWN && run_t.power_times==1){
        
           run_t.recoder_start_conuter_flag=0;
		 run_t.gKey_command_tag = DEC_KEY_ITEM;


		}
	 

	 break;


   #endif 



