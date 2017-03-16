#include "SpeechInterface.h"
#include "http_request.h"

#if _debug
#define DBG(x, ...)  printf("[SPEECHINTERFACE: DBG] %s \t[%s,%d]\r\n", x, ##__VA_ARGS__, __FILE__, __LINE__); 
#define WARN(x, ...) printf("[SPEECHINTERFACE: WARN] %s \t[%s,%d]\r\n", x, ##__VA_ARGS__, __FILE__, __LINE__); 
#define ERR(x, ...)  printf("[SPEECHINTERFACE: ERR] %s \t[%s,%d]\r\n", x, ##__VA_ARGS__, __FILE__, __LINE__); 
#endif

#define GUID_SIZE 36
#define SPEECH_RECOGNITION_API_REQUEST_URL  ""                                                                  \
                                            "https://speech.platform.bing.com/recognize?"                       \
                                            "scenarios=smd&appid=D4D52672-91D7-4C74-8AD8-42B1D98141A5"          \
                                            "&locale=en-us&device.os=bot"                                       \
                                            "&form=BCSSTT&version=3.0&format=json&instanceid=%s&requestid=%s"

const char GUID_GENERATOR_HTTP_REQUEST_URL[] = "http://www.fileformat.info/tool/guid.htm?count=1&format=text&hyphen=true";
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

SpeechInterface::SpeechInterface(NetworkInterface * networkInterface, const char * subscriptionKey, bool debug)
{
    _wifi = networkInterface;
    memcpy(_cognitiveSubKey, subscriptionKey, 33);
    _debug = debug;
}

SpeechInterface::~SpeechInterface(void)
{

}

int SpeechInterface::generateGuidStr(char * guidStr)
{
    if (guidStr == NULL)
    {
        return -1;
    }

    HttpRequest* guidRequest = new HttpRequest(_wifi, HTTP_GET, GUID_GENERATOR_HTTP_REQUEST_URL);
    HttpResponse * response = guidRequest->send();
    if (!response)
    {
        printf("Guid generator HTTP request failed.\r\n");
        return -1;
    }
    string guid = response->get_body();
    strcpy(guidStr, guid.c_str());

    printf("Got new guid: %s \r\n", guidStr);
    return strlen(guidStr);
}

int SpeechInterface::recognizeSpeech(char * audioFileBinary, int length, char * text, int textLen)
{
    return 0;
}

int SpeechInterface::convertTextToSpeech(char * text, int length, char * audioFileBinary, int audioLen)
{
    return 0;
}


int SpeechInterface::getJwtToken(char * token, int tokenLen)
{
    return 0;
}
