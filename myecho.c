#include<stdio.h>
#include<string.h>

int main (int ac, char* av[]) {
  int t = 0;
  int i = 1;
  if (strcmp (av[1], "-n") == 0) {
    t = 1;
    i++;
  }
  for (i; i <= ac; i++) {
		printf("%s", av[i]);
    if (i != ac)
        printf(" ");
  }
  if (t == 0)
      printf("\n");
}