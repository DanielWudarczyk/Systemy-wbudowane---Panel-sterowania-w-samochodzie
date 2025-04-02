#include <REGX52.H>

//deklaracja funkcji zewnetrznych
extern void LcdInit();
extern void Lcd_DisplayString (char row, char column, char *string);
extern void Lcd_WriteControl (unsigned char LcdCommand);
extern void LcdKlimaOFF();
extern void LcdKlimaON();
extern void LcdTRC();

unsigned char data Var1, Var2, Var3;
unsigned char str[6];
bit widok=0, klima=0, tryb=0, enable=1;
unsigned char timer=0;
unsigned char poziom=3, trc=0, przytrzymanie_counter=0;

void Init(void)
{
		//inicjalizacja komunikacji UART
		P3_4 = 0;
		SCON = 0x50;
		T2CON = 0x30;
		TH2 = RCAP2H = 0xFF;
		TL2 = RCAP2L = 0xDC;
		ES=1;	
		EA=1;	
		TR2=1;	
		
		//ustawienia timera 0
		TMOD=0x10;
		ET0=1;
		TR0=1;
}

//Funkcja pomocnicza zamieniajaca char na string
void charToStr(unsigned char num, char *str) {
    char temp[6];
    char i = 0, j = 0;

    do {
        temp[i++] = (num % 10) + '0';
        num /= 10;
    } while (num > 0);

    while (i > 0) {
        str[j++] = temp[--i];
    }
    str[j] = '\0';
}

//Obsluga przerwan timera 0
void ISR_T0() interrupt 1
{
	if(enable==1)
	{
		if(P3_3==0 && przytrzymanie_counter<8)
		{
			przytrzymanie_counter++;
		}
		if(przytrzymanie_counter>7)   //przytrzymano wystarczajaco dlugo przycisk przy LCD
		{
			if(widok==0)   //byl widok klimatyzacji
			{
				widok=1;  
				LcdTRC();   //zmiana na widok kontroli trakcji
				
				//wyswietlenie aktualnego trybu kontroli trakcji
				if(trc==0)
				{
					Lcd_DisplayString (2,1,"tryb normalny");   //wyswietlenie trybu na ekranie
					Lcd_WriteControl(0x0C);   //usuniecie migajacego kursora
				}
				else if(trc==1)
				{
					Lcd_DisplayString (2,1,"tryb sportowy");
					Lcd_WriteControl(0x0C);
				}
				else
				{
					Lcd_DisplayString (2,1,"wylaczona    ");
					Lcd_WriteControl(0x0C);
				}
			}
			else   //zmiana na widok klimatyzacji
			{
				widok=0;
				if(klima==1)   //klimatyzacja wlaczona
				{
					//wyswietlenie ekranu klimatyzacji
					LcdKlimaON();
					charToStr(poziom,str);
					Lcd_DisplayString (4,14,str);
					Lcd_WriteControl(0x0C);
					if(tryb==0)
					{
						Lcd_DisplayString (3,6,"ogrzewania");
						Lcd_WriteControl(0x0C);
					}
					else
					{
						Lcd_DisplayString (3,6,"chlodzenia");
						Lcd_WriteControl(0x0C);
					}
				}
				else   //klimatyzacja wylaczona
				{
					LcdKlimaOFF();
				}
			}
			enable=0;   //przy dlugim wcisnieciu nie powtarzamy ciagle ustawien ekranu
		}
	}
	else if(P3_3==1)   //przycisk puszczony
	{
		przytrzymanie_counter=0;
		enable=1;
	}
}

//Obsluga przychodzacych sygnalow
void ISR_Serial(void) interrupt  4	
{
  if(TI == 1) {TI = 0;}
  if(RI == 1)
  {
    RI = 0;
		
		if(SBUF=='6')   //wlaczenie / wylaczenie klimatyzacji
		{
			if(klima==0)   //jesli byla wylaczona
			{
				klima=1;   //wlaczamy
				if(widok==0)   //jesli wlaczony widok klimatyzacji
				{
					//wyswietlenie parametrow na ekranie
					LcdKlimaON();
					charToStr(poziom,str);
					Lcd_DisplayString (4,14,str);
					Lcd_WriteControl(0x0C);
					if(tryb==0)
					{
						Lcd_DisplayString (3,6,"ogrzewania");
						Lcd_WriteControl(0x0C);
					}
					else
					{
						Lcd_DisplayString (3,6,"chlodzenia");
						Lcd_WriteControl(0x0C);
					}
				}
			}
			else   //klimatyzacja byla wlaczona
			{
				klima=0;   //wylaczamy
				if(widok==0)
				{
					LcdKlimaOFF();   //aktualizacja ekranu
				}
			}
		}
		else if(SBUF=='7')   //zmiana trybu klimatyzacji
		{
			if(tryb==0)
			{
				tryb=1;   //zmiana na tryb chlodzenia
				if(widok==0)
				{
					//aktualizacja na ekranie
					Lcd_DisplayString (3,6,"chlodzenia");
					Lcd_WriteControl(0x0C);
				}
			}
			else
			{
				tryb=0;   //tryb grzania
				if(widok==0)
				{
					//aktualizacja na ekranie
					Lcd_DisplayString (3,6,"ogrzewania");
					Lcd_WriteControl(0x0C);
				}
			}
		}
		else if(SBUF=='8')   //zmniejszenie nadmuchu
		{
			if(poziom>1)
			{
				poziom--;
				if(widok==0)
				{
					//aktualizacja na ekranie
					charToStr(poziom,str);
					Lcd_DisplayString (4,14,str);
					Lcd_WriteControl(0x0C);
				}
			}
		}
		else if(SBUF=='9')   //zwiekszenie nadmuchu
		{
			if(poziom<6)
			{
				poziom++;
				if(widok==0)
				{
					//aktualizacja na ekranie
					charToStr(poziom,str);
					Lcd_DisplayString (4,14,str);
					Lcd_WriteControl(0x0C);
				}
			}
		}
		//zmiany trybow kontroli trakcji
		else if(SBUF=='a')
		{
			trc=0;
			if(widok==1)   //jesli ustawiony widok kontroli trakcji
			{
				//aktualizacja na ekranie
				Lcd_DisplayString (2,1,"tryb normalny");
				Lcd_WriteControl(0x0C);
			}
		}
		else if(SBUF=='b')
		{
			trc=1;
			if(widok==1)
			{
				//aktualizacja na ekranie
				Lcd_DisplayString (2,1,"tryb sportowy");
				Lcd_WriteControl(0x0C);
			}
		}
		else if(SBUF=='c')
		{
			trc=2;
			if(widok==1)
			{
				//aktualizacja na ekranie
				Lcd_DisplayString (2,1,"wylaczona    ");
				Lcd_WriteControl(0x0C);
			}
		}
  }
} 

void main(void){
  LcdInit();   // Zainicjowanie rejestrów wyœwietlacza LCD
  Init();
  LcdKlimaOFF();
  while(1) {;}
}