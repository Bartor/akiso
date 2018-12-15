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
	
	int max_clients = 10;
	int clients[max_clients];
	char* nicknames[max_clients];
	
	for (int i = 0; i < max_clients; i++) {
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
		
		for(int i = 0; i < max_clients; i++) {
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
			for(int i = 0; i < max_clients; i++) {
				if (nicknames[i][0] != '\0') {
					strcat(temp, nicknames[i]);
				}
			}
			strcat(temp, "Please choose your nickname: ");
			if (send(newsocket, temp, strlen(temp), 0) != strlen(temp)) {
				perror("send");
				exit(1);
			}
			printf("welcome message send\n");
			
			for(int i = 0; i < max_clients; i++) {
				if (clients[i] == 0) {
					clients[i] = newsocket;
					printf("client added to clients\n\n");
					break;
				}
			}
		}
		
		for(int i = 0; i < max_clients; i++) {
			int sd = clients[i];
			char* nick = nicknames[i];
			
			if (FD_ISSET(sd, &rfds)) {
				int readBuffer[1024];
				
				int readsize = read(sd, readBuffer, 1024);
				if (readsize == 0) {
					close(sd);
					printf("%s disconnected\n", nicknames[i]);
					
					clients[i] = 0;
					nicknames[i][0] = '\0';
				} else {
					if (nick[0] == '\0') {
						strcpy(nicknames[i], readBuffer);
						printf("nickname established: %s\n", nicknames[i]);
						
						char temp[1024];
						strcpy(temp, "You are now known as ");
						strcat(temp, readBuffer);
						send(sd, temp, strlen(temp), 0);
						continue;
					}
					
					char mod[1024];
					strcpy(mod, readBuffer);
					char* to = strtok(mod, " ");
					printf("%s tries to send mess to %s\n", nick, to);
					int toindex = find(nicknames, to, max_clients);
					printf("found %s at %d\n", to, toindex);
					if (toindex >= 0) {
						char mess[1024];
						while (1) {
							to = strtok(NULL, " ");
							if (to == NULL) break;
							strcat(mess, to);
							strcat(mess, " ");
						}
						printf("mess=%s\n", mess);
						send(clients[toindex], mess, strlen(mess), 0);
					}
				}
			}
		}
	}
	return 1;
}