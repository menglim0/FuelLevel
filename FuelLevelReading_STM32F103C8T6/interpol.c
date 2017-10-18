#include "interpol.h"

#include "stm32f10x.h"
#include "stm32lib.h"

u16 FuelLevel_Reading_AD_InputValue[13]={1170,1328,1643,1894,2086,2217,2359,2458,2533,2633,2684,2731,2900};
u16 FuelLevel_Reading_Percent_OutputValue[13]={255,242,222,198,163,129,92,59,39,20,10,0,0};

int Interpol_FindPoint(int x)
{
	char i;
	char interpol_index;
	int interpol_x1,interpol_x2,interpol_y1,interpol_y2;
	
	u16 Interpol_Percent_OutputValue;
	if(x>2731)
	{
		Interpol_Percent_OutputValue =0;
	}
	else if(x<1170)
	{
	Interpol_Percent_OutputValue = 255;
	}
	else
	{
		for(i=0;i<12;i++)
		{
			if(FuelLevel_Reading_AD_InputValue[i]==x)
			{
				Interpol_Percent_OutputValue=FuelLevel_Reading_Percent_OutputValue[i];
				return Interpol_Percent_OutputValue;
			}
			else if(x<=FuelLevel_Reading_AD_InputValue[i+1]&&x>FuelLevel_Reading_AD_InputValue[i])
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
	Temp_ratio2=x2-x1;
	Temp_ratio3=x-x1;
	Temp_ratio=(Temp_ratio1/Temp_ratio2)*Temp_ratio3;
	//Temp_ratio = ((y1-y2)/(x2-x1))*(x-x1);
	y=y1-(int)Temp_ratio;
	return y;
}
