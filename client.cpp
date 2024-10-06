#include <unistd.h>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <cstring>
#include <string>

int main()
{
    struct addrinfo hint, *res, *p;
    memset(&hint, 0, sizeof(hint));

    hint.ai_family = AF_UNSPEC;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_protocol = 0;

    int resultfd= getaddrinfo("localhost", "8080", &hint, &res);

    int sockfd = -1;

    if(resultfd != 0){
        std::cout<<"ERROR: "<<gai_strerror(resultfd)<<std::endl;
        return 1;
    }
    for(p=res; p != NULL; p = p->ai_next){

    sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);

    if(sockfd == -1){
        std::cout<<"ERROR: unable to create socket!"<<std::endl;
        continue;
        } else {
             std::cout<<"SOCKET CREATED SUCESSFULLY!"<<std::endl;
        } 
    
    if(connect(sockfd, p->ai_addr, p->ai_addrlen) == -1 ){
        std::cout<<"Unable to connect to host"<<std::endl;
        close(sockfd);
        continue;
    } else {
        std::cout<<"Connected"<<std::endl;
        break;
        }
    } 

    if(p == NULL){
        freeaddrinfo(res);
        close(sockfd);
        return 1;
    }

    char msg_buffer[1024] = {0};
    ssize_t bytesRec =  recv(sockfd, msg_buffer, sizeof(msg_buffer)-1, 0);

    if(bytesRec == -1){
        std::cout<<"unable to receive message"<<std::endl;
    }  else {
        std::cout<<"Message Received: "<< msg_buffer << std::endl;
    }

    std::string msg;
    std::cout<<"write your message: "<<std::endl;
    std::getline(std::cin, msg);
    std::size_t msglen = msg.size();

    send(sockfd, msg.c_str(), msglen, 0);
    
    close(sockfd);
    freeaddrinfo(res);
}
