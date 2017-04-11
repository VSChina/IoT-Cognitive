#ifndef __HTTP_CLIENT_H__
#define __HTTP_CLIENT_H__

#include "mbed.h"
#include "http_parser.h"

typedef struct
{
    int status_code;
    char* status_message;
    char* body;
} Http_Response;

class HTTPClient {
    public:
        HTTPClient(http_method method, const char* url, Callback<void(const char *at, size_t length)> body_callback = 0);
        HTTPClient(const char* ssl_ca_pem, http_method method, const char* url,Callback<void(const char *at, size_t length)> body_callback = 0);
        virtual ~HTTPClient(void);
        Http_Response* send(const void* body = NULL, int body_size = 0);
        void set_header(const char* key, const char* value);
        nsapi_error_t get_error();
        
    private:
        const char * _cert;
        http_method _method;
        const char * _url;
        char** _headers;
        int _size;
        int _header_num;
        Callback<void(const char *at, size_t length)> _body_callback;
        nsapi_error_t error;
};

#endif