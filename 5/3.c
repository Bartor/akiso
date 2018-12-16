#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>

#define BUFFSIZE 1024
#define MAXCLIENTS 10

int find(char** arr, char* key, int size) {
	int ret = -1;
	for(int i = 0; i < size; i++) {
		if (!strncmp(arr[i], key, strlen(key))) ret = i;
	}
	return ret;
}

int main(int argc, char* argv[]) {
	fd_set rfds;
	int max_fd;
	
	int clients[MAXCLIENTS];
	char* nicknames[MAXCLIENTS];
	
	for (int i = 0; i < MAXCLIENTS; i++) {
		clients[i] = 0;
		nicknames[i] = calloc(1, sizeof(char));
		nicknames[i][0] = '\0';
	}
	
	int sockfd, portno, addrlen;
	
	struct sockaddr_in serv_addr;
	
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(1);
	}
	
	bzero((char*) &serv_addr, sizeof(serv_addr));
	portno = atoi(argv[1]);
	
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(portno);
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	
	if (bind(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
		perror("bind");
		exit(1);
	}
	
	listen(sockfd, 5);
	printf("Server is listening on port %d...\n", portno);
	
	addrlen = sizeof(serv_addr);
	
	while(1) {
		FD_ZERO(&rfds);
		
		FD_SET(sockfd, &rfds);
		max_fd = sockfd;
		
		for(int i = 0; i < MAXCLIENTS; i++) {
			int sd = clients[i];
			if (sd > 0) FD_SET(sd, &rfds);
			if (sd > max_fd) max_fd = sd;
		}
		
		int retval = select(max_fd + 1, &rfds, NULL, NULL, NULL);
		if ((retval < 0) && (errno != EINTR)) {
			perror("select");
			exit(1);
		}
		
		int newsocket;
		if (FD_ISSET(sockfd, &rfds)) {
			if((newsocket = accept(sockfd, (struct sockaddr*) &serv_addr, (socklen_t*) &addrlen)) < 0) {
				perror("accept");
				exit(1);
			}
			printf("new conncetion!\n");
			
			char temp[1024];
			strcpy(temp, "Henlo my fren! Users currently on server:\n\n");
			for(int i = 0; i < MAXCLIENTS; i++) {
				if (nicknames[i][0] != '\0') {
					strcat(temp, "- ");
					strcat(temp, nicknames[i]);
					strcat(temp, "\n");
				}
			}
			
			send(newsocket, temp, strlen(temp), 0);
			strcpy(temp, "\nChoose your nickname: ");
			send(newsocket, temp, strlen(temp), 0);
			
			printf("welcome message send\n");
			
			for(int i = 0; i < MAXCLIENTS; i++) {
				if (clients[i] == 0) {
					clients[i] = newsocket;
					printf("client added to clients\n\n");
					break;
				}
			}
		}
		
		for(int i = 0; i < MAXCLIENTS; i++) {
			//fetch file descriptor and nickname
			int clientFd = clients[i];
			char* clientNick = nicknames[i];
			
			//if something awaits of the fd, read it
			if (FD_ISSET(clientFd, &rfds)) {
				//we create a new readBuffer
				char readBuffer[BUFFSIZE];
				//we read from the fd
				int readSize = read(clientFd, readBuffer, BUFFSIZE);
				//get rid of trailing newline
				readBuffer[readSize - 1] = '\0';
				
				//if he didn't send anything, he just disconnected
				if (readSize == 0) {
					close(clientFd);
					printf("%s disconnected\n", clientNick);
					
					//reset his fd and nickname
					clients[i] = 0;
					nicknames[i][0] = '\0';
				} else { //if he sent something, let's process it
					//we fetch the first word of the message - it'll be useful later
					char* firstWord = strtok(readBuffer, " ");
					
					//if he doesn't have a nick yet, his first word of first mess will be it
					if (clientNick[0] == '\0') {
						//if he chose an existing nickname, make him chane it
						if (find(nicknames, firstWord, MAXCLIENTS) != -1) {
							char* reply = "That nickname is already taken, please choose something different: ";
							send(clientFd, reply, strlen(reply), 0);
							continue;
						}
						
						//we copy the first word to be a nickname
						strcpy(nicknames[i], firstWord);
						//we print to console about it
						printf("nickname established: %s\n", nicknames[i]);
						
						//we create a reply message
						char reply[1024];
						strcpy(reply, "You are now known as ");
						strcat(reply, firstWord);
						strcat(reply, "\n");
						
						send(clientFd, reply, strlen(reply), 0);
						
						//discard normal flow of messaging
						continue;
					}
					//we treat the first word as a recipients nickname
					int recipientIndex = find(nicknames, firstWord, MAXCLIENTS);
					//if recipient doesn't exist, tell it
					if (recipientIndex == -1) {
						char* reply = "Given recipient doesn't exist.\n";
						send(clientFd, reply, strlen(reply), 0);
						continue;
					} else {
						int recipientFd = clients[recipientIndex];
						char* word = strtok(NULL, " ");
						
						send(recipientFd, clientNick, strlen(clientNick) + 1, 0);
						char reply[1024];
						strcpy(reply, "\n");
						while (word != NULL) {
							strcat(reply, word);
							strcat(reply, " ");
							word = strtok(NULL, " ");
						}
						strcat(reply, "\n");
						send(recipientFd, reply, strlen(reply), 0);
					}
				}
			}
		}
	}
	return 1;
}