#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <fcntl.h> // Needed for changing terminal flags
#include <termios.h> // Needed for terminal IO
#include <unistd.h>

#define BUFFER_SIZE (100)

int Set();
int Reset();

static int inputdesc;

int main(int argc, char* argv[])
{
	// Variables for handling input from the terminal
	inputdesc = 0; // 0 being the default for stdin
	FILE *input;
	inputdesc = open("/dev/tty", O_RDONLY); // Setting descriptor to the terminal
	input = fdopen(inputdesc, "r"); // Setting filepointer to the descriptor
	// Use input as you would any other file to get input chars


	char user_input = ' ';
	char file_contents[BUFFER_SIZE];
	int line_counter = 0;
	FILE* fp;

	if(argc > 1){
		fp = fopen(argv[1], "r");

	Set();

		if(!fp){
			perror("There was a problem opening the file.\n");
			exit(-1);
		}

		fseek(fp, 0, SEEK_END);
		double size = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		while(!feof(fp) && user_input != 'q'){
			line_counter = 0;

			if(user_input == ' '){
				while(line_counter < 23 && !feof(fp)){
					if(fgets(file_contents, BUFFER_SIZE, fp)){
						printf("%s", file_contents);
						++line_counter;
					}
				}
			}

			else if(user_input == '\n')
				if(fgets(file_contents, BUFFER_SIZE, fp))
					printf("%s", file_contents);

			else
				printf("Invalid command given");

			printf("%s", argv[1]);
			printf(": %.2f%% of the file is displayed.\n", (ftell(fp) / size) * 100);
			user_input = getc(stdin);
		}
	}

	//if no file name is given copy stdin to stdout
	else{
		printf("Copy stdin to stdout...\n");
		fp = stdin;

		while(!feof(fp)){
			if(fgets(file_contents, BUFFER_SIZE, fp)){
				printf("%s", file_contents);
				++line_counter;
			}
		}
	}

	Reset();
return 0;
}

// Function Set
//		Setups passed file descriptor for no echo and nonblocking input
// Parameters - int <file descriptor to modify>
// Returns - (-1) for error, (0) for success 
// Requires the static int inputdesc for the input descriptor
int Set()
{
	struct termios info;
	if(tcgetattr(inputdesc, &info ) == -1)
	{ return -1; }
	
	info.c_lflag &= ~ECHO;
	info.c_lflag &= ~ICANON;
	
	if(tcsetattr (inputdesc, TCSANOW, &info) == -1)
	{ return -1; }
	 
    return 0;
}

// Function Reset
//		Setups passed file descriptor to reactivate echo and blocking input
// Parameters - int <file descriptor to modify>
// Returns - (-1) for error, (0) for success 
// Requires the static int inputdesc for the input descriptor
int Reset()
{
	struct termios info;
	if(tcgetattr(inputdesc, &info) == -1)
	{ return -1; }
	
	info.c_lflag |= ECHO;
	info.c_lflag |= ICANON;
	
	if(tcsetattr(inputdesc, TCSANOW, &info) == -1)
	{ return -1; }

	return 0;
}
