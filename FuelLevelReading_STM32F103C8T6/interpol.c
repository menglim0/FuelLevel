#include "interpol.h"

#include "stm32f10x.h"
#include "stm32lib.h"

u16 FuelLevel_Reading_AD_InputValue_PullDown[13]={1170,1328,1643,1894,2086,2217,2359,2458,2533,2633,2684,2731,2900}; //pull down 100 ohm
u16 FuelLevel_Reading_Percent_OutputValue[14]={255,255,242,222,198,163,129,92,59,39,20,10,0,0};
u16 FuelLevel_Reading_AD_InputValue[14]={3300,2926,2768,2453,2202,2010,1879,1737,1638,1563,1463,1412,1365,1000};

enum FaultState
{ NoFault,ShortLow,ShortHigh,Otherfault };

enum FaultState Fuellevel_FaultState;

int Interpol_FindPoint(int x)
{
	char i;
	//char interpol_index;
	int interpol_x1,interpol_x2,interpol_y1,interpol_y2;
	
	u16 Interpol_Percent_OutputValue;
	if(x>3300)
	{
		Interpol_Percent_OutputValue =255;
		Fuellevel_FaultState = ShortHigh;
	}
	else if(x<1000)
	{
	Interpol_Percent_OutputValue = 0;
	Fuellevel_FaultState = ShortLow;
	}
	else
	{
		
			Fuellevel_FaultState = NoFault;
		for(i=0;i<14;i++)
		{
			if(FuelLevel_Reading_AD_InputValue[i]==x)
			{
				Interpol_Percent_OutputValue=FuelLevel_Reading_Percent_OutputValue[i];
				return Interpol_Percent_OutputValue;
			}
			else if(x<=FuelLevel_Reading_AD_InputValue[i]&&x>FuelLevel_Reading_AD_InputValue[i+1])
			{
				interpol_x1 = FuelLevel_Reading_AD_InputValue[i];
				interpol_x2 = FuelLevel_Reading_AD_InputValue[i+1];
				interpol_y1 =FuelLevel_Reading_Percent_OutputValue[i];
				interpol_y2 =FuelLevel_Reading_Percent_OutputValue[i+1];
				Interpol_Percent_OutputValue = Interpol_Calculate(interpol_x1,interpol_y1,interpol_x2,interpol_y2, x);
				return Interpol_Percent_OutputValue;
			}
			
		}
}
		return Interpol_Percent_OutputValue;

}

int Interpol_Calculate(int x1,int y1,int x2,int y2, int x)
{
	int y;
	double Temp_ratio;
	double Temp_ratio1;
	double Temp_ratio2;
	double Temp_ratio3;
	Temp_ratio1=y1-y2;
	Temp_ratio2=x1-x2;
	Temp_ratio3=x-x2;
	Temp_ratio=(Temp_ratio1/Temp_ratio2)*Temp_ratio3;
	//Temp_ratio = ((y1-y2)/(x2-x1))*(x-x1);
	y=y2+(int)Temp_ratio;
	return y;
}

char Get_FuelLevelFaule_State()
{return Fuellevel_FaultState;}
