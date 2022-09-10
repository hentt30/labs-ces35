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
#include <fstream>

#include "http.h"

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
  
  // usa um vetor de caracteres para preencher o endereço IP do cliente
  

  // faz leitura e escrita dos dados da conexao 
  // utiliza um buffer de 20 bytes (char)
  bool isEnd = false;
  

  while (!isEnd) {
    struct sockaddr_in clientAddr;
  socklen_t clientAddrSize = sizeof(clientAddr);
  int clientSockfd = accept(sockfd, (struct sockaddr*)&clientAddr, &clientAddrSize);

  if (clientSockfd == -1) {
    perror("accept");
    return 4;
  }

    // zera a memoria do buffer
    char ipstr[INET_ADDRSTRLEN] = {'\0'};
  inet_ntop(clientAddr.sin_family, &clientAddr.sin_addr, ipstr, sizeof(ipstr));
  std::cout << "Accept a connection from: " << ipstr << ":" <<
    ntohs(clientAddr.sin_port) << std::endl;
    char buf[20] = {0};
    std::stringstream ss;
    

    // recebe ate 20 bytes do cliente remoto
    std::string textMessage;
    while(true)
    {
      
      memset(buf, '\0', sizeof(buf));
      if (recv(clientSockfd, buf, 20, 0) == -1) {
        perror("recv");
        return 5;
      }
      size_t itr = 0;
      while(buf[itr]!= '\0' && textMessage.find("\r\n\r\n") == (size_t)-1)
      {
        textMessage += buf[itr];
        ++itr;
      }
      
      if(textMessage.find("\r\n\r\n") != (size_t)-1)
      {
        break;
      }

    }

    std::vector<uint8_t> message;
    for(char c : textMessage)
    {
      message.push_back(c);
    }

    HTTPRequest request = HTTPRequest();
    
    HTTPResponse response = HTTPResponse();

    if (request.decode(message) == -1) {
      response.setStatusCode(400);
      response.setStatusMessage("Bad Request");
      response.setContentLength(0);
      response.setContentType("text/html");
      response.setBody("");
      response.setHttpVersion("HTTP/1.0");
    }
    else {
      
      std::ifstream resource;
      std::string path = "."+dir+request.getPath();
      resource.open(path);
      if (!resource) {
        response.setStatusCode(404);
        response.setStatusMessage("Not Found");
        response.setContentLength(0);
        response.setContentType("text/html");
        response.setBody("");
        response.setHttpVersion("HTTP/1.0");
      }
      else {
        std::stringstream buffer2;
        buffer2 << resource.rdbuf();
        std::string text_resource = buffer2.str();

        response.setStatusCode(200);
        response.setStatusMessage("OK");
        response.setContentLength(text_resource.length());
        response.setContentType("text/html");
        response.setBody(text_resource);
        response.setHttpVersion("HTTP/1.0");
      }
    }
    std::vector<uint8_t> encoded_response = response.encode();

    // envia de volta o buffer recebido como um echo
    for (size_t itr = 0; itr < encoded_response.size();) {
      memset(buf, '\0', sizeof(buf));
      for (size_t itr2 = 0; itr2 < BUFFER_SIZE && itr < encoded_response.size() ; ++itr2) {
        buf[itr2] = encoded_response[itr];
        ++itr;
      }
      if (send(clientSockfd, buf, BUFFER_SIZE, 0) == -1) {
        perror("send");
        return 6;
      }
    }
    

    // o conteudo do buffer convertido para string pode 
    // ser comparado com palavras-chave

    if (ss.str() == "close\n")
      break;

    // zera a string para receber a proxima
    ss.str("");
    close(clientSockfd);
  }

  // fecha o socket
  

  return 0;
}
