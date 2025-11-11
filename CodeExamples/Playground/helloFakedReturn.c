// Call like this:
//     bin/helloFakedReturn
//
// or like this:
//     echo "Peter" | bin/helloFakedReturn
//
// or like this, if you are a hacker:
//     a) remove the PIE flag from the executable or disable ASLR in the running kernel
//     b) cat fakeinput | bin/helloFakedReturn

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

void pirate(){
	printf("You have been hacked!\n");
}

char* gets_unsave(char *somebuffer){
        char *cur = somebuffer; // a pointer used in the loop below
        // here we deliberately generate a vulnarability for buffer overflow
        do {
          char c = getchar();
          if (c == '\n' || c == EOF){
            *cur = '\0'; // terminate string and break
            break;
          } else {
            *cur = c;    // store character in buffer
          }
          cur++; // direct pointer onto the next slot
        } while (true);  // check  for overflow is missing here
        return somebuffer;
}
        
char* get_name(){
	char buf[15];      // a buffer for 14 + 1 characters
	printf("Please insert your surname (15 characters max): ");
        gets_unsave(buf);
	return strdup(buf);  // return a copy of the name on the heap
}

int main() {
	char *name=NULL; 
        printf("ASLR test: main is at %p\n", &main);
	name = get_name();
	if (name != NULL){
		printf("\nHello %s\n", name);
		free(name);
	}
	return 0;
}
