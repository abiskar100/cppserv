#include <iostream>
#include <netdb.h>
#include <sys/socket.h>
#include <cstring>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

void handleClient(int client_sockfd){

   const char* msg_buffer = "Hello from server";
   send(client_sockfd, msg_buffer, strlen(msg_buffer), 0);

   std::string completeMessage;

    char mfc[1024] = {0};
    ssize_t bytesR;

    while((bytesR = recv(client_sockfd, mfc, 1024, 0)) > 0){
            completeMessage.append(mfc, bytesR);
            std::cout<<"Received "<< bytesR << " bytes"<<std::endl;
    }

    if(bytesR == -1){
        std::cout<<"unable to receive the message from client"<<std::endl;
    } else {
        std::cout<<"client disconnected"<<std::endl;
    }

    std::cout<<std::endl;
    std::cout<<"Message Received: "<<std::endl;
    std::cout<<std::endl;
    std::cout<<completeMessage<<std::endl;

    close(client_sockfd);

}

int main()
{
    struct addrinfo hint, *res, *p;
    memset(&hint, 0, sizeof(hint));

    hint.ai_family = AF_UNSPEC;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_flags = AI_PASSIVE;
    hint.ai_protocol = 0;

    int resultfd = getaddrinfo(NULL, "8080", &hint, &res);
    int sockfd = -1;

    if(resultfd != 0){
        std::cout<<"ERROR:"<<gai_strerror(resultfd)<<std::endl;
        return 1;
    }

    for(p=res; p != NULL; p=p->ai_next){

    sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);

    if(sockfd == -1){
        std::cout<<"ERROR: unable to create socket!"<<std::endl;
        continue;
    } else {
        std::cout<<"SOCKET CREATED SUCESSFULLY!"<<std::endl;
    }
 
    if(bind(sockfd, p->ai_addr, p->ai_addrlen) == -1){
        close(sockfd);
        std::cout<<"ERROR: unable to bind the socket"<<std::endl;
        continue;
    }

    break;
    }

    if(p == NULL){
        std::cout<<"couldn't bind to any address"<<std::endl;
        freeaddrinfo(res);
        return 1;
    }
    
    if(listen(sockfd, SOMAXCONN) == -1){
        std::cout<<"ERROR: unable to listen "<<std::endl;
        close(sockfd);
        freeaddrinfo(res);
        return 1;
    } 

    std::cout<<"listening...."<<std::endl;
    
    struct sockaddr_storage client;
    socklen_t clientaddr_len = sizeof client;

    while(true){
    int client_sockfd = accept(sockfd, (struct sockaddr *)&client, &clientaddr_len);

    if(client_sockfd == -1){ 
        freeaddrinfo(res);
        close(client_sockfd);
        std::cout<<"Unalble to connect to the client"<<std::endl;
        return 1;
    } else {
        std::cout<<"connected to client SUCESSFULLY!"<<std::endl;
    }

    pid_t pid = fork();
    if(pid == 0){
        close(sockfd);
        handleClient(client_sockfd);
        exit(0);
    } else if (pid < 0){
        std::cout<<"fork failed"<<std::endl;
    }

    close(client_sockfd);
    }

    close(sockfd);
    freeaddrinfo(res);
}
