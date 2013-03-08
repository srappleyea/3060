#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <fcntl.h> // Needed for changing terminal flags
#include <termios.h> // Needed for terminal IO
#include <unistd.h>

#define BUFFER_SIZE (100)
#define LINE_DISPLAY (23)

void sigint_fnc(const struct termios);
int Set();
int Reset();
int reply(){
	int c;
	printf("\033[7m Waiting for command: \033[m");

	while((c = getchar()) != EOF){
		if(c == 'q')
			return 0;

		if(c == ' ')
			return 1;

		if(c == '\n')
			return 2;
	}
	
	printf("\n");
	return 0;
}

static int inputdesc;

int main(int argc, char* argv[])
{
	// Variables for handling input from the terminal
	inputdesc = 0; // 0 being the default for stdin
	FILE *input;
	inputdesc = open("/dev/tty", O_RDONLY); // Setting descriptor to the terminal
	input = fdopen(inputdesc, "r"); // Setting filepointer to the descriptor
	// Use input as you would any other file to get input chars
	
	struct termios term1;

	int user_input = 1;
	char file_contents[BUFFER_SIZE];
	int line_counter = 0;
	int show_file_name = 1;
	int bytes;
	FILE* fp;

	if (tcgetattr(STDIN_FILENO, &term1) != 0)
    	perror("tcgetattr() error");

	signal(SIGINT, sigint_fnc(term1));

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

		while(!feof(fp) && user_input != 0){
			line_counter = 0;
			
			//read the 23 lines from the file if space bar was hit
			if(user_input == 1){
				while(line_counter < LINE_DISPLAY && fgets(file_contents, BUFFER_SIZE, fp)){
					printf("%s", file_contents);
					++line_counter;
				}
			}

			//read only one line from the file "Enter" was hit
			else if(user_input == 2)
				if(fgets(file_contents, BUFFER_SIZE, fp))
					printf("%s", file_contents);

			else{
				printf("Invalid command given!\n");
				continue;
			}
			
			//the title of the file is to only named once so show it once then set the flag to not show
			//it again
			if(show_file_name == 1){
				printf("%s: ", argv[1]);
				show_file_name = 0;
			}

			//displaying the percentage of the file that has been currently displayed on the console
			printf("%.2f%% of the file is displayed.\n", (ftell(fp) / size) * 100);

			//wait for user input and use reverse video
			user_input = reply();
		}
	}

	//if no file name is given copy stdin to stdout
	else{
		printf("Copy stdin to stdout...\n");
		fp = stdin;

		while(fgets(file_contents, BUFFER_SIZE, fp)){
			printf("%s", file_contents);
			break;
		}

		for(bytes = 0; file_contents[bytes] != NULL; ++bytes);
		printf("bytes: %i\n", bytes);
	}

	Reset();
return 0;
}

void sigint_fnc(term1){
	if (tcsetattr(STDIN_FILENO, TCSADRAIN, &term1) != 0)
    	perror("tcsetattr() error");
	printf("terminal settings have been restored and the program is terminating");
	exit(-1);
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
