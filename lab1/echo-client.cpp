#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>

#include <iostream>
#include <sstream>

#include "http.h"
using namespace std;

in_addr hostToIpAddr(const std::string& host) {
    hostent* hostname = gethostbyname(host.c_str());
    return (**(in_addr**)hostname->h_addr_list);
    
}

int main(int argc, char *argv[]) {


  for(int i = 1; i < argc; i++) {
    // cria o socket TCP IP
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // atividade de preenchimento da estrutura de endereço IP
    // e porta de conexão, precisa ser zerado o restante da estrutura
    // struct sockaddr_in {
    //  short            sin_family;   // e.g. AF_INET, AF_INET6
    //  unsigned short   sin_port;     // e.g. htons(3490)
    //  struct in_addr   sin_addr;     // see struct in_addr, below
    //  char             sin_zero[8];  // zero this if you want to
    // };
    //cout << inet_ntoa(hostToIpAddr("google.com")) << endl;
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(80);     // short, network byte order
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(serverAddr.sin_zero, '\0', sizeof(serverAddr.sin_zero));

    // conecta com o servidor atraves do socket
  
    if (connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
      perror("connect");
      return 2;
    }

    // a partir do SO, eh possivel obter o endereço IP usando 
    // pelo cliente (nos mesmos) usando a funcao getsockname
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    if (getsockname(sockfd, (struct sockaddr *)&clientAddr, &clientAddrLen) == -1) {
      perror("getsockname");
      return 3;
    }

    // em caso de multiplos endereços, isso permite o melhor controle
    // a rotina abaixo, imprime o valor do endereço IP do cliente
    char ipstr[INET_ADDRSTRLEN] = {'\0'};
    inet_ntop(clientAddr.sin_family, &clientAddr.sin_addr, ipstr, sizeof(ipstr));
    std::cout << "Set up a connection from: " << ipstr << ":" <<
      ntohs(clientAddr.sin_port) << std::endl;

    // trecho de código para leitura e envio de dados nessa conexao
    // buffer eh o buffer de dados a ser recebido no socket com 20 bytes
    // input eh para a leitura do teclado
    // ss eh para receber o valor de volta
    char buf[20] = {0};
    std::string input;
    std::stringstream ss;
    // zera o buffer
    memset(buf, '\0', sizeof(buf));

    // make the requests
    for(size_t itr =  1; itr < (size_t)argc; ++itr){
      
    }
    std::cout << "send: ";
    std::cin >> input;

    // converte a string lida em vetor de bytes 
    // com o tamanho do vetor de caracteres
    if (send(sockfd, input.c_str(), input.size(), 0) == -1) {
      perror("send");
      return 4;
    }

    // recebe no buffer uma certa quantidade de bytes ate 20 
    if (recv(sockfd, buf, 20, 0) == -1) {
      perror("recv");
      return 5;
    }

    // coloca o conteudo do buffer na string
    // imprime o buffer na tela
    ss << buf << std::endl;
    std::cout << "echo: ";
    std::cout << buf << std::endl;

    // se a string tiver o valor close, sair do loop de eco
    if (ss.str() == "close\n")
      break;

    // zera a string ss
    ss.str("");
  }

  // fecha o socket
  close(sockfd);

  return 0;
}
