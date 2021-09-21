#include <stdio.h>

int main(){
	int c;
	puts("Enter text. press esc to exit\n\n");
	do{
		c = getchar();


		putchar(c);

	} while (c != 27);
return 0;
}

