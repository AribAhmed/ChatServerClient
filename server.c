// Server side implementation of UDP client-server model 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <errno.h>
#include <fcntl.h> 
#include <pthread.h> 

//#define PORT	 8080 
#define MAXLINE 10000

extern int errno ;

//char listInfo[MAXLINE];
char arrayOfLists[1000][1000];
char arrayOfInfo[1000][1000];
char usersConnected[1000][1000];
int numOfConUsers = 0;
int numOfUsers = 0;

void *insideThread(void *threadSockFd){
    while(1){

        int sockfd = (intptr_t) threadSockFd;
        int comm_fd = accept(sockfd, (struct sockaddr*) NULL, NULL);
        printf("after accept\n");
        char testClient[MAXLINE];
        read(comm_fd, testClient, MAXLINE);
        int clientNotTaken = 0;
        for(int e = 0; e <=numOfConUsers; e++){
            if(strcmp(testClient, usersConnected[e]) == 0){
                printf("User taken! Sending back error\n");
                write(comm_fd, "notok", strlen("notok"));
                clientNotTaken = 1;
            }
        }
        if(clientNotTaken == 0){
            strcpy(usersConnected[numOfConUsers], testClient);
            numOfConUsers++;
            write(comm_fd, "ok", strlen("ok"));
        

            char clientID[MAXLINE];
            char clientIP[MAXLINE];
            char clientPort[MAXLINE];
            int n;

            char buffer[MAXLINE];
            char waitInfo[MAXLINE];
            char listInfo[MAXLINE];
            char connectInfo[MAXLINE];

            strcpy(listInfo, "");

            while(1){
                
                memset(buffer,0,sizeof(buffer));
                int total = sizeof(buffer)-1;  
                strcpy(buffer, "");

                read(comm_fd,buffer,MAXLINE);
                printf("%s\n", buffer);

                char listInfoSend[MAXLINE];
                strcpy(listInfoSend, "");

                if(strcmp(buffer, "/list") == 0){

                    printf("Client wants the list\n");
                    if(numOfUsers == 0){
                        strcpy(listInfoSend, "No users connected");
                    }

                    int Eeee = 1;
                    for(int d = 1; d <= numOfUsers; d++){
                        printf("%s", arrayOfLists[d]); 
                        if(strcmp(arrayOfLists[d], "DONOTLIST") == 0){
                            continue;
                        }
                        char numOfUserStr[MAXLINE];
                        sprintf(numOfUserStr, "%i", Eeee);
                        strcat(numOfUserStr, ") ");
                        strcat(listInfoSend, numOfUserStr);

                        strcat(listInfoSend, arrayOfLists[d]);
                        Eeee++;
                    }

                    if(strcmp(listInfoSend, "") == 0){
                        strcpy(listInfoSend, "No users connected");
                    }
                    printf("%s", listInfoSend);
                    write(comm_fd, listInfoSend, strlen(listInfoSend)); 

                }else if(strcmp(buffer, "/wait") == 0){
                    
                    printf("Client wants to wait\n");
                    strcpy(waitInfo, "");
                    strcpy(clientID, "");
                    read(comm_fd,waitInfo,MAXLINE);
                    printf("Wait info: %s\n", waitInfo);
                    
                    numOfUsers++;
                    char numOfUserStr[MAXLINE];
                    //char listInfo[MAXLINE];
                    sprintf(numOfUserStr, "%i", numOfUsers);
                
                    char dummyWait[MAXLINE];
                    strcpy(dummyWait, waitInfo);
                    
                    char *token;
                    token = strtok(waitInfo, "/");
                    strcpy(clientID, token);
                    token = strtok(NULL, "/");
                    strcpy(clientPort, token);
                    token = strtok(NULL, "/");
                    strcpy(clientIP, token);
                    
                    printf("%s\n", dummyWait);
                    printf("Client ID: %s\n", clientID);
                    printf("Client Port: %s\n", clientPort);
                    printf("Client IP: %s\n", clientIP);

                    //strcat(numOfUserStr, ") ");
                    //strcat(listInfo, numOfUserStr);
                    strcpy(listInfo, "");
                    strcat(listInfo, clientID);
                    strcat(listInfo, "\n");

                    printf("ListInfo: %s\n", listInfo);

                    strcpy(arrayOfLists[numOfUsers], listInfo);
                    strcpy(arrayOfInfo[numOfUsers], dummyWait);

                    char waitInfo2[MAXLINE];
                    memset(waitInfo2,0,sizeof(waitInfo2));
                    read(comm_fd,waitInfo2,MAXLINE);
                    printf("%s\n", waitInfo2);

                    if(strcmp(waitInfo2, "/quit") == 0){
                        printf("User disconnected\n");
                        for(int e = 0; e <=numOfConUsers; e++){
                            if(strcmp(testClient, usersConnected[e]) == 0){
                                strcpy(usersConnected[e], "/;/;l/;/;/;/;dsa/f");
                            }
                        }
                    
                        return NULL;
                    }
                    
                    char removeList[MAXLINE];
                    char removeList2[MAXLINE];
                    strcpy(removeList, "");
                    for(int g = 1; g <= numOfUsers; g++){
                        strcpy(removeList, arrayOfLists[g]);
                       
                        printf("Before token: %s\n", removeList);
                       
                        strtok(removeList, "\n");
                        
                        if(strcmp(waitInfo2, removeList) == 0){
                            printf("Removing %s from list!\n", waitInfo2);
                            strcpy(arrayOfLists[g], "DONOTLIST");
                        }
                        
                    }

                    for(int g = 1; g <= numOfUsers; g++){
                        
                        strcpy(removeList2, arrayOfInfo[g]);
                      
                        printf("Before token: %s\n", removeList2);
                       
                        strtok(removeList2, "/");
                    
                        if(strcmp(waitInfo2, removeList2) == 0){
                            printf("Removing %s from infoList!\n", waitInfo2);
                            strcpy(arrayOfInfo[g], "DONOTLIST");
                        }
                    }
                    

            
                }else if(strcmp(buffer, "/connect") == 0){
                    
                    printf("Client wants to connect\n");
                    strcpy(connectInfo, "");
                    read(comm_fd, connectInfo, MAXLINE);
                    strtok(connectInfo, "\n");
                    printf("User wants to connect to: %s\n", connectInfo);
                    char dummyString[MAXLINE];
                    int boolCheck = 0;
                    for(int p = 0; p <= numOfUsers; p++){
                        printf("Original String: %s\n", arrayOfInfo[p]);
                        strcpy(dummyString, arrayOfInfo[p]);
                        strtok(dummyString, "/");
                        if(strcmp(dummyString, connectInfo) == 0){
                            printf("Found user!\n");
                            write(comm_fd, arrayOfInfo[p], strlen(arrayOfInfo[p])); 
                            boolCheck = 1;
                            break;
                        }

                    }
                    
                    if(boolCheck == 0){
                        printf("No user found\n");
                        write(comm_fd, "NA", strlen("NA")); 
                    }


                }else if(strcmp(buffer, "/quit") == 0){

                    printf("User disconnected\n");
                    for(int e = 0; e <=numOfConUsers; e++){
                        if(strcmp(testClient, usersConnected[e]) == 0){
                            strcpy(usersConnected[e], "/;/;l/;/;/;/;dsa/f");
                        }
                    }
                    
                    return NULL;
                }  
                    
            }

        }
    }
}


int main(int argc, char **argv) { 

    int sockfd, comm_fd;
	struct sockaddr_in servaddr;
     
	//Check the program arguments
	////////////////////////////////
	if(argc != 2){
        fprintf(stderr, "%s\n", "Only 1 argument allowed, server <port-number> ");
        exit(1);
    }

    int portNum = atoi(argv[1]); 

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
        fprintf(stderr, "%s\n", "Socket creationg error, exiting");
        exit(1);
    }

	bzero( &servaddr, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(portNum);

	if ( bind(sockfd, (const struct sockaddr *)&servaddr, 
			sizeof(servaddr)) < 0 ) { 
		fprintf(stderr, "bind failed\n"); 
		exit(1); 
	} 
	
    listen(sockfd, 10);
    int rc;
    pthread_t threads[100];
    
    for (int k = 0; k < 100; k++){
    
        rc = pthread_create(&threads[k], NULL, insideThread, (void *) (intptr_t) sockfd); 

        if (rc) {
            printf("Error:unable to create thread, %d\n", rc);
            exit(-1);
        }
    }
    
    pthread_exit(NULL);
    //return 0;
    
}