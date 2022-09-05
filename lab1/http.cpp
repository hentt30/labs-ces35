#include <string>
#include <iostream>
#include <vector>

typedef uint8_t int8;

class HTTPRequest
{
    private:
        std::string url;
        std::string method;
        int statusCode;
    public:
        void setUrl(const std::string& url){ this->url = url;}
        void setMethod(const std::string& method){  this->method = method; }
        void setStatusCode(int statusCode){ this->statusCode = statusCode;}
        std::vector<int8> encode(std::string& output){
            std::vector<int8> byteCode;
            return byteCode;
        } 
        void decode(std::vector<int8> data){
            
        }


};

class HTTPResponse
{
    public:
        int a;
};