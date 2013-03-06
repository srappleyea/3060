#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <fcntl.h> // Needed for changing terminal flags
#include <termios.h> // Needed for terminal IO
#include <unistd.h>

int Set(int);
int Reset(int);

int main()
{
	// Variables for handling input from the terminal
	int inputdesc; 
	FILE *input;
	inputdesc = open("/dev/tty", O_RDONLY); // Setting descriptor to the terminal
	input = fdopen(inputdesc, "r"); // Setting filepointer to the descriptor
	// Use inputdesc with set/reset to setup terminal input for -echo and nonblocking
	// Use input as you would any other file to get input chars

return 0;
}

// Function Set
//		Setups passed file descriptor for no echo and nonblocking input
// Parameters - int <file descriptor to modify>
// Returns - (-1) for error, (0) for success 
int Set(int inputdesc)
{
	struct termios info;
	if(tcgetattr(inputdesc, &info ) == -1)
	{ return -1; }
	
	info.c_lflag &= ~ECHO;
	
	if(tcsetattr (inputdesc, TCSANOW, &info) == -1)
	{ return -1; }
	
	if ( info.c_lflag & ECHO)
   	{ printf("echo flag is set\n"); }
      
    return 0;
}

// Function Reset
//		Setups passed file descriptor to reactivate echo and blocking input
// Parameters - int <file descriptor to modify>
// Returns - (-1) for error, (0) for success 
int Reset(int inputdesc)
{
	struct termios info;
	if(tcgetattr(inputdesc, &info) == -1)
	{ return -1; }
	
	info.c_lflag |= ECHO;
	
	if(tcsetattr(inputdesc, TCSANOW, &info) == -1)
	{ return -1; }
	
	return 0;
}
