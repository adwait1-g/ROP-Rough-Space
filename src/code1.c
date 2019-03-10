#include<unistd.h>

int main() {
	execve("/bin//sh", 0, 0);
	return 0;
}
