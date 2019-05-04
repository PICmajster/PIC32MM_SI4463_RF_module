/*
    File Name:        :  dogm162.c

    Device            :  PIC32MM0256GPM048
    Compiler          :  XC32 2.05
    MPLAB             :  MPLAB X 4.15
    Created by        :  http://strefapic.blogspot.com
*/

#include "xc.h" /* wykrywa rodzaj procka i includuje odpowiedni plik*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h> /*deklaracje uint8_t itp*/

#include "delay.h"  /*biblioteka dajaca dostep do delay-i.*/
#include "dogm162.h"/*obsluga wyswietlacza LCD*/

/*definicje funkcji*/
void lcd_WriteByte(unsigned char bajt)
{
	/*ustaw linie EN, przed wysylka danych*/
    
    E = 1;
  	/*wyslanie 4 najstarszych bitow danych*/
	if(bajt & 0x80)	DB7	= 1; else DB7 = 0;
	if(bajt & 0x40)	DB6	= 1; else DB6 = 0;
	if(bajt & 0x20)	DB5	= 1; else DB5 = 0;
	if(bajt & 0x10)	DB4	= 1; else DB4 = 0;
	delayUs(1);
	/*potwierdzenie wyslania danych (opadajacym zboczem EN)*/
	E = 0;
		
	/*ustawienie EN*/
	delayUs(1);
    E = 1;
  	/*wyslanie 4 najmlodszych bitow danych*/	
	if(bajt & 0x08)	DB7	= 1; else DB7 = 0;
	if(bajt & 0x04)	DB6	= 1; else DB6 = 0;
	if(bajt & 0x02)	DB5	= 1; else DB5 = 0;
	if(bajt & 0x01)	DB4	= 1; else DB4 = 0;
	delayUs(1);
	/*potwierdz wysylke danych opadajacym zboczem EN*/
	E = 0;

	delayUs(16);
    
}	


void lcd_Initialize()
{
	/*ustawienie kierunku wyjsciowego linii podlaczonych do LCD*/
    TRIS_RS = 0;
	TRIS_E = 0;
	TRIS_DB7 = 0;
	TRIS_DB6 = 0;
	TRIS_DB5 = 0;
	TRIS_DB4 = 0;

	/*zerowanie linii*/
    RS = 0; /* 0 - wskazuje na rejestr rozkazow / 1 - wskazuje na rejestr danych*/
	E = 0;
	DB7 = 0;
	DB6 = 0;
	DB5 = 0;
	DB4 = 0;

  /*Start Inicjalizacji DOGM162 tryb 4-bity*/
    delayMs(40);    
  /*wysylamy instrukcje do rejestru rozkazow*/
  lcd_WriteByte(0x30);/*Function Set 8 bit DL=1 IS2/IS1=0*/
   delayUs(30);
  lcd_WriteByte(0x20);/*Function Set 4 bit DL = 0*/
   delayUs(30);
  lcd_WriteByte(0x29);/*Function Set 4 bit DL=1 IS2=0/IS1=1*/
   delayUs(30);
  lcd_WriteByte(0x14);/*Bias set*/
   delayUs(30);
  lcd_WriteByte(0x78);/*Contrast set*/
  delayUs(30); 
  lcd_WriteByte(0x5D);/*Power/Icon control*/
  delayUs(30);
  lcd_WriteByte(0x6D);/*Follower control*/
  delayUs(30);
  lcd_WriteByte(0x0C);/*DISPLAY ON, CURSOR OFF , CURSOR BLINK OFF*/
  delayUs(30);
  lcd_WriteByte(0x01);/*Clear Display*/
  delayMs(2);
  lcd_WriteByte(0x28);/*Function Set 4 bit DL=1 IS2/IS1=0*/
  delayUs(30);
  lcd_WriteByte(0x06);/*Entry mode set, Cursor auto-increment*/
  delayUs(30);
   
RS = 1 ; /*przelacz na rejestr danych*/  
        
 /*Koniec inicjalizacji i ustawien wyswietlacza DOGM162*/      
}

void lcd_String(char *napis)
{
    while(*napis){
    lcd_WriteByte(*napis++);
    }
         
}

void lcd_Locate(uint8_t y, uint8_t x)
{
	uint8_t n ;
    /*y (wiersze) = 1 do 2*/
    /*x (kolumna) = 1 do 16*/
    /*ustal adres poczatku znaku w wierszu*/
	switch(y)
    {
        case 1: y = LCD_Line1 ;break;
        case 2: y = LCD_Line2 ;break;
    
    }
    /*ustal nowy adres pamieci DD RAM*/
	/*ustaw bajt do Set DDRAM adres*/
    /* x odejmujemy jeden aby przekonwertowac z 0-15 na 1-16 */
	n = 0b10000000 + y + (x-1) ;
	
	/*wyslij rozkaz ustawienia nowego adresu DD RAM*/
	RS = 0; /*stan niski na lini? RS, wybieramy rejestr instrukcji*/
	lcd_WriteByte(n);
    RS = 1;  /*przelacz na rejestr danych */ 
}
// wyslanie liczby dziesietnej
    void lcd_Integer(uint16_t val)
    {
    char bufor[17];
    sprintf(bufor,"%i",val);
    lcd_String(bufor);
    }
void lcd_Cls()
{
	RS = 0; /*przelacz na rejestr rozkazow*/
	lcd_WriteByte(1);
	RS = 1; /*przelacz na rejestr danych*/
	delayMs(1);
} 

void WpiszSwojeZnaki(void) {
    /*definicja wlasnych znaków maks 8 szt*/
    char znak1[]= {0,0,14,17,31,16,14,2}; /* definicja literki e z ogonkiem */
    char znak2[]= {0x0C,0x12,0x12,0x0C,0,0,0,0};/*definicja stC*/
    int i; 
    /* adresy poczatku definicji znaku to wielokrotnosc osmiu DEC(0,8,16,24,32,40,48,56)
     * ale uwaga wazne ! adresy kodowane sa na 6 mlodszych bitach dwa najstarsze bity
     * to zawsze  01 (01AAAAAA-gdzie A adres).Uwzgledniajac wartosc calego bajtu
     * adresy poczatku beda wygladal tak HEX(0x40,0x48,0x50,0x58,0x60,0x68,0x70,0x78)
     * Aby wpisac do pamieci wyswietlacza zdefiniowany znak nalezy najpierw wyslac 
     * do rejestru rozkazów (RS na 0) adres poczatku definicji znaku 
     * a w drugim kroku przesylamy dane (RS=1) 8 x bajt (tablica) definjujace obraz znaku*/
    
    RS = 0 ;/*stan niski na linii RS, wybieramy rejestr instrukcji*/
     /*wysylamy instrukcje do rejestru rozkazow (ustaw adres poczatkowy w CGRAM 
      na nasz znak w tym przypadku znak na pozycji drugiej) */
    lcd_WriteByte(0x48);/*wysylamy instrukcje do rejestru rozkazow 
     (ustaw adres poczatkowy w CGRAM na nasz znak w tym przypadku znak na pozycji drugiej) */
    
    RS = 1 ;/*stan wysoki na linii RS, wybieramy rejestr danych*/
    /*wysylamy 8 x bajt zdefiniowanego w tablicy znak1[] znaku*/
    for(i=0;i<=7;i++)
    {
    lcd_WriteByte(znak2[i]);
    }
   
    RS = 0 ;/*stan niski na lini RS, wybieramy rejestr instrukcji*/
    /*ustawiamy adres DDRAM na pierwszy znak w pierwszej linii, nie zapomnijmy
     o tym poniewaz inaczej zostaniemy w pamieci CGRAM*/
    lcd_WriteByte(0x80);
    RS = 1 ; /*stan wysoki na linii RS, wybieramy rejestr danych*/
} 