#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include <sstream>
#include <stdexcept>

typedef uint8_t int8;

class HTTPRequest
{
    private:
        std::string url;
        std::string method;
        std::string host;
    public:
        void setUrl(const std::string& url){ this->url = url;}
        void setMethod(const std::string& method){  this->method = method; }
        void setHost(const std::string& host){ this->host = host; }
        
        template<typename ... Args>
        std::string format(const std::string& formatString, Args ... args)
        {
            int size_s = std::snprintf(nullptr,0,format.c_str(),args ...) + 1; // get the size of the formatting string and dont store it nowhere
            if(size_s <= 0){ throw std::runtime_error("Error during format");}// if the formating was wrong, throw an exception
            size_t size = static_cast<size_t>(size_s); // resolve the cast at compile time
            std::unique_ptr<char[]> buf(new char[size]); // create an pointer to an buffer that will store the format string
            std::snprintf( buf.get(), size, format.c_str(), args ...); // now execute the snprintf to store the formatted string in the buffer
            return std::string(buf.get(),buf.get()+size-1); // contruct a new string with the pointer generated above
        }
        std::string getHttpMessage(){
            std::string message = this->format<std::string>("%s %s HTTP/1.0\r\nHost:%s\r\nContent-Type: text/html\r\n\r\n",this->method,this->url,this->host);
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
            size_t dataItr = 0;
            int count = 0;
            //iterate line by line
            std::string line;
            for(size_t itr = 0; itr < data.size(); ++itr){
                if(data[itr] == 10 || data[itr] == 13){count += 1;}
                else{count = std::max(count -1, 0);}
                if(count > 2){
                    std::cout << "End of the message" << std::endl;
                    break;
                }
                else if(count == 2){
                    //parse line
                    std::vector<std::string> arguments;
                    std::istringstream ss(line);
                    std::string token;
                    while(std::getline(ss, token, ' ')) {
                        arguments.push_back(token);
                    }
                    if(arguments[0] == "GET"){
                        this->setMethod(arguments[0]);
                        this->setUrl(arguments[1]);
                    }
                    else if(arguments[0] == "Host:"){   this->setHost(arguments[1]); }
                    line = "";
                }
                line += (char) data[itr];
            }
        }
};

class HTTPResponse
{
    private:
        std::string method;
        int statusCode;
        int contentLength;
    public:
        void setMethod(const std::string& method){  this->method = method; }
        void setStatusCode(int statusCode){ this->statusCode = statusCode;}
        std::vector<int8> encode(std::string& output){
            std::vector<int8> byteCode;
            return byteCode;
        } 
        void decode(std::vector<int8> data){
            
        }
};