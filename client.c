// Client side implementation of UDP client-server model 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <signal.h>
#include <errno.h>
#include <fcntl.h> 
#include <sys/types.h>
#include <sys/time.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <pthread.h> 
#include<err.h>

#define MAXLINE 10000
#define SIZE 100

int interrupted;
int valueCheck = 0;
int checkValue = 0;
int valueCheck2 = 0;
int checkValue2 = 0;
int returnvalue = 0;

struct threadData{
    char CLIENTID[MAXLINE];
    char CUSTOMERID[MAXLINE];
    int theSockFD;
};

void sighandler(int signum) {

    if(interrupted == 0){
        alarm(1);
    }else if(interrupted == 1){
        printf("\nStopped waiting.\n");
        alarm(1);
    }else if(interrupted == 2){
        interrupted = 3;
        alarm(1);
    }
   
}

void handle_sigalarm(int sig)
{
    /*Do nothing*/
}

void *waitCheck(void *check){
    returnvalue = 0;

    while(1){
        struct threadData *readData = check;
        printf("%s> ", readData->CUSTOMERID);
        char *str;
        char buffer[MAXLINE];
        str = malloc(1000);
        //alarm(2);
        fgets(str, 1000, stdin);
        if(strcmp(str, "/quit\n") == 0){
            returnvalue = 1;
            alarm(1);
            return NULL;
        }
        
        
        int errorar = errno;
        if (EINTR == errorar)
        {   
            return NULL;
        }

    }

    return NULL;
    
        

}

void *readThread(void *bread){
    valueCheck = 0;
    

    while(1){
    
        struct threadData *readData = bread;
        interrupted = 2;
        int connectSockFd = readData->theSockFD;
        char recvline[MAXLINE];
        memset(recvline,0,strlen(recvline));
        
        read(connectSockFd,recvline,MAXLINE);
        if(strcmp(recvline, "end") == 0){
            printf("\n%s has disconnected.\n", readData->CLIENTID);
            valueCheck = 3;
            alarm(1);
            
            return NULL;
        }
        
        if(checkValue == 1){
         
            valueCheck = 3;
            
            return NULL;
        }
        
        int errno_save = errno;
        if (EINTR == errno_save)
        {   
            
            valueCheck = 3;
           
            return NULL;
        }
        
    
        printf("\n%s: %s%s>\n", readData->CLIENTID, recvline, readData->CUSTOMERID);
        
    }
}

void *readThread2(void *bread){
    valueCheck2 = 0;

    while(1){
        
        struct threadData *readData = bread;
        interrupted = 2;
        int connectSockFd = readData->theSockFD;
        char recvline[MAXLINE];
        memset(recvline,0,strlen(recvline));
        read(connectSockFd,recvline,MAXLINE);
        //printf("%s\n", recvline);
        if(strcmp(recvline, "end") == 0){
            printf("\n%s has disconnected.\n", readData->CLIENTID);
            valueCheck2 = 3;
            alarm(1);
            
             return NULL;
        }
        
        if(checkValue2 == 1){
           
            valueCheck2 = 3;
          
            return NULL;
        }
        
        int errno_save = errno;
        if(EINTR == errno_save)
        {   
            
            valueCheck2 = 3;
            checkValue2 = 1;
            return NULL;
        }
        

        printf("\n%s: %s%s>\n", readData->CLIENTID, recvline, readData->CUSTOMERID);
        
    }
}




int main(int argc, char **argv) { 

	int sockfd; 
    char buffer[MAXLINE];
	struct sockaddr_in	 servaddr; 
    
    //check for program arguments
    ///////////////////////////////
    if(argc != 4){
        fprintf(stderr, "%s\n", "Only 3 arguments allowed, client <IP-Address> <Port> <Client-ID>");
        exit(1);
    } 

    time_t t;
    srand((unsigned) time(&t));
    char *IPAddress = argv[1];
    int Port = atoi(argv[2]);
    char *clientID = argv[3];

    sockfd=socket(AF_INET,SOCK_STREAM,0);
    if(sockfd < 0){
        fprintf(stderr, "%s\n", "Socket creationg error, exiting");
        exit(1);
    }
    bzero(&servaddr,sizeof servaddr);
    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(Port);
 
    inet_pton(AF_INET, IPAddress, &(servaddr.sin_addr));

    
    if(connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr)) < 0){
        fprintf(stderr, "%s\n", "Connection Error");
        exit(1);
    }

    //char str[MAXLINE];
    char str2[MAXLINE];
    
    struct ifaddrs *ifap, *ifa;
    struct sockaddr_in *sa;
    char *addr;

    getifaddrs (&ifap);
    ifa = ifap; 
    int i = 0;
    for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr && ifa->ifa_addr->sa_family==AF_INET) {
            sa = (struct sockaddr_in *) ifa->ifa_addr;
            addr = inet_ntoa(sa->sin_addr);
            if(i == 1){
                break;
            }
            i++;
        }
    }

    freeifaddrs(ifap);
    printf("Network Name: %s\tIP Address: %s\n", ifa->ifa_name, addr);
    write(sockfd, clientID, strlen(clientID));
    char clientnameCheck[MAXLINE];
    strcpy(clientnameCheck, "");
    read(sockfd, clientnameCheck, MAXLINE);
    if(strcmp(clientnameCheck, "notok") == 0){
        fprintf(stderr, "Name is already taken!\n");
        exit(1);
    }

    signal(SIGINT, sighandler);
    sigaction(SIGALRM , &(struct sigaction){handle_sigalarm}, NULL); 

    
    while(1){
        interrupted = 0;
        errno = 0;
        printf("%s> ", clientID);
        char *str;
        memset(str2,0,sizeof(buffer));
        str = malloc(1000);
        fgets(str, 1000, stdin);

        int errorar = errno;
        if (EINTR == errorar)
        {   
            /*
            printf("\n");
            write(sockfd, "/quit", strlen("/quit"));
            checkValue = 1;
            checkValue2 = 1;
            close(sockfd);
            exit(1);
            */
        }
        

        strcpy(str2, str);
        strtok(str, "\n");
        
        if(strcmp(str, "/list") == 0){

            write(sockfd, "/list", strlen("/list"));
            memset(buffer,0,sizeof(buffer));
            int total = sizeof(buffer)-1;  

            strcpy(buffer, "");
            read(sockfd,buffer,total);
            printf("%s\n", buffer);

        }else if(strcmp(str, "/wait") == 0){
            char connectclientID[MAXLINE];
            char connectClientID[MAXLINE];  
            int newPortNum = (rand() % (9999 - 1000 + 1)) + 100; 

            write(sockfd, "/wait", strlen("/wait"));
            
            char waitInfo[MAXLINE];
            strcpy(waitInfo, "");

            strcat(waitInfo, clientID);
            strcat(waitInfo, "/");

           
            char newPortNumStr[MAXLINE];
        
            struct sockaddr_in newaddr;
            int newFD;
            int newCommFD = -1;
            newFD = socket(AF_INET, SOCK_STREAM, 0);
            if(newFD < 0){
                fprintf(stderr, "%s\n", "Socket creation error, exiting");
                write(sockfd, "/quit", strlen("/quit"));
                exit(1);
            }
           
            bzero( &newaddr, sizeof(newaddr));
            newaddr.sin_family = AF_INET;
            newaddr.sin_addr.s_addr = INADDR_ANY;
            newaddr.sin_port = htons(newPortNum);
            
            int y = 0;
            while ( bind(newFD, (const struct sockaddr *)&newaddr, 
                    sizeof(newaddr)) < 0 ) { 
                newPortNum = (rand() % (9999 - 1000 + 1)) + 100; 
                newaddr.sin_port = htons(newPortNum); 
                y++;
                if(y == 10){
                    fprintf(stderr, "Bind failed failed, exiting");
                    write(sockfd, "/quit", strlen("/quit"));
                    exit(1);
                }
            } 

            sprintf(newPortNumStr, "%i", newPortNum);
            strcat(waitInfo, newPortNumStr);
            strcat(waitInfo, "/");
            strcat(waitInfo, addr);


            sleep(1);
            write(sockfd, waitInfo, strlen(waitInfo));
            interrupted = 1;
            
            listen(newFD, 10);
            
            //sigaction(SIGALRM , &(struct sigaction){handle_sigalarm}, NULL); 

            int bool = 0;
            while(1){
                
                if(bool == 1){
                    break;
                }
                checkValue2 = 0;

                printf("Waiting for connection.\n");

                pthread_t threadz[0];
                int dc; 
                struct threadData data2;
                strcpy(data2.CUSTOMERID, clientID);

                dc = pthread_create(&threadz[0], NULL, waitCheck, &data2); 

                if (dc) {

                    printf("Error:unable to create thread, %d\n", dc);
                    write(sockfd, "/quit", strlen("/quit"));
                    exit(-1);
                } 

                newCommFD = accept(newFD, (struct sockaddr*) NULL, NULL);
                    
                int errno_save = errno;
                
                alarm(0);
                if (-1 == newCommFD)
                {   
                    if(returnvalue == 1){
                        printf("Quitting out\n");
                        write(sockfd, "/quit", strlen("/quit"));
                        exit(1);
                    }
                    if (EINTR == errno_save)
                    {   
                        pthread_cancel(threadz[0]);
                        alarm(1);
                        write(sockfd, clientID, strlen(clientID));
                        sleep(1);
                        break;
                    }
                    
                }
              
                if(newCommFD != -1){
                    pthread_cancel(threadz[0]);
                    write(sockfd, clientID, strlen(clientID));
                    char sendline[MAXLINE];
                    char recvline[MAXLINE];
                    //char connectClientID[MAXLINE];
                    read(newCommFD, connectClientID, MAXLINE);
                    printf("Connection from %s.\n", connectClientID);
                    int rc;
                    pthread_t threads[0];
                    struct threadData data;

                    strcpy(data.CLIENTID, connectClientID);
                    data.theSockFD = newCommFD;
                    strcpy(data.CUSTOMERID, clientID);

                    rc = pthread_create(&threads[0], NULL, readThread2, &data); 

                    if (rc) {
                        printf("Error:unable to create thread, %d\n", rc);
                        write(sockfd, "/quit", strlen("/quit"));
                        exit(-1);
                    } 
                    while(1){
                   
                        
                        printf("%s> ", clientID);

                        memset(sendline,0,strlen(sendline));
                        fgets(sendline, MAXLINE, stdin);
                        if(valueCheck2 == 3){
                            bool = 1;
                            break;
                        }
                
                        errno_save = errno;
                        if (EINTR == errno_save)
                        {
                            
                            bool = 1;
                            checkValue2 = 1;
                            printf("\nLeft Conversation with %s.\n", connectClientID);
                            write(newCommFD,"end",strlen("end"));
                            break;
                            
                        } else {
                            if(strcmp(sendline, "/quit") == 0){
                                write(sockfd, "/quit", strlen("/quit"));
                                write(newCommFD,"end",strlen("end"));
                                close(newCommFD);
                                close(sockfd);
                                checkValue2 = 1;
                                exit(1);

                            }
                            write(newCommFD,sendline,strlen(sendline));
                        }
                    }
                } 
            }
            //pthread_exit(NULL);
            close(newCommFD);
            
        }else if(strcmp(str, "/quit") == 0){
            checkValue = 1;
            checkValue2 = 1;
            write(sockfd, "/quit", strlen("/quit"));
            close(sockfd);
            exit(1);
        }else if(strcmp(str, "/connect") >= 0){
            strtok(str, " ");

            if(strcmp(str, "/connect") == 0){
                checkValue = 0;
                char connectclientID[MAXLINE];
                char connectClientID[MAXLINE];
                write(sockfd, "/connect", strlen("/connect"));

                char connectID[MAXLINE];
                char connectRead[MAXLINE];
                strcpy(connectID, str2);
                
                
                char *userToConnect;
                userToConnect = strtok(connectID, " ");
                userToConnect = strtok(NULL, " ");
                
               
                sleep(1);
                
                write(sockfd, userToConnect, strlen(userToConnect));
                memset(connectRead,0,sizeof(connectRead));
                read(sockfd,connectRead,MAXLINE);
             

                if(strcmp(connectRead, "NA") == 0){
                    printf("No user found\n");
                } else {
                    

                    char connectclientIP[MAXLINE];
                    char connectclientPort[MAXLINE];

                    char *token;
                    token = strtok(connectRead, "/");
                    strcpy(connectclientID, token);
                    token = strtok(NULL, "/");
                    strcpy(connectclientPort, token);
                    token = strtok(NULL, "/");
                    strcpy(connectclientIP, token);

                    int connectSockFd; 
                    struct sockaddr_in	connectservaddr; 

                    connectSockFd=socket(AF_INET,SOCK_STREAM,0);
                    if(connectSockFd < 0){
                        fprintf(stderr, "%s\n", "Socket creationg error, exiting");
                        write(sockfd, "/quit", strlen("/quit"));
                        exit(1);
                    }
                    bzero(&connectservaddr,sizeof connectservaddr);
                    connectservaddr.sin_family=AF_INET;
                    connectservaddr.sin_port=htons(atoi(connectclientPort));
                
                    inet_pton(AF_INET, connectclientIP, &(connectservaddr.sin_addr));

                    if(connect(connectSockFd,(struct sockaddr *)&connectservaddr,sizeof(connectservaddr)) < 0){
                        fprintf(stderr, "%s\n", "Connection Error");
                        write(sockfd, "/quit", strlen("/quit"));
                        close(sockfd);
                        close(connectSockFd);
                        exit(1);
                    }
                    
                    char sendline[MAXLINE];
                    
                    write(connectSockFd, clientID, strlen(clientID));
                    printf("Connected to %s.\n", connectclientID);
                    sigaction(SIGALRM , &(struct sigaction){handle_sigalarm}, NULL); 
                    int rc;
                    pthread_t threads[0];
                    struct threadData data;

                    strcpy(data.CLIENTID, connectclientID);
                    data.theSockFD = connectSockFd;
                    strcpy(data.CUSTOMERID, clientID);

                    interrupted = 2;
                    rc = pthread_create(&threads[0], NULL, readThread, &data); 
                        
                    if (rc == 1) {
                        printf("Error:unable to create thread, %d\n", rc);
                        exit(-1);
                    } 
                        
                    
                    while(1){
                        
                        
                        printf("%s> ", clientID);
                        memset(sendline,0,strlen(sendline));

                        fgets(sendline, MAXLINE, stdin);
                        if(valueCheck == 3){
                            
                            break;
                        }
            
                        int errno_save = errno;
                        if (EINTR == errno_save)
                        {   
            
                            printf("\nLeft Conversation with %s.\n", connectclientID);
                            checkValue = 1;
                            write(connectSockFd,"end",strlen("end"));
                            break;

                        } else {
                            if(strcmp(sendline, "/quit\n") == 0){

                                checkValue = 1;
                                checkValue2 = 1;
                                write(sockfd, "/quit", strlen("/quit"));
                                write(connectSockFd,"end",strlen("end"));
                                close(connectSockFd);
                                close(sockfd);
                                checkValue = 1;
                                exit(1);
                                
                            }
            
                            write(connectSockFd,sendline,strlen(sendline));
                        }

                         
                        
                    }
                    
                    //pthread_exit(NULL);
                    close(connectSockFd);
                }

                
            }
            
        }else{
            printf("Command not recognized\n");

        }   
        
    }
    


}