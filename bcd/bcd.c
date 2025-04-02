#include <REGX52.H>

unsigned char xdata Ones _at_ 0xFD00;   //zmienna do sterowania cyfra jednosci
unsigned char xdata Tens _at_ 0xFE00;   //zmienna do sterowanie cyfra dziesiatek
unsigned char Koder[]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F};   //zakodowane cyfry do wyswietlenia na BCD
unsigned char stacje[][10]={{0x40,0x50,0x15,0x71,0x00,0x71,0x15},{0x40,0x79,0x6D,0x76,0x77},{0x40,0x2A,0x77,0x2A,0x77}}; //RMF FM, ESKA, WAWA
bit wlaczone=0, wyciszone=0, miganie=1; 
unsigned char stacja=0, glosnosc=15, przytrzymanie_counter=0;
unsigned int czas=0;
unsigned char pierwsza_litera=0, druga_litera=1, zmiana=0, idx=0;
	
void Init(void)
{
	//inicjalizacja transmisji UART
  P3_4 = 0;
  SCON = 0x50;
  T2CON = 0x30;
  TH2 = RCAP2H = 0xFF;
  TL2 = RCAP2L = 0xDC;
	TI=0;
	RI=0;
	ES=1;
	TR2=1;
	
	//ustawienie timerow i przerwan
	TMOD = 0x00;
	EA = 1; 
	EX0 = 1; 
	IT0 = 1;
	ET0 = 1; 
	TR0 = 1; 
	ET1 = 1;
	TR1 = 1;
}

void Send(unsigned char Value)
{
  P3_4 = 1;
    TI = 0;
      SBUF = Value;
      while(TI == 0){;}
    TI = 0;    
  P3_4 = 0;	
}

//Funkcja wyswietlajaca przez chwile dane na BCD
void Wyswietl(unsigned char Tens_value, unsigned char Ones_value)
{
	unsigned int czas=0;
	while(czas<5000)
	{
		Tens=Tens_value;
		Ones=Ones_value;
		czas++;
	}
}

//Obsluga przychodzacych sygnalow
void ISR_Serial(void) interrupt  4
{
  if(TI == 1) {TI = 0;}
  if(RI == 1)
  {
    RI = 0;
		
		if(wlaczone)
		{
			if(SBUF=='1')	//zmiana stacji w lewo
			{
				if(stacja==0)
				{
					stacja=2;
				}
				else
				{
					stacja=(stacja-1)%3;
				}
				zmiana=0;
				pierwsza_litera=0;
				druga_litera=1;
			}
			else if(SBUF=='2')	//zmiana stacji w prawo
			{
				stacja=(stacja+1)%3;
				zmiana=0;
				pierwsza_litera=0;
				druga_litera=1;
			}
			else if(SBUF=='3')	//wyciszenie / odciszenie radia
			{
				if(!wyciszone)	//wyciszanie
				{
					wyciszone=1;
				}
				else	//odciszanie
				{
					wyciszone=0;
					Wyswietl(Koder[glosnosc/10],Koder[glosnosc%10]);
				}
			}
			else if(SBUF=='4')	//sciszenie radia
			{
				if(glosnosc!=0)
				{
					glosnosc--;
				}
				Wyswietl(Koder[glosnosc/10],Koder[glosnosc%10]);   //wyswietlenie glosnosci
			}
			else if(SBUF=='5')	//podgloszenie radia
			{
				if(glosnosc!=30)
				{
					glosnosc++;
				}
				Wyswietl(Koder[glosnosc/10],Koder[glosnosc%10]);
			}
		}
  }
}  

//Obsluga przycisku
void ISR_INT0() interrupt 0	
{
	if(wlaczone==0)	//wlaczanie radia
	{
		wlaczone=1;
		Send('d');   //wyslanie sygnalu do klawiatury (wlaczenie diody)
	}
}

//Obsluga timera 0
void ISR_T0() interrupt 1
{
	if(wlaczone==1)	//wylaczanie radia
	{
		if(P3_2==0 && przytrzymanie_counter<14)	
		{
			przytrzymanie_counter++;
		}
		if(przytrzymanie_counter>13)   //przytrzymano przycisk przez odpowiedni czas
		{
			Send('e');   //wyslanie sygnalu do klawiatury (wylaczenie diody)
			wlaczone=0;
			przytrzymanie_counter=0;
		}
	}
	if(P3_2==1)
	{
		przytrzymanie_counter=0;
	}
}

//Obsluga timera 1
void ISR_T1() interrupt 3
{
	if(wlaczone==1)	//obsluga wyswietlania stacji
	{
		if(stacja==0)   //wyswietlanie pierwszej stacji
		{
			if(zmiana==10)   //pora na zmiane liter na ekranie
			{
				if(pierwsza_litera<6)
				{
					pierwsza_litera++;
				}
				else
				{
					pierwsza_litera=0;
				}
				if(druga_litera<6)
				{
					druga_litera++;
				}
				else
				{
					druga_litera=0;
				}
				zmiana=0;
			}
			else
			{
				zmiana++;
			}
		}
		else	//stacja 1 lub 2
		{
			if(zmiana==10)
			{
				if(pierwsza_litera<4)
				{
					pierwsza_litera++;
				}
				else
				{
					pierwsza_litera=0;
				}
				if(druga_litera<4)
				{
					druga_litera++;
				}
				else
				{
					druga_litera=0;
				}
				zmiana=0;
			}
			else
			{
				zmiana++;
			}
		}
	}	
}

void main()
{
	Init();
	while(1)   //ciagle wyswietlanie stacji
	{
		if(wlaczone == 1) 	//radio wlaczone
		{
			if(!wyciszone)	//radio gra
			{
				Tens=stacje[stacja][pierwsza_litera];
				Ones=stacje[stacja][druga_litera];
			}
			else	//radio wyciszone - miga symbol
			{
				if(miganie==1)
				{
					Wyswietl(0x46,0x70);   //wyswietlany symbol
					miganie=0;
				}
				else
				{
					Wyswietl(0x00,0x00);   //czyszczenie ekranu
					miganie=1;
				}
			}
		}
	}
}