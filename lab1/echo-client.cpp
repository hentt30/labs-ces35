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
using namespace std;


in_addr_t hostToIpAddr(const std::string& host) {
    hostent* hostname = gethostbyname(host.c_str());
    return (**(in_addr_t**)hostname->h_addr_list);
    
}

in_addr hostToIpAddrB(const std::string& host) {
    hostent* hostname = gethostbyname(host.c_str());
    return (**(in_addr**)hostname->h_addr_list);
    
}

std::string getHostFromUrl(const std::string & url){
  int itr = 0;
 
  if(url.compare(0,7,"http://") == 0){
    itr += 7;
  }

  std::string host = "";
  for(size_t  i = itr; i < url.length();++i){
    if(url[i] == ':' || url[i] =='/'){
      break;
    }
    host += url[i];
  }

  
  return host;
}

int getPortFromUrl(const std::string & url){
  int itr = 0;
  bool hasHttp = false;
  if(url.compare(0,7,"http://") == 0){
    itr += 7;
    hasHttp = true;
  }

  int reference = hasHttp ? 7:0;
  if(url.compare(reference,4,"www.") == 0){
    itr += 4;
  }

  std::string portString = "80";
  for(size_t  i = itr; i < url.length();++i){
    if(url[i] == ':'){
      ++i;
      portString = "";
      while(url[i] != '/' && i < url.length()){
        portString += url[i];
        ++i;
      }
      break;
    }
  }

  
  return std::stoi(portString);
}


std::string getPathFromUrl(const std::string & url){
  int itr = 0;
  bool hasHttp = false;
  if(url.compare(0,7,"http://") == 0){
    itr += 7;
    hasHttp = true;
  }

  int reference = hasHttp ? 7:0;
  if(url.compare(reference,4,"www.") == 0){
    itr += 4;
  }

  std::string path = "";
  for(size_t  i = itr; i < url.length();++i){
    if(url[i] == '/'){
      while(i < url.length()){
        path += url[i];
        ++i;
      }
    }
  }

  
  return path.size() ==0 ? "/":path;
}


int main(int argc, char *argv[]) {

  int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  for(int i = 1; i < argc; i++) {
    
    // cria o socket TCP IP
    

    // atividade de preenchimento da estrutura de endereço IP
    // e porta de conexão, precisa ser zerado o restante da estrutura
    // struct sockaddr_in {
    //  short            sin_family;   // e.g. AF_INET, AF_INET6
    //  unsigned short   sin_port;     // e.g. htons(3490)
    //  struct in_addr   sin_addr;     // see struct in_addr, below
    //  char             sin_zero[8];  // zero this if you want to
    // };
    //cout << inet_ntoa(hostToIpAddrB("localhost"))<<endl;
    std::string url = argv[i];
    
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(getPortFromUrl(url));     // short, network byte order
    serverAddr.sin_addr.s_addr = hostToIpAddr(getHostFromUrl(url));
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
    std::cout << "Set up a connection from: " << ipstr << ":" << ntohs(clientAddr.sin_port) << std::endl;

    // trecho de código para leitura e envio de dados nessa conexao
    // buffer eh o buffer de dados a ser recebido no socket com 20 bytes
    // input eh para a leitura do teclado
    // ss eh para receber o valor de volta
    char buf[BUFFER_SIZE] = {0};
    std::string input;
    std::stringstream ss;
    // zera o buffer
    memset(buf, '\0', sizeof(buf));

    HTTPRequest request = HTTPRequest();
    request.setMethod("GET");
    request.setHost(getHostFromUrl(url));
    request.setPath(getPathFromUrl(url));

    // converte a string lida em vetor de bytes 
    // com o tamanho do vetor de caracteres
    
    cout<<request.getHttpMessage()<<endl;
    std::vector<uint8_t> messageEncoded = request.encode();
    if (send(sockfd,&messageEncoded[0], messageEncoded.size(), 0) == -1) {
      perror("send");
      return 4;
    }

    
    std::vector<uint8_t> encodedMessage;
    while(true){
      // recebe no buffer uma certa quantidade de bytes ate 20 
      if (recv(sockfd, buf, BUFFER_SIZE, 0) == -1) {
        perror("recv");
        return 5;
      }
      size_t itr = 0;
      while(buf[itr] != '\0') {
        encodedMessage.push_back((uint8_t)buf[itr]);
        ++itr;
      }

      size_t size = encodedMessage.size();
      cout<<size<<endl;
      if(encodedMessage.size() < 4) continue;
      
      if(encodedMessage[size - 1] == 13 && encodedMessage[size - 2] == 10 && encodedMessage[size - 3] == 13 && encodedMessage[size - 4] == 10)
      {
        break;
      }

    }

    string a;
    for(uint8_t i : encodedMessage) a +=  (char)i;

    // coloca o conteudo do buffer na string
    // imprime o buffer na tela
    ss << a << std::endl;

    HTTPResponse response = HTTPResponse();
    response.decode(encodedMessage);

    std::ofstream resource(getPathFromUrl(url));
    resource << response.getBody();
    resource.close();

    ss << response.getHttpMessage() << std::endl;
    
    // o conteudo do buffer convertido para string pode 
    // ser comparado com palavras-chave
    if (ss.str() == "close\n")
      break;
    
    // zera a string ss
    ss.str("");
      
  }
  // fecha o socket
  close(sockfd);

  return 0;
}
