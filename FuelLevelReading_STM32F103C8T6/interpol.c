#include "interpol.h"

#include "stm32f10x.h"
#include "stm32lib.h"

extern int Interpol_Calculate(int x1,int y1,int x2,int y2, int x)
{
	int y;
	y=(y2-y1)/(x2-x1)*x;
	return y;
}