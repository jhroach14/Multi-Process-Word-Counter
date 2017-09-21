/*************************************************
	* C program to count no of lines, words and 	 *
	* characters in a file.			 *
	*************************************************/

#include <stdio.h>
#include <sys/wait.h>

#define FILEPATH "/tmp/CSCI4730/books"

typedef struct count_t {
		int linecount;
		int wordcount;
		int charcount;
} count_t;

count_t word_count(char *file)
{
		FILE *fp;
		char ch;

		count_t count;
		// Initialize counter variables
		count.linecount = 0;
		count.wordcount = 0;
		count.charcount = 0;

		printf("[pid %d] read: %s\n", getpid(), file);
		// Open file in read-only mode
		fp = fopen(file, "r");
		
//		sleep(10);
		// If file opened successfully, then write the string to file
		if ( fp )
		{
				//Repeat until End Of File character is reached.	
				while ((ch=getc(fp)) != EOF) {
						// Increment character count if NOT new line or space
						if (ch != ' ' && ch != '\n') { ++count.charcount; }

						// Increment word count if new line or space character
						if (ch == ' ' || ch == '\n') { ++count.wordcount; }

						// Increment line count if new line character
						if (ch == '\n') { ++count.linecount; }
				}

				if (count.charcount > 0) {
						++count.linecount;
						++count.wordcount;
				}

				fclose(fp);
		}
		else
		{
				printf("Failed to open the file: %s\n", file);
		}

		return count;
}

#define MAX_PROC 100

int main(int argc, char **argv)
{
		int numFiles, numProcs;
		char filename[100];
		int pid[MAX_PROC];
		count_t total, tmp;

		if(argc < 3) {
				printf("usage: wc <# of files to count(1-10)> <# of processes to fork(1-10)\n");
				return 0;
		}

		numFiles = atoi(argv[1]);
		numProcs = atoi(argv[2]);
		if(numFiles <= 0 || numFiles > 10 || numProcs <= 0 || numProcs > 10) {
				printf("usage: wc <# of files to count(1-10)> <# of processes to fork(1-10)\n");
				return 0;
		}

		total.linecount = 0;
		total.wordcount = 0;
		total.charcount = 0;

		printf("counting %d files in %d processes..\n", numFiles, numProcs);


		/* ... */


		printf("\n=========================================\n");
		printf("Total Lines : %d \n", total.linecount);
		printf("Total Words : %d \n", total.wordcount);
		printf("Total Characters : %d \n", total.charcount);
		printf("=========================================\n");

		return(0);
}
