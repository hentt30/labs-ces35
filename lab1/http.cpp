#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <stdlib.h>

#include "utils.h"

typedef uint8_t int8;

class HTTPRequest
{
    private:
        std::string method;
        std::string path;
        std::string host;
    public:
        void setPath(const std::string& path){ this->path = path;}
        std::string getPath(){ return this->path;}
        void setMethod(const std::string& method){ this->method = method;}
        void setHost(const std::string& host){ this->host = host;}
        
        std::string getHttpMessage(){
            std::string message = format<std::string>("%s %s HTTP/1.0\r\nHost: %s\r\n\r\n",this->method,this->path,this->host);
            return message;
        }
        std::vector<int8> encode(std::string& output){
            std::string message = this->getHttpMessage();
            std::vector<int8> byteCode;
            for(char value: message)
            {
                byteCode.push_back((int8)value);
            }
            return byteCode;
        } 

        int decode(std::vector<int8> data){
            try {
                //iterate line by line
                std::string line;
                bool line_end1 = false, line_end2 = false, end_header = false;
                for(size_t itr = 0; itr < data.size(); ++itr){
                    if(data[itr] == 13){ line_end1 = true;}
                    if(data[itr] == 10 && line_end1){ line_end2 = true;}
                    else{ line_end1 = false;}
                    if(data[itr] == 13 && line_end2){ end_header = true;}
                    else{ line_end2=false;}
                    if(data[itr] == 10 && end_header){
                        std::cout << "End message decoding" << std::endl;
                        break;
                    }
                    else{ end_header=false;}
                    if(line_end2 && !end_header){
                        //parse line
                        std::vector<std::string> arguments;
                        std::istringstream ss(line);
                        std::string token;
                        while(std::getline(ss, token, ' ')) {
                            arguments.push_back(token);
                        }
                        if(arguments[0] == "GET"){
                            this->setMethod(arguments[0]);
                            if(arguments[1] == "/"){ this->setPath("/index.html");}
                            else{ this->setPath(arguments[1]);}
                        }
                        else if(arguments[0] == "Host:"){ this->setHost(arguments[1]);}
                        line = "";
                    }
                    if(!line_end1){ line += (char) data[itr];}
                }
                return 0;
            }
            catch (const std::exception& e) {
                return -1;
            }
        }
};

class HTTPResponse
{
    private:
        int statusCode;
        std::string statusMessage;
        int contentLength;
        std::string contentType;
        std::string body;
    public:
        void setStatusCode(int statusCode){ this->statusCode = statusCode;}
        void setStatusMessage(const std::string& message){ this->statusMessage = message;}
        void setContentLength(int contentLength){ this->contentLength = contentLength;}
        void setContentType(const std::string& type){ this->contentType = type;}
        void setBody(const std::string& body){ this->body = body;}
        std::string getBody() { return this->body;}

        std::string getHttpMessage(){
            std::string message = format<std::string>("HTTP/1.0 %s %s\r\nContent-Length: %d\r\nContent-Type: %s\r\n\r\n", std::to_string(this->statusCode), this->statusMessage,this->contentType,this->contentLength);
            return message;
        }
        std::vector<int8> encode(std::string& output){
            std::string message = this->getHttpMessage();
            std::vector<int8> byteCode;
            for(char value: message)
            {
                byteCode.push_back((int8)value);
            }
            return byteCode;
        } 
        void decode(std::vector<int8> data){
            //iterate line by line
            std::string line;
            bool line_end1 = false, line_end2 = false, end_header = false;
            for(size_t itr = 0; itr < data.size(); ++itr){
                if(data[itr] == 13){ line_end1 = true;}
                if(data[itr] == 10 && line_end1){ line_end2 = true;}
                else{ line_end1 = false;}
                if(data[itr] == 13 && line_end2){ end_header = true;}
                else{ line_end2=false;}
                if(data[itr] == 10 && end_header){
                    std::cout << "End message decoding" << std::endl;
                    std::string body;
                    for(size_t itr2 = itr+1; itr2 < data.size(); ++itr2){
                        body += (char) data[itr2];
                    }
                    this->setBody(body);
                    break;
                }
                else{ end_header=false;}
                if(line_end2 && !end_header){
                    //parse line
                    std::vector<std::string> arguments;
                    std::istringstream ss(line);
                    std::string token;
                    while(std::getline(ss, token, ' ')) {
                        arguments.push_back(token);
                    }
                    if(arguments[0] == "HTTP/1.0"){
                        this->setStatusCode(std::stoi(arguments[1]));
                        this->setStatusMessage(arguments[2]);
                    }
                    else if(arguments[0] == "Content-Length:"){ this->setContentLength(std::stoi(arguments[1]));}
                    else if(arguments[0] == "Content-Type:"){ this->setContentType(arguments[1]);}
                    line = "";
                }
                if(!line_end1){ line += (char) data[itr];}
            }
        }
};