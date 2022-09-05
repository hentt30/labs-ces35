#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <string>
#include <iostream>

int main(int argc, char *argv[]) {
  struct addrinfo hints;
  struct addrinfo* res;

  if (argc != 2) {
    std::cerr << "usage: showip hostname" << std::endl;
    return 1;
  }

  // hints - modo de configurar o socket para o tipo  de transporte
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET; // IPv4
  hints.ai_socktype = SOCK_STREAM; // TCP

  // funcao de obtencao do endereco via DNS - getaddrinfo 
  // funcao preenche o buffer "res" e obtem o codigo de resposta "status" 
  int status = 0;
  if ((status = getaddrinfo(argv[1], "80", &hints, &res)) != 0) {
    std::cerr << "getaddrinfo: " << gai_strerror(status) << std::endl;
    return 2;
  }

  std::cout << "IP addresses for " << argv[1] << ": " << std::endl;

  for(struct addrinfo* p = res; p != 0; p = p->ai_next) {
    // a estrutura de dados eh generica e portanto precisa de type cast
    struct sockaddr_in* ipv4 = (struct sockaddr_in*)p->ai_addr;

    // e depois eh preciso realizar a conversao do endereco IP para string
    char ipstr[INET_ADDRSTRLEN] = {'\0'};
    inet_ntop(p->ai_family, &(ipv4->sin_addr), ipstr, sizeof(ipstr));
    std::cout << "  " << ipstr << std::endl;
  }

  freeaddrinfo(res); // libera a memoria alocada dinamicamente para "res"
  return 0;
}
