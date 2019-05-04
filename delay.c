/*
    File Name:        :  delay.c

    Device            :  PIC32MM0256GPM048
    Compiler          :  XC32 2.05
    MPLAB             :  MPLAB X 4.15
    Created by        :  http://strefapic.blogspot.com
*/

#include "xc.h" /* wykrywa rodzaj procka i includuje odpowiedni plik naglowkowy "pic32mm0256gpm048.h"*/
#include "delay.h"

 
/***********************************************************
 *   Delay function using the Count register 
 *   in coprocessor 0 in the MIPS core.
 */
  void __attribute__((used)) delayUs(unsigned int usec)
{
    unsigned int tWait, tStart;

    tWait=(SYSTEMCLOCK/2000000)*usec; 
    tStart=_mfc0(9,0);
    while((_mfc0(9,0)-tStart)<tWait);        // wait for the time to pass
}

  void __attribute__((used)) delayMs(unsigned int usec)
{
    unsigned int tWait, tStart;

    tWait=(SYSTEMCLOCK/2000)*usec; 
    tStart=_mfc0(9,0);
    while((_mfc0(9,0)-tStart)<tWait);        // wait for the time to pass
}