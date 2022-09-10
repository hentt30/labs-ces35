#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <stdlib.h>

#include "http.h"

typedef uint8_t int8;

template<typename ... Args>
std::string format(const std::string& formatString, Args ... args)
{
    int size_s = std::snprintf(nullptr,0,formatString.c_str(),args ...) + 1; // get the size of the formatting string and dont store it nowhere
    if(size_s <= 0){ throw std::runtime_error("Error during format");}// if the formating was wrong, throw an exception
    auto size = static_cast<size_t>(size_s); // resolve the cast at compile time
    std::unique_ptr<char> buf(new char[size]); // create an pointer to an buffer that will store the format string
    std::snprintf( buf.get(), size, formatString.c_str(), args ...); // now execute the snprintf to store the formatted string in the buffer
    return std::string(buf.get(), size -1 +buf.get()); // construct a new string with the pointer generated above
}

void HTTPRequest::setPath(const std::string& path){ if(path == "/"){this->path = "/index.html";}else{this->path = path;}}
std::string HTTPRequest::getPath(){ return this->path;}
void HTTPRequest::setMethod(const std::string& method){ this->method = method;}
void HTTPRequest::setHost(const std::string& host){ this->host = host;}
        
std::string HTTPRequest::getHttpMessage(){
    std::string message = this->method +" "+this->path+" HTTP/1.0\r\nHost:"+this->host+"\r\n\r\n";
    return message;
}
std::vector<int8> HTTPRequest::encode(){
    std::string message = this->getHttpMessage();
    std::vector<int8> byteCode;
    for(char value: message)
    {
        byteCode.push_back((int8)value);
    }
    return byteCode;
} 

int HTTPRequest::decode(std::vector<int8> data){
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

void HTTPResponse::setStatusCode(int statusCode){ this->statusCode = statusCode;}
void HTTPResponse::setStatusMessage(const std::string& message){ this->statusMessage = message;}
void HTTPResponse::setContentLength(int contentLength){ this->contentLength = contentLength;}
void HTTPResponse::setContentType(const std::string& type){ this->contentType = type;}
void HTTPResponse::setBody(const std::string& body){ this->body = body;}
void HTTPResponse::setHttpVersion(const std::string& httpVersion){ this->httpVersion = httpVersion;}
std::string HTTPResponse::getBody() { return this->body;}

std::string HTTPResponse::getHttpMessage(){
    std::string message = this->httpVersion+" " +std::to_string(this->statusCode)+" "+this->statusMessage+"\r\nContent-Length: "+std::to_string(this->contentLength)+"\r\nContent-Type: "+this->contentType+"\r\n\r\n"+ this->body;
    return message;
}
std::vector<int8> HTTPResponse::encode(){
    std::string message = this->getHttpMessage();
    std::vector<int8> byteCode;
    for(char value: message)
    {
        byteCode.push_back((int8)value);
    }
    return byteCode;
} 
void HTTPResponse::decode(std::vector<int8> data){
    //iterate line by line
    std::string line;
    int count = 0;
    for(size_t itr = 0; itr < data.size(); ++itr){
        if(data[itr] == 13 || data[itr] == 10){++count;}
        else {count = std::max(count-1,0);}

        if(count == 4){
            std::string body;
            for(size_t itr2 = itr+1; itr2 < data.size(); ++itr2){
                body += (char) data[itr2];
            }
            this->setBody(body);
            break;
        }
        else if(count==2){
            //parse line
            std::vector<std::string> arguments;
            std::istringstream ss(line);
            std::string token;
            while(std::getline(ss, token, ' ')) {
                arguments.push_back(token);
            }
            if(arguments[0] == "HTTP/1.0"||arguments[0] == "HTTP/1.1"){
                this->setHttpVersion(arguments[0]);
                this->setStatusCode(std::stoi(arguments[1]));
                this->setStatusMessage(arguments[2]);
            }
            else if(arguments[0] == "Content-Length:"){ this->setContentLength(std::stoi(arguments[1]));}
            else if(arguments[0] == "Content-Type:"){ this->setContentType(arguments[1]);}
            line = "";
        }
        else{ line += (char) data[itr];}
    }
}
