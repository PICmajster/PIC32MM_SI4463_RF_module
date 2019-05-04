/*****************************************************************************
  FileName:        main.c
  Processor:       PIC32MM0256GPM048
  Compiler:        XC32 ver 2.15
  IDE :            MPLABX-IDE 5.15
  Created by:      http://strefapic.blogspot.com
 ******************************************************************************/
/*----------------------------------------------------------------------------*/
/* LCD DOGM162  SPI                                                           */
/*----------------------------------------------------------------------------*/
// ** Sample application for PIC32MM **
//    
//	  Required connections for LCD DOGM162:
//    Signal LCD ---> MCU PIC32MM
//     RS                   --> RB10
//     E                    --> RB11
//     DB4                  --> RC3
//     DB5                  --> RC4
//     DB6                  --> RC5
//     DB7                  --> RD0
/*----------------------------------------------------------------------------*/
/* Transceiver RF SI4463                                                      */
/*----------------------------------------------------------------------------*/ 
//    Required connections for SI4463:
//    Signal SI4463 ---> MCU PIC32MM
//     MISO                 --> RA9
//     IN2                  --> RB9
//     MOSI                 --> RB3
//     SCK                  --> RB8
//     nSEL                 --> RC6 
//     SDN                  --> RC8
//------------------------------------
//     LED                  --> RB6

//******************************************************************************


#include "mcc_generated_files/system.h"
#include "delay.h"
#include "dogm162.h"
#include <stdint.h> /*uint8_t etc*/
#include "radio_config_Si4463.h"
#include "si4463.h"
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/tmr1.h"

#define LED_TOG PORTB ^= (1<<_PORTB_RB6_POSITION) /*zmienia stan bitu na przeciwny dla RB6*/
uint8_t data_buff[]="DATA";
uint8_t send_buff[64];
uint8_t receive_buff[64];
//uint16_t licznik = 100 ; //for TX
uint16_t licznik = 0 ; //for RX

int main(void)
{
    //initialize the device
    SYSTEM_Initialize();
    
    delayMs(50);
    lcd_Initialize();
    lcd_Locate(1,1);
    //lcd_String("Test SI4463 TX"); //for TX
    lcd_String("Test SI4463 RX"); //for RX
    
    SI4463_Init();
    SI4463_RX_Start( 0, 0 ); //for RX
    SI4463_Get_Interrupt(inter_buff); //get interrupt status and clear
   
    /*TX*/
//    uint8_t i=0, len;
//    len = sizeof(data_buff); //calculate the data length
//    send_buff[0] = len;  //enter the data length on the first position of the transmit buffer
//           
//    for (i=1 ; i < len; i++){
//       send_buff[i] =  data_buff[i-1]; //enter the data into the transmitting buffer starting from the second position
//    }
    /*end TX*/    
             
    while (1)
    {
      /*TX*/            
//        if(!Soft_Timer1) {
//              Soft_Timer1 = 8 ; /*Timer1 tick x Soft_Timer1 = 25ms x 8 = 200ms*/
//              SI4463_Send_Data( send_buff, sizeof(data_buff)+1 , 0 ) ; //send data
//              
//              }    
//                  
//      if(!IO_nIRQ_GetValue()){ //nIRQ Low ?
//        SI4463_Get_Interrupt(inter_buff); //get interrupt status and clear 
//        if(inter_buff[2] & 0x20) {//PACKET_SENT_PEND interruption ? , see API Documentation GET_INT_STATUS
//         if(!--licznik) TMR1_Stop();
//         lcd_Locate(2,1);
//         lcd_String("   ");
//         lcd_Locate(2,1);
//         lcd_Integer(licznik);
//         LED_TOG;
//                   
//        } 
//       }
//end TX
        
      /*RX*/
          if(!IO_nIRQ_GetValue()){ //nIRQ Low ?
            SI4463_Get_Interrupt(inter_buff); //get interrupt status and clear 
            if(inter_buff[2] & 0x10) {//PACKET RX PEND interruption ??, see API Documentation GET_INT_STATUS ??
                if(!(inter_buff[2] & 0x08)) {//CRC data interruption Error ?)
                
                SI4463_RX_FIFO ( receive_buff, SI4463_Get_Packet_Info()  ); //rewrite data from the RX FIFO (hardware buffer) to the receive_buff (user buffer) 
                licznik++;
                lcd_Locate(2,1);
                lcd_String("                ");
                lcd_Locate(2,1);
                lcd_Integer(licznik);
                lcd_String(" ");
                lcd_String(receive_buff);
                LED_TOG;                        
               
              }
            } 
           SI4463_Clear_RX_FIFO();
           SI4463_RX_Start( 0, 0 );  
           SI4463_Get_Interrupt(inter_buff); //get interrupt status and clear 
    }
// end RX   
 
    }

}
