#include <string>
#include <vector>

typedef uint8_t int8;

class HTTPRequest
{
    private:
        std::string url;
        std::string method;
        std::string host;
        template<typename ... Args>
        std::string format(const std::string& formatString, Args ... args);
        std::string getHttpMessage();
    public:
        void setUrl(const std::string& url){ this->url = url;}
        void setMethod(const std::string& method){  this->method = method; }
        void setHost(const std::string& host){ this->host = host; }
        std::vector<int8> encode(std::string& output);
        void decode(std::vector<int8> data);
};

class HTTPResponse
{
    private:
        std::string method;
        int statusCode;
        int contentLength;
    public:
        void setMethod(const std::string& method);
        void setStatusCode(int statusCode);
        std::vector<int8> encode(std::string& output);
        void decode(std::vector<int8> data);
};