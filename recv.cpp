#include <sys/shm.h>
#include <sys/msg.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include "msg.h" /* For the message struct */

#include <sys/stat.h>

using namespace std;

/* The size of the shared memory segment */
#define SHARED_MEMORY_CHUNK_SIZE 1000

/* The ids for the shared memory segment and the message queue */
int shmid, msqid;

/* The pointer to the shared memory */
void *sharedMemPtr = NULL;

/**
 * The function for receiving the name of the file
 * @return - the name of the file received from the sender
 */
string recvFileName()
{
    /* The file name received from the sender */
	string fileName;
        
	/* COMPLETE: declare an instance of the fileNameMsg struct to be
	 * used for holding the message received from the sender.
         */

	fileNameMsg msg;

        /* COMPLETE: Receive the file name using msgrcv() */

	printf("Receiving file name from sender...\n");
	if(msgrcv(msqid, &msg, sizeof(fileNameMsg) - sizeof(long), FILE_NAME_TRANSFER_TYPE, 0) < 0) {
		perror("msgrcv");
		exit(EXIT_FAILURE);
	}
	// else {
	// 	printf("SUCCESS: Message file name received.\n");
		fileName = msg.fileName;
	// }
	
	/* COMPLETE: return the received file name */
	
        return fileName;
}
/**
 * Sets up the shared memory segment and message queue
 * @param shmid - the id of the allocated shared memory 
 * @param msqid - the id of the shared memory
 * @param sharedMemPtr - the pointer to the shared memory
 */
void init(int &shmid, int &msqid, void *&sharedMemPtr)
{
    /* COMPLETE: 
        1. Create a file called keyfile.txt containing string "Hello world" (you may do
 	    so manually or from the code). */

	key_t key = ftok("keyfile.txt", 'a');

	/* 2. Use ftok("keyfile.txt", 'a') in order to generate the key. */

	printf("Generating ftok key...\n");
	if (key == -1) {
		perror("ftok");
		exit(EXIT_FAILURE);
	}
	// else
	// 	printf("SUCCESS: ftok key generated.\n");

	/* 3. Use will use this key in the TODO's below. Use the same key for the queue
	   and the shared memory segment. This also serves to illustrate the difference
 	   between the key and the id used in message queues and shared memory. The key is
	   like the file name and the id is like the file object.  Every System V object 
	   on the system has a unique id, but different objects may have the same key.
	*/

	/* COMPLETE: Allocate a shared memory segment. The size of the segment must be SHARED_MEMORY_CHUNK_SIZE. */

	printf("Allocating shared memory segment...\n");
	shmid = shmget(key, SHARED_MEMORY_CHUNK_SIZE, 0666 | IPC_CREAT);
	if(shmid == -1) {
		perror("shmget");
		exit(EXIT_FAILURE);
	}
	// else
	// 	printf("SUCCESS: Allocate shared memory segment.\n");
	
	/* COMPLETE: Attach to the shared memory */

	printf("Attaching to shared memory segment...\n");
	// sharedMemPtr = shmat(shmid, (void *) 0, 0);
	sharedMemPtr = shmat(shmid, (void*) 0, 0);

	if(sharedMemPtr == (void*) -1) {
		perror("shmat");
		exit(EXIT_FAILURE);
	}
	// else
	// 	printf("SUCCESS: Attached pointer to shared memory segment.\n");
	
	/* COMPLETE: Create a message queue */

	printf("Creating message queue...\n");
	msqid = msgget(key, 0666| IPC_CREAT);
	if(msqid == -1) {
		perror("msgget");
		exit(EXIT_FAILURE);
	}
	// else
	// 	printf("SUCCESS: Message queue created.\n");
	
	/* COMPLETE: Store the IDs and the pointer to the shared memory region in the corresponding parameters */
	
	// Completed in each of the previous tasks.
}

/**
 * The main loop
 * @param fileName - the name of the file received from the sender.
 * @return - the number of bytes received
 */
unsigned long mainLoop(const char *fileName)
{
    /* The size of the message received from the sender */
	int msgSize = -1;
	
	/* The number of bytes received */
	int numBytesRecv = 0;
	
	/* The string representing the file name received from the sender */
	string recvFileNameStr = fileName;
	
	/* COMPLETE: append __recv to the end of file name */

	recvFileNameStr.erase(recvFileNameStr.end() - 4, recvFileNameStr.end());
	recvFileNameStr.append("__recv.txt");
	
	/* Open the file for writing */
	FILE* fp = fopen(recvFileNameStr.c_str(), "w");
			
	/* Error checks */
	if(!fp)
	{
		perror("fopen");	
		exit(-1);
	}

    while(msgSize != 0)
	{	

		/* COMPLETE: Receive the message and get the value of the size field. The message will be of 
		 * of type SENDER_DATA_TYPE. That is, a message that is an instance of the message struct with 
		 * mtype field set to SENDER_DATA_TYPE (the macro SENDER_DATA_TYPE is defined in 
		 * msg.h).  If the size field of the message is not 0, then we copy that many bytes from 
		 * the shared memory segment to the file. Otherwise, if 0, then we close the file 
		 * and exit.
		 *
		 * NOTE: the received file will always be saved into the file called
		 * <ORIGINAL FILENAME__recv>. For example, if the name of the original
		 * file is song.mp3, the name of the received file is going to be song.mp3__recv.
		 */

		message msg;
		printf("Receiving message...\n");
		// printf("Size of message: %lu\n", sizeof(message));
		// printf("Size of long: %lu\n", sizeof(long));
		// printf("Size of msg: %lu\n", sizeof(msg));
		if(msgrcv(msqid, &msg, sizeof(message) - sizeof(long), SENDER_DATA_TYPE, 0) == -1) {
		// if(msgrcv(msqid, &msg, sizeof(msg), SENDER_DATA_TYPE, 0) == -1) {
			perror("msgrcv");
			fclose(fp);
			exit(EXIT_FAILURE);
		}
		// else {
		// 	printf("SUCCESS: Message received.\n");
			msgSize = msg.size;
		// }
		
		/* If the sender is not telling us that we are done, then get to work */
		if(msgSize != 0)
		{
			/* COMPLETE: count the number of bytes received */

			numBytesRecv += msgSize;
			printf("%d bytes received.\n", numBytesRecv);
			
			/* Save the shared memory to file */
			if(fwrite(sharedMemPtr, sizeof(char), msgSize, fp) < 0)
			{
				perror("fwrite");
			}
			
			/* COMPLETE: Tell the sender that we are ready for the next set of bytes. 
 			 * I.e., send a message of type RECV_DONE_TYPE. That is, a message
			 * of type ackMessage with mtype field set to RECV_DONE_TYPE. 
 			 */

			ackMessage nextMsg;
			nextMsg.mtype = RECV_DONE_TYPE;

			printf("Telling sender we are ready for the next set of bytes.\n");
			if(msgsnd(msqid, &nextMsg, sizeof(ackMessage) - sizeof(long), 0) == -1) {
			// if(msgsnd(msqid, &doneMsg, sizeof(doneMsg), 0) == -1) {
				perror("msgsnd");
				exit(EXIT_FAILURE);
			} 
			// else
			// 	printf("SUCCESS: Replied to sender.\n");

		}
		/* We are done */
		else
		{
			/* Close the file */
			fclose(fp);
		}
	}



	return numBytesRecv;
}

/**
 * Performs cleanup functions
 * @param sharedMemPtr - the pointer to the shared memory
 * @param shmid - the id of the shared memory segment
 * @param msqid - the id of the message queue
 */
void cleanUp(const int &shmid, const int &msqid, void *sharedMemPtr)
{
	/* COMPLETE: Detach from shared memory */

	printf("Detaching from shared memory...\n");
	if(shmdt(sharedMemPtr) == -1) {
		perror("shmdt");
		// exit(EXIT_FAILURE);
	}
	// else
	// 	printf("SUCCESS: Detached from shared memory.\n");
	
	/* COMPLETE: Deallocate the shared memory segment */

	printf("Deallocating the shared memory segment...\n");
	if(shmctl(shmid, IPC_RMID, NULL) == -1) {
		perror("shmctl");
		// exit(EXIT_FAILURE);
	}
	// else
	// 	printf("SUCCESS: Deallocated shared memory segment.\n");
	
	/* COMPLETE: Deallocate the message queue */

	printf("Deallocating message queue...\n");
	if(msgctl(msqid, IPC_RMID, NULL) == -1) {
		perror("msgctl");
		// exit(EXIT_FAILURE);
	}
	// else
	// 	printf("SUCCESS: Deallocated message queue.\n");
}

/**
 * Handles the exit signal
 * @param signal - the signal type
 */
void ctrlCSignal(int signal)
{
	/* Free system V resources */
	cleanUp(shmid, msqid, sharedMemPtr);
}

int main(int argc, char **argv)
{

	/* COMPLETE: Install a signal handler (see signaldemo.cpp sample file).
 	 * If user presses Ctrl-c, your program should delete the message
 	 * queue and the shared memory segment before exiting. You may add 
	 * the cleaning functionality in ctrlCSignal().
 	 */

	signal(SIGINT, ctrlCSignal);

	/* Initialize */
	init(shmid, msqid, sharedMemPtr);

	/* Receive the file name from the sender */
	string fileName = recvFileName();

	/* Go to the main loop */
	fprintf(stderr, "The number of bytes received is: %lu\n", mainLoop(fileName.c_str()));

	/* COMPLETE: Detach from shared memory segment, and deallocate shared memory 
	 * and message queue (i.e. call cleanup) 
	 */

	cleanUp(shmid, msqid, sharedMemPtr);

	return 0;
}
