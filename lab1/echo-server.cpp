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

in_addr_t hostToIpAddr(const std::string& host) {
    hostent* hostname = gethostbyname(host.c_str());
    return (**(in_addr_t**)hostname->h_addr_list);
    
}

int main(int argc, char *argv[]) {

  // cria um socket para IPv4 e usando protocolo de transporte TCP
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  // Opções de configuração do SOCKETs
  // No sistema Unix um socket local TCP fica preso e indisponível 
  // por algum tempo após close, a não ser que configurado SO_REUSEADDR
  int yes = 1;
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
    perror("setsockopt");
    return 1;
  }

  // struct sockaddr_in {
  //  short            sin_family;   // e.g. AF_INET, AF_INET6
  //  unsigned short   sin_port;     // e.g. htons(3490)
  //  struct in_addr   sin_addr;     // see struct in_addr, below
  //  char             sin_zero[8];  // zero this if you want to
  // };

  struct sockaddr_in addr;
  std::string host;
  int port;
  std::string dir;
  // Adding command line arguments
  if(argc < 2){
    host = "localhost";
  }else{host = argv[1];}
  if(argc < 3){
    port = 3000;
  }else{port = atoi(argv[2]);}
  if(argc < 4){
    dir = "/tmp";
  }else{dir = argv[3];}
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);     // porta tem 16 bits, logo short, network byte order
  addr.sin_addr.s_addr = hostToIpAddr(host);
  memset(addr.sin_zero, '\0', sizeof(addr.sin_zero));

  // realizar o bind (registrar a porta para uso com o SO) para o socket
  if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
    perror("bind");
    return 2;
  }

  // colocar o socket em modo de escuta, ouvindo a porta 
  if (listen(sockfd, 1) == -1) {
    perror("listen");
    return 3;
  }

  // aceitar a conexao TCP
  // verificar que sockfd e clientSockfd sao sockets diferentes
  // sockfd eh a "socket de boas vindas"
  // clientSockfd eh a "socket diretamente com o cliente"
  struct sockaddr_in clientAddr;
  socklen_t clientAddrSize = sizeof(clientAddr);
  int clientSockfd = accept(sockfd, (struct sockaddr*)&clientAddr, &clientAddrSize);

  if (clientSockfd == -1) {
    perror("accept");
    return 4;
  }

  // usa um vetor de caracteres para preencher o endereço IP do cliente
  char ipstr[INET_ADDRSTRLEN] = {'\0'};
  inet_ntop(clientAddr.sin_family, &clientAddr.sin_addr, ipstr, sizeof(ipstr));
  std::cout << "Accept a connection from: " << ipstr << ":" <<
    ntohs(clientAddr.sin_port) << std::endl;

  // faz leitura e escrita dos dados da conexao 
  // utiliza um buffer de 20 bytes (char)
  bool isEnd = false;
  char buf[20] = {0};
  std::stringstream ss;

  while (!isEnd) {
    // zera a memoria do buffer
    memset(buf, '\0', sizeof(buf));

    // recebe ate 20 bytes do cliente remoto
    if (recv(clientSockfd, buf, 20, 0) == -1) {
      perror("recv");
      return 5;
    }

    // Imprime o valor recebido no servidor antes de reenviar
    // para o cliente de volta
    ss << buf << std::endl;
    std::cout << buf << std::endl;
    // We should analyze the buffer call
    
    // envia de volta o buffer recebido como um echo
    if (send(clientSockfd, buf, 20, 0) == -1) {
      perror("send");
      return 6;
    }

    // o conteudo do buffer convertido para string pode 
    // ser comparado com palavras-chave
    if (ss.str() == "close\n")
      break;

    // zera a string para receber a proxima
    ss.str("");
  }

  // fecha o socket
  close(clientSockfd);

  return 0;
}
