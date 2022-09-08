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
        std::vector<int8> encode();
        void decode(std::vector<int8> data);
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
        void setStatusCode(int statusCode);
        void setStatusMessage(const std::string& message);
        void setContentLength(int contentLength);
        void setContentType(const std::string& type);
        void setBody(const std::string& body);
        std::vector<int8> encode();
        void decode(std::vector<int8> data);
};