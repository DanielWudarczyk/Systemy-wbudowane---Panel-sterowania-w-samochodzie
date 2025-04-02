#include <REGX52.H>

unsigned char pwmHelperMotor1=0, pwmHelperMotor2=0;   //zmienne uzywane do sterowania kolorem zarowki i predkoscia silnika
unsigned char predkoscLicznik2=3, predkoscMianownik2=6;
unsigned char obroty2=0;
bit tryb=0, kierunek=0, on=0;

void Init(void)
{
	//inicjalizowanie komunikacji UART
  P3_4 = 0;
  SCON = 0x50;
  T2CON = 0x30;
  TH2 = RCAP2H = 0xFF;
  TL2 = RCAP2L = 0xDC;
	ES=1;
	EA=1;
	TR2=1;	
	
	//poczatkowe ustawienia silnika i zarowki
	P2_3=P2_4=0;
	P2_1=1;
	P2_2=0;
	P2_5=1;
	P2_6=0;
	
	TMOD=0x22;   //timery 1 i 2 w trybie 2
	
	//wlaczenie przerwan zewnetrznych i timerow
	IT0=IT1=1; 
	ET0=ET1=1; 
	EX0=EX1=1; 
	TR0=TR1=1;
}

//Obsluga przychodzacych sygnalow
void ISR_Serial(void) interrupt  4
{
  if(TI == 1) {TI = 0;}
  if(RI == 1)
  {
    RI = 0;
		
		if(SBUF=='6')
		{
			if(on==0)   //wlaczenie klimatyzacji
			{
				on=1;
				kierunek=0;
				obroty2=0;
			}
			else   //wylaczenie klimatyzacji
			{
				on=0;
				P2_3=0;
				P2_4=0;
			}
		}
		else if(SBUF=='7')   //zmiana trybu klimatyzacji
		{
			if(tryb==0)
			{
				tryb=1;
				pwmHelperMotor1=0;
				//zapalona gorna zarowka
				P2_1=0;
				P2_2=1;
			}
			else
			{
				tryb=0;
				pwmHelperMotor1=0;
				//zapalona dolna zarowka
				P2_1=1;
				P2_2=0;
			}
		}
		else if(SBUF=='8')
		{
			if(predkoscLicznik2>1)
			{
				predkoscLicznik2--;   //zmniejszenie predkosci motora
				pwmHelperMotor2=0;
			}
		}
		else if(SBUF=='9')
		{
			if(predkoscLicznik2<6)
			{
				predkoscLicznik2++;   //zwiekszenie perdkosci motora
				pwmHelperMotor2=0;
			}
		}
  }
} 

//obsluga przerania INT0 - zliczanue obrotow motoru i zmiana kierunku
void INT0_ISR() interrupt 0
{
	obroty2+=1;
	if(obroty2==20)
	{
		obroty2=0;
		if(kierunek==0)
		{
			//w lewo
			P2_5=0;
			P2_6=1;
			kierunek=1;
		}
		else
		{
			//w prawo
			P2_5=1;
			P2_6=0;
			kierunek=0;
		}
	}
}

//Obsluga przerwania od timera 1 - ustawianie koloru zarowki
void Motor1_T1_ISR() interrupt 3 
{
	if(on==1)
	{
		if(tryb==0)   //tryb grzania
		{
			if(pwmHelperMotor1<3)
			{
				P2_3=1;
			}
			else
			{
				P2_3=0;
			}
			
			pwmHelperMotor1++;
			
			if(pwmHelperMotor1==7)
			{
				pwmHelperMotor1=0;
			}
		}
		else   //tryb chlodzenia
		{
			if(pwmHelperMotor1<5)
			{
				P2_3=1;
			}
			else
			{
				P2_3=0;
			}
			
			pwmHelperMotor1++;
			
			if(pwmHelperMotor1==5)
			{
				pwmHelperMotor1=0;
			}
		}
	}
}

//Obsluga przerwania od timera 0 - ustawianie predkosci silnika
void Motor2_T0_ISR() interrupt 1
{
	if(on==1)
	{
		if(pwmHelperMotor2<predkoscLicznik2)
		{
			P2_4=1;
		}
		else
		{
			P2_4=0;
		}
		
		pwmHelperMotor2++;
		
		if(pwmHelperMotor2==predkoscMianownik2)
		{
			pwmHelperMotor2=0;
		}
	}
}

void main(void)
{
	Init();
	while(1){
		;
  }
}