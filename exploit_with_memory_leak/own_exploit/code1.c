#include<stdio.h>
#include<unistd.h>

void func();

int main() {

	printf("Address of read = %p\n", read);
	printf("Address of write = %p\n", write);

	func();

	return 0;
}


void func() {

	char buffer[100];
	read(0, buffer, 200);

	return;
}
