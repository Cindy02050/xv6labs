#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void printPrime(int *input, int count)
{
  if (count == 0) {
    return;
  }
  int p[2], i, prime = *input;
  pipe(p);
  char buff[4];
  printf("prime %d\n", prime);
  if (fork() == 0) {
        //0 -> child, read from left and write to right 
	close(p[0]);
	for (i = 0; i < count; i++) {
	  write(p[1], (char *)(input + i), 4);
	}
	close(p[1]);
	exit(0);
  } else {
        //!0 -> parent, read from left
	close(p[1]);
	count = 0;
	while (read(p[0], buff, 4) != 0) {
	  int temp = *((int *)buff);
	  if (temp % prime) {
	    *input++ = temp;
		count++;
	  }
	}
	printPrime(input - count, count);
	close(p[0]);
	wait(0);
  }
}

int main(int argc, char *argv[]) 
{
  if (argc != 1)
  {
    fprintf(2, "Please use it by: primes\n");
    exit(1);	
  }
  int input[34];
  for (int i = 0; i < 34; i++) {
    input[i] = i + 2;
  }
  printPrime(input, 34);
  exit(0);
}
