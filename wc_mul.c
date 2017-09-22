/*************************************************
	* C program to count no of lines, words and 	 *
	* characters in a file.			 *
	*************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <sys/ipc.h>

#define FILEPATH "/tmp/CSCI4730/books"

//data struct for count result
typedef struct count_t { 
	long linecount;
	long wordcount;
	long charcount;
} count_t;

//count method
count_t word_count(char *file){

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
		
//	sleep(10);
	// If file opened successfully, then write the string to file
	if ( fp ){
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

	}else{
		printf("Failed to open the file: %s\n", file);
	}

	return count;
}

struct msgbuf{

	long msgType;
	struct count_struct{
		long linecount;
		long wordcount;
		long charcount;
	} count;	
};

#define MAX_PROC 100

int main(int argc, char **argv){

	int numFiles, numProcs;
	char filename[100];
	int pid[MAX_PROC];
	count_t total, tmp;

	if(argc < 3) {//validate input
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


	//multi process implemented by james roach 9/17
	
	int filesToRead = 0;
	int fileIndex = 0;
	
	key_t key;
	int msqid;
	 
	if((key = ftok("/tmp/CSCI4730/books/text.00", 'j')) ==-1){
		perror("ftok");
		exit(1);
	}
	if((msqid = msgget(key, 0666 | IPC_CREAT)) == -1){
		perror("msgget");
		exit(1);
	}

	for(int i=0; i < numProcs; i++){ //for each process		
		
		if( i<(numFiles % numProcs)){
			filesToRead = numFiles/numProcs+1;
			
		}else{
			filesToRead = (numFiles/numProcs);
		}
		
		fileIndex += filesToRead;
		printf("Child Proc %d, read %d files (%d ~ %d)\n", i, filesToRead, (fileIndex-filesToRead), fileIndex-1);
 			
		if((pid[i] = fork()) == -1){ //fork child
			perror("fork");
			exit(1);
		}else
		if(pid[i] == 0){ //child
			struct count_t count, tmpCount;
			count.linecount = 0;
			count.wordcount = 0;
			count.charcount = 0;

			for(int j = (fileIndex-filesToRead); j < fileIndex; j++){
				char buffer[30];
				sprintf(buffer,"/tmp/CSCI4730/books/text.%02d", j);
				tmpCount = word_count(buffer);

				count.linecount += tmpCount.linecount;
				count.wordcount += tmpCount.wordcount;
				count.charcount += tmpCount.charcount;  
			}

			printf("[pid %d] send the result to the parent %d.\n", getpid(), getppid());
			
			struct msgbuf buf;
			buf.msgType = 1;
			buf.count.linecount = count.linecount;
			buf.count.wordcount = count.wordcount;
			buf.count.charcount = count.charcount;
			//printf("process %ld has %ld words, %ld lines, and %ld chars\n", getpid(), buf.count.wordcount,buf.count.linecount,buf.count.charcount);
			
			if(msgsnd(msqid, &buf, sizeof (struct count_struct), 0) == -1){
				perror("msgsend");
			}

			exit(0);
		}
	}
	
	int rv;
	for(int i=0; i < numProcs; i++){
		
		waitpid(pid[i], &rv, 0);
		if(rv == 0){
			printf("The child process %d terminated normally. The exit status 0\n",pid[i]);
		}else{
			printf("The child process %d terminated by a signal %d \n", pid[i], rv);
		}
	}

	struct msgbuf tmpMsg;
	for(int i=0; i < numProcs; i++){
		
		if(msgrcv(msqid, &tmpMsg, sizeof (struct count_struct), 1, 0) == -1){
			perror("msgrcv");
		}
		
		//printf("parent has recieved %ld words, %ld lines, and %ld chars\n", tmpMsg.count.wordcount,tmpMsg.count.linecount,tmpMsg.count.charcount);
		total.linecount += tmpMsg.count.linecount;
		total.wordcount += tmpMsg.count.wordcount;
		total.charcount += tmpMsg.count.charcount;
		//printf("parent now has %ld words, %ld lines, and %ld chars total\n", total.wordcount,total.linecount,total.charcount);
			
	
	}


	printf("\n=========================================\n");
	printf("Total Lines : %ld \n", total.linecount);
	printf("Total Words : %ld \n", total.wordcount);
	printf("Total Characters : %ld \n", total.charcount);
	printf("=========================================\n");

	return(0);
}
