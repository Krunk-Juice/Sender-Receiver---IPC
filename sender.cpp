#include <sys/shm.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include "msg.h"    /* For the message struct */

using namespace std;

/* The size of the shared memory segment */
#define SHARED_MEMORY_CHUNK_SIZE 1000

/* The ids for the shared memory segment and the message queue */
int shmid, msqid;

/* The pointer to the shared memory */
void* sharedMemPtr;

/**
 * Sets up the shared memory segment and message queue
 * @param shmid - the id of the allocated shared memory 
 * @param msqid - the id of the allocated message queue
 */
void init(int& shmid, int& msqid, void*& sharedMemPtr)
{
	/* COMPLETE: 
        1. Create a file called keyfile.txt containing string "Hello world" (you may do
 	    so manually or from the code). */

	key_t key = ftok("keyfile.txt", 'a');

	/* 2. Use ftok("keyfile.txt", 'a') in order to generate the key. */

	printf("Generating ftok key...\n");
	if (key == -1) {
		perror("FAILURE: ftok key has not been generated.\n");
		exit(EXIT_FAILURE);
	}
	else
		printf("SUCCESS: ftok key generated.\n");


	/* 3. Use will use this key in the TODO's below. Use the same key for the queue
	   and the shared memory segment. This also serves to illustrate the difference
 	   between the key and the id used in message queues and shared memory. The key is
	   like the file name and the id is like the file object.  Every System V object 
	   on the system has a unique id, but different objects may have the same key.
	*/
	

	
	/* COMPLETE: Get the id of the shared memory segment. The size of the segment must be SHARED_MEMORY_CHUNK_SIZE */

	printf("Obtaining ID of the shared memory segment...\n");
	shmid = shmget(key, SHARED_MEMORY_CHUNK_SIZE, 0666|IPC_CREAT);
	if (shmid == -1) {
		perror("FAILURE: Failed to obtain the shared memory segment ID.\n");
		exit(EXIT_FAILURE);
	}
	else
		printf("SUCCESS: Shared memory segment ID obtained.\n");
	

	/* COMPLETE: Attach to the shared memory */

	printf("Attaching pointer to shared memory...\n");
	sharedMemPtr = shmat(shmid,(void *) 0, 0);
	if(sharedMemPtr == (void*) -1) {
		perror("FAILURE: Unable to attach pointer to shared memory.\n");
		exit(EXIT_FAILURE);
	}
	else
		printf("SUCCESS: Attached pointer to shared memory segment.\n");

	/* COMPLETE: Attach to the message queue */

	printf("Attaching message to queue...\n");
	msqid = msgget(key, 0666 | IPC_CREAT);
	if (msqid == -1) {
		perror("FAILURE: Unable to attach message to queue.\n");
		exit(EXIT_FAILURE);
	}
	else
		printf("SUCCESS: Attached to message queue.\n");
	

	/* Store the IDs and the pointer to the shared memory region in the corresponding function parameters */
}

/**
 * Performs the cleanup functions
 * @param sharedMemPtr - the pointer to the shared memory
 * @param shmid - the id of the shared memory segment
 * @param msqid - the id of the message queue
 */
void cleanUp(const int& shmid, const int& msqid, void* sharedMemPtr)
{
	/* COMPLETE: Detach from shared memory */

	printf("Detaching pointer from shared memory...\n");
	if (shmdt(sharedMemPtr) == -1) {
		perror("FAILURE: Unable to detach pointer from shared memory.\n");
		exit(EXIT_FAILURE);
	}
	else
		printf("SUCCESS: Pointer detached from shared memory.\n");
	
}

/**
 * The main send function
 * @param fileName - the name of the file
 * @return - the number of bytes sent
 */
unsigned long sendFile(const char* fileName)
{

	/* A buffer to store message we will send to the receiver. */
	message sndMsg; 
	
	/* A buffer to store message received from the receiver. */
	ackMessage rcvMsg;
		
	/* The number of bytes sent */
	unsigned long numBytesSent = 0;
	
	/* Open the file */
	FILE* fp =  fopen(fileName, "r");

	/* Was the file open? */
	if(!fp)
	{
		perror("fopen");
		exit(-1);
	}
	
	/* Read the whole file */
	while(!feof(fp))
	{
		/* Read at most SHARED_MEMORY_CHUNK_SIZE from the file and
 		 * store them in shared memory.  fread() will return how many bytes it has
		 * actually read. This is important; the last chunk read may be less than
		 * SHARED_MEMORY_CHUNK_SIZE.
 		 */
		if((sndMsg.size = fread(sharedMemPtr, sizeof(char), SHARED_MEMORY_CHUNK_SIZE, fp)) < 0)
		{
			perror("fread");
			exit(-1);
		}
		
		/* COMPLETE? TODO: count the number of bytes sent. */	

		numBytesSent += sndMsg.size;
		printf("%lu bytes sent.\n", numBytesSent);
			
		/* COMPLETE: Send a message to the receiver telling him that the data is ready
 		 * to be read (message of type SENDER_DATA_TYPE).
 		 */

		sndMsg.mtype = SENDER_DATA_TYPE;
		printf("Sending Message...\n");
		if(msgsnd(msqid,&sndMsg, sizeof(sndMsg), 0) == -1) {
			perror("FAILURE: Message was not sent.\n");
			exit(EXIT_FAILURE);
		}
		else
			printf("SUCCESS: Message was sent.\n");
		
		/* COMPLETE: Wait until the receiver sends us a message of type RECV_DONE_TYPE telling us 
 		 * that he finished saving a chunk of memory. 
 		 */

		printf("Wait until receiver sends us a message.\n");
		if(msgrcv(msqid, &rcvMsg, 0, RECV_DONE_TYPE, 0) == -1) {
			perror("FAILURE: Did not wait to receive message.\n");
			exit(-1);
		}
		else
			printf("SUCCESS: Waited to receive message.\n");

	}
	

	/** COMPLETE: once we are out of the above loop, we have finished sending the file.
 	  * Lets tell the receiver that we have nothing more to send. We will do this by
 	  * sending a message of type SENDER_DATA_TYPE with size field set to 0. 	
	  */
	
	sndMsg.size = 0;
	sndMsg.mtype = SENDER_DATA_TYPE;
	printf("Sender tells receiver there is nothing more to send.\n");
	if(msgsnd(msqid, &sndMsg, sizeof(sndMsg), 0) == -1) {
		perror("FAILURE: Sender could not tell receiver.\n");
		exit(EXIT_FAILURE);
	}
	else
		printf("SUCCESS: Sender told receiver.\n");
		
	/* Close the file */
	fclose(fp);
	
	return numBytesSent;
}

/**
 * Used to send the name of the file to the receiver
 * @param fileName - the name of the file to send
 */
void sendFileName(const char* fileName)
{
	/* Get the length of the file name */
	int fileNameSize = strlen(fileName);

	/* COMPLETE: Make sure the file name does not exceed 
	 * the maximum buffer size in the fileNameMsg
	 * struct. If exceeds, then terminate with an error.
	 */

	if(fileNameSize > (sizeof(fileNameMsg) - sizeof(long))) {
		perror("FAILURE: File name exceeded the maximum buffer.\n");
		exit(EXIT_FAILURE);
	}
	else
		printf("SUCCESS: File name does not exceed maximum buffer size.\n");

	/* COMPLETE: Create an instance of the struct representing the message
	 * containing the name of the file.
	 */

	fileNameMsg msg;

	/* COMPLETE: Set the message type FILE_NAME_TRANSFER_TYPE */

	msg.mtype = FILE_NAME_TRANSFER_TYPE;

	/* COMPLETE: Set the file name in the message */

	strncpy(msg.fileName, fileName, fileNameSize + 1);

	/* COMPLETE: Send the message using msgsnd */

	if(msgsnd(msqid, &msg, sizeof(msg) - sizeof(long), 0) == -1) {
		perror("FAILURE: Message could not be sent.\n");
		exit(EXIT_FAILURE);
	}
	else
		printf("SUCCESS: Message sent.\n");
}


int main(int argc, char** argv)
{
	
	/* Check the command line arguments */
	if(argc < 2)
	{
		fprintf(stderr, "USAGE: %s <FILE NAME>\n", argv[0]);
		exit(-1);
	}
		
	/* Connect to shared memory and the message queue */
	init(shmid, msqid, sharedMemPtr);
	
	/* Send the name of the file */
        sendFileName(argv[1]);
		
	/* Send the file */
	fprintf(stderr, "The number of bytes sent is %lu\n", sendFile(argv[1]));
	
	/* Cleanup */
	cleanUp(shmid, msqid, sharedMemPtr);
		
	return 0;
}