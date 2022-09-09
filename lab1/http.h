#define BUFFER_SIZE 3000
#include <string>
#include <vector>

typedef uint8_t int8;

class HTTPRequest
{
    private:
        std::string path;
        std::string method;
        std::string host;
    public:
        void setPath(const std::string& path);
        std::string getPath();
        void setMethod(const std::string& method);
        void setHost(const std::string& host);
        std::vector<int8> encode();
        int decode(std::vector<int8> data);
        std::string getHttpMessage();
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
        std::string getBody();
        std::vector<int8> encode();
        void decode(std::vector<int8> data);
        std::string getHttpMessage();
};