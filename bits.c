#include <stdio.h>

unsigned int main(void){

	unsigned int i;
	i = 16;
	i &= ~(8);

	printf("%d\n\r", i);

	i |= 8;

	printf("%d\n\r", i);

	i &= ~(112);

	printf("%d\n\r", i);

	return 0;
}