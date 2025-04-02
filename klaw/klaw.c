#include <REGX52.H>

unsigned char code Tab[] = {0xEF,0xDF,0xBF,0x7F};		//tablica chodzacych zer po wierszach
unsigned char data i;
unsigned char bdata Key;	//zmienna przechowujaca aktualny stan pinow P2
bit Enable=1;		//flaga zapewniajaca ze przycisniecie nie jest rejestrowane wiele razy
bit w1=0, w2=0, trc=0;
unsigned int a;
unsigned int timeout = 1000;

void wait()
{
	for(a=0;a<5000;a++){;}
}

//Inicjalizacja
void Init(void)
{
  P3_4 = 0;   //ustawienie w tryb odbioru
  SCON = 0x50;   //konfiguracja portu szeregowego - 8-bitowy UART z baud rate sterowanym Timerem, wlaczony odbior REN 
  T2CON = 0x30;   //ustawienie timera 2 - dziala jako timer w trybie auto-reload
  TH2 = RCAP2H = 0xFF;   //ustawnie wartosci poczatkowej i wartosci, ktora bedzie przeladowywana
  TL2 = RCAP2L = 0xDC;   //tak samo
	TI=0;   //wyzerowanie flagi udanej wysylki bajtu
	RI=0;   //bufor odbiorczy pusty
	ES=1;   //wlaczenie przerwan UART
	TR2=1;   //wlaczenie timera 2
	EA=1;   //wlaczenie globalnych przerwan
}

//Wysylanie danych
void Send(unsigned char Value)
{
  P3_4 = 1;   //ustawienie w tryb nadawania
    TI = 0;
      SBUF = Value;
      while(TI == 0 && timeout--){;}
			timeout=1000;
    TI = 0;
  P3_4 = 0;	  //ustawienie w tryb odbioru
}

//Decyzja po odebraniu sygnalu
void Decyzja(unsigned char Value)	
{
	if(Value==0xE7)	//1
	{
		if(trc==0)   //wylaczony tryb sterowania kontrola trakcji, czyli wlaczone sterowanie radiem
		{
			Send('1');   //poprzednia stacja radiowa
		}
		else   //sterujemy kontrola trakcji
		{
			Send('a');   //tryb normalny kontroli trakcji
		}
		w1=w2=0;   //zerowanie postepu we wpisywaniu kodu zmieniajacego tryb obslugi
	}
	else if(Value==0xEB)	//2
	{
		if(trc==0)
		{
			Send('2');   //nastepna stacja radiowa
		}
		else
		{
			Send('b');   //tryb sportowy kontroli trakcji
		}
		w1=w2=0;
	}
	else if(Value==0xED)	//3
	{
		if(trc==0)
		{
			Send('3');   //wyciszenie / odciszenie radia
		}
		else
		{
			Send('c');   //wylaczenie kontroli trakcji
		}
		w1=w2=0;
	}
	else if(Value==0xD7)	//4
	{
		if(trc==0)
		{
			Send('4');   //zmniejszenie glosnosci
		}
		w1=w2=0;
	}
	else if(Value==0xDB)	//5
	{
		if(trc==0)
		{
			Send('5');   //zwiekszenie glosnosci
		}
		w1=w2=0;
	}
	else if(Value==0xDD)	//6
	{
		Send('6');   //wlaczenie / wylaczenie klimatyzacji
		w1=w2=0;
	}
	else if(Value==0xB7)	//7
	{
		Send('7');   //zmiana trybu klimatyzacji
		w1=w2=0;
	}
	else if(Value==0xBB)	//8
	{
		Send('8');  //zmniejszenie poziomu nadmuchu
		w1=w2=0;
	}
	else if(Value==0xBD)	//9
	{
		Send('9');   //zwiekszenie poziomu nadmuchu
		w1=w2=0;
	}
	else if(Value==0x77)	//*
	{
		if(w1==0)
		{
			w1=1;   //wcisniety pierwszy przycisk kodu zmieniajacego tryb obslugi
		}
		w2=0;
	}
	else if(Value==0x7B)	//0
	{
		if(w1==1 && w2==0)
		{
			w2=1;   //wcisniety drugi z kolei przycisk kodu zmieniajacego tryb obslugi
		}
		else
		{
			w1=0;
		}
			
	}
	else if(Value==0x7D)	//#
	{
		//wcisnieto trzeci z kolei przycisk kodu zmieniajacego tryb obslugi
		if(w1==1 && w2==1)
		{
			if(trc==0)
			{
				trc=1;   //wlaczenie trybu obslugi kontroli trakcji
			}
			else
			{
				trc=0;   //wlacznie trybu obslugi radia
			}
			//podwojne migniecie diody
			if(P0_0==1)
			{
				P0_0=0;
				wait();
				P0_0=1;
				wait();
				P0_0=0;
				wait();
				P0_0=1;
			}
			else
			{
				P0_0=1;
				wait();
				P0_0=0;
				wait();
				P0_0=1;
				wait();
				P0_0=0;
			}
		}
		w1=w2=0;
	}
}

//Odbieranie danych
void ISR_Serial(void) interrupt  4
{
  if(TI == 1) {TI = 0;}
  if(RI == 1)   //odebrano bajt danych
  {
    RI = 0;
		
		if(SBUF=='d')
		{
			P0_0=0;   //zapalenie diody
		}
		else if(SBUF=='e')
		{
			P0_0=1;   //zgaszenie diody
		}
  }
}

void main(void)
{ 
	Init();
	while(1)
  {
    P2 = Tab[i];   //ustawienie zera na jednym z wierszy
    Key = P2;   //aktualny stan portow do ktorych jest podpieta klawiatura
    if(Key != Tab[i])   //wcisnieto klawisz
     {
       if(Enable==1)
       {
         Decyzja(Key);   //obsluzenie sygnalu
         Enable=0;	
       }
     }
    else
     {      
       i++;	
       if(i>3){i=0;}	
       Enable=1;
     }
  }
}