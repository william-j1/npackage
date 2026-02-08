#include "tests/units.c"

/*
entry point
*/
int main(void)
{
	setlocale(LC_ALL, "");
	iut1();
	iut2();
	iut3();
	iut4();
	return 0;
}
