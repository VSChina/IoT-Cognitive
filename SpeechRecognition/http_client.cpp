#include "http_client.h"
#include "http_request.h"
#include "https_request.h"

extern NetworkInterface *network;

const char CERT[] = 
"-----BEGIN CERTIFICATE-----\r\nMIIDdzCCAl+gAwIBAgIEAgAAuTANBgkqhkiG9w0BAQUFADBaMQswCQYDVQQGEwJJ\r\n"
"RTESMBAGA1UEChMJQmFsdGltb3JlMRMwEQYDVQQLEwpDeWJlclRydXN0MSIwIAYD\r\nVQQDExlCYWx0aW1vcmUgQ3liZXJUcnVzdCBSb290MB4XDTAwMDUxMjE4NDYwMFoX\r\n"
"DTI1MDUxMjIzNTkwMFowWjELMAkGA1UEBhMCSUUxEjAQBgNVBAoTCUJhbHRpbW9y\r\nZTETMBEGA1UECxMKQ3liZXJUcnVzdDEiMCAGA1UEAxMZQmFsdGltb3JlIEN5YmVy\r\n"
"VHJ1c3QgUm9vdDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKMEuyKr\r\nmD1X6CZymrV51Cni4eiVgLGw41uOKymaZN+hXe2wCQVt2yguzmKiYv60iNoS6zjr\r\n"
"IZ3AQSsBUnuId9Mcj8e6uYi1agnnc+gRQKfRzMpijS3ljwumUNKoUMMo6vWrJYeK\r\nmpYcqWe4PwzV9/lSEy/CG9VwcPCPwBLKBsua4dnKM3p31vjsufFoREJIE9LAwqSu\r\n"
"XmD+tqYF/LTdB1kC1FkYmGP1pWPgkAx9XbIGevOF6uvUA65ehD5f/xXtabz5OTZy\r\ndc93Uk3zyZAsuT3lySNTPx8kmCFcB5kpvcY67Oduhjprl3RjM71oGDHweI12v/ye\r\n"
"jl0qhqdNkNwnGjkCAwEAAaNFMEMwHQYDVR0OBBYEFOWdWTCCR1jMrPoIVDaGezq1\r\nBE3wMBIGA1UdEwEB/wQIMAYBAf8CAQMwDgYDVR0PAQH/BAQDAgEGMA0GCSqGSIb3\r\n"
"DQEBBQUAA4IBAQCFDF2O5G9RaEIFoN27TyclhAO992T9Ldcw46QQF+vaKSm2eT92\r\n9hkTI7gQCvlYpNRhcL0EYWoSihfVCr3FvDB81ukMJY2GQE/szKN+OMY3EU/t3Wgx\r\n"
"jkzSswF07r51XgdIGn9w/xZchMB5hbgF/X++ZRGjD8ACtPhSNzkE1akxehi/oCr0\r\nEpn3o0WC4zxe9Z2etciefC7IpJ5OCBRLbf1wbWsaY71k5h+3zvDyny67G7fyUIhz\r\n"
"ksLi4xaNmjICq44Y3ekQEe5+NauQrz4wlHrQMz2nZQ/1/I6eYs9HRCwBXbsdtTLS\r\nR9I4LtD+gdwyah617jzV/OeBHRnDJELqYzmp\r\n-----END CERTIFICATE-----\r\n";

HTTPClient::HTTPClient(http_method method, const char* url, Callback<void(const char *at, size_t length)> body_callback) {
    _method = method;
    _cert =CERT;
    _url = url;
    _body_callback = body_callback;
    _size = 1;
    _header_num = 0;
    _headers = (char**)malloc(2 * _size * sizeof(char *));
}

HTTPClient::HTTPClient(const char* ssl_ca_pem, http_method method, const char* url, Callback<void(const char *at, size_t length)> body_callback) {
    _method = method;
    _cert =ssl_ca_pem;
    _url = url;
    _body_callback = body_callback;
    _size = 1;
    _header_num = 0;
    _headers = (char**)malloc(2 * _size * sizeof(char*));
}

HTTPClient::~HTTPClient() {
    for (int i = 0; i < _header_num * 2; ++i) {
        free(_headers[i]);
    }
    free(_headers);
}

Http_Response* HTTPClient::send(const void* body, int body_size) {
    if (strlen(_url) >= 5 && (strncmp("http:", _url, 5) == 0)) {
        HttpRequest request = HttpRequest(network, _method, _url);
        for (int i = 0; i < _header_num; ++i) {
            request.set_header(_headers[i * 2], _headers[i * 2 + 1]);
        }
        HttpResponse* httpresponse = request.send(body, body_size);
        if (httpresponse == NULL) {
            error = request.get_error();
            return NULL;
        }
        Http_Response* response = new Http_Response();
        response->status_code = httpresponse -> get_status_code();
        response->status_message = (char *)malloc(httpresponse -> get_status_message().length() + 1);
        strcpy (response->status_message, httpresponse->get_status_message().c_str());
        response->body = (char *)malloc(httpresponse->get_body().length() + 1);
        strcpy(response->body, (httpresponse->get_body().c_str()));
        delete httpresponse;
        return response;
    }
    if (strlen(_url) >= 6 && (strncmp("https:", _url, 6) == 0)) {
        HttpsRequest* request = new HttpsRequest(network, _cert, _method, _url);
        for (int i = 0; i < _header_num; ++i) {
            request->set_header(_headers[i * 2], _headers[i * 2 + 1]);
        }
        HttpResponse* httpresponse = request->send(body, body_size);
        if (httpresponse == NULL) {
            error = request->get_error();
            return NULL;
        }
        Http_Response* response = new Http_Response();
        response->status_code = httpresponse -> get_status_code();
        response->status_message = (char *)malloc(httpresponse -> get_status_message().length() + 1);
        strcpy (response->status_message, httpresponse->get_status_message().c_str());
        response->body = (char *)malloc(httpresponse->get_body().length() + 1);
        strcpy(response->body, (httpresponse->get_body().c_str()));
        delete request;
        delete httpresponse;
        return response;
    }
    return NULL;
}

void HTTPClient::set_header(const char* key, const char* value) {
    if (_header_num == _size) {
        char** newBuf = (char**)malloc(4 * _size * sizeof(char*));
        for (int i = 0; i < _size * 2; ++i) {
            newBuf[i] = _headers[i];
        }
        _size = _size * 2;
        free(_headers);
        _headers = newBuf;
    }
    _headers[_header_num * 2] = (char *)malloc(strlen(key) + 1); 
    strcpy(_headers[_header_num * 2], key);
    _headers[_header_num * 2 + 1] = (char *)malloc(strlen(value) + 1);
    strcpy(_headers[_header_num * 2 + 1], value);
    _header_num++;
}

nsapi_error_t HTTPClient::get_error() {
    return error;
}