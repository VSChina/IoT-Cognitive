
#include "NTPClient.h"
#include "SpeechInterface.h"
#include "http_request.h"
#include "https_request.h"
#include <json.h>

#define SPEECH_RECOGNITION_API_REQUEST_URL  ""                                                                  \
                                            "https://speech.platform.bing.com/recognize?"                       \
                                            "scenarios=smd&appid=D4D52672-91D7-4C74-8AD8-42B1D98141A5"          \
                                            "&locale=en-us&device.os=bot"                                       \
                                            "&form=BCSSTT&version=3.0&format=json&instanceid=%s&requestid=%s"

#define GUID_GENERATOR_HTTP_REQUEST_URL  "http://www.fileformat.info/tool/guid.htm?count=1&format=text&hyphen=true"
#define TOKEN_REQUEST_URL "https://api.cognitive.microsoft.com/sts/v1.0/issueToken"

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

SpeechInterface::SpeechInterface(NetworkInterface * networkInterface, const char * subscriptionKey, const char * deviceId, bool debug)
{
    _wifi = networkInterface;
    _requestUri = (char *)malloc(260);
    _cognitiveSubKey = (char *)malloc(33);
    _deviceId = (char *)malloc(37);

    memcpy(_cognitiveSubKey, subscriptionKey, 33);
    memcpy(_deviceId, deviceId, 37);
    
    _debug = debug;

    printf("subscriptionKey: %s, deviceId: %s \r\n", subscriptionKey, deviceId);
}

SpeechInterface::~SpeechInterface(void)
{  
    delete _cognitiveSubKey;
    delete _deviceId;
    delete _requestUri;
}

int SpeechInterface::generateGuidStr(char * guidStr)
{
        TCPSocket socket;
        printf("will open socket\r\n");
        int open_result = socket.open(_wifi);
        printf("open socket done %d\r\n", open_result);
    if (guidStr == NULL)
    {
        return -1;
    }

    HttpRequest* guidRequest = new HttpRequest(_wifi, HTTP_GET, GUID_GENERATOR_HTTP_REQUEST_URL);
    if (guidRequest == NULL) {
        printf("guidRequest is null\r\n");
    } else {
        printf("guidRequest is not null\r\n");
    }
    HttpResponse* _response = guidRequest->send();
    if (!_response)
    {
        printf("Guid generator HTTP request failed.\r\n");
        return -1;
    }

    strcpy(guidStr, _response->get_body().c_str());   
    printf("Got new guid: %s \r\n", guidStr);
    
    return strlen(guidStr);
}

char* SpeechInterface::getJwtToken()
{    
    HttpsRequest* tokenRequest = new HttpsRequest(_wifi, CERT, HTTP_POST, TOKEN_REQUEST_URL);
    tokenRequest->set_header("Ocp-Apim-Subscription-Key", _cognitiveSubKey);
    HttpResponse* _response = tokenRequest->send();
    if (!_response)
    {
        printf("HttpRequest failed (error code %d)\n", tokenRequest->get_error());
        return NULL;
    }

    string token = _response->get_body();
    printf("Got JWT token: %s\r\n", token.c_str());
    
    delete tokenRequest;  
    return (char *)token.c_str();
}

SpeechResponse* SpeechInterface::recognizeSpeech(char * audioFileBinary, int length)
{
    printf("file length : %d\r\n", length);

    // Generate a new guid for cognitive service API request
    char * guid = (char *)malloc(33);
    if (guid == NULL) {
        printf("guid is null\r\n");
    } 
    generateGuidStr(guid);

    // Generate a JWT token for cognitove service authentication
    string jwtToken = getJwtToken();
    
    // Preapre Speech Recognition API request URL
    sprintf(_requestUri, SPEECH_RECOGNITION_API_REQUEST_URL, _deviceId, guid);
    printf("recognizeSpeech request URL: %s\r\n", _requestUri);

    HttpsRequest* speechRequest = new HttpsRequest(_wifi, CERT, HTTP_POST, _requestUri);
    speechRequest->set_header("Authorization", "Bearer " + jwtToken);
    speechRequest->set_header("Content-Type", "plain/text");
    
    HttpResponse* _response = speechRequest->send(audioFileBinary, length);
    if (!_response)
    {
        printf("Speech API request failed.\r\n");
        return NULL;
    }
    
    string body = _response->get_body();
    printf("congnitive result: %s\r\n", body.c_str());

    char * bodyStr = (char*)body.c_str();

    SpeechResponse *speechResponse = new SpeechResponse();
    if (speechResponse == NULL) {
        printf("SpeechResponse is null \r\n");
    }
    // Parse Jso n result to SpeechResponse object
    struct json_object *responseObj, *subObj, *valueObj, *bestResult;

    responseObj = json_tokener_parse(bodyStr);
    printf("JSON object from response:\n%s\n", json_object_to_json_string_ext(responseObj, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY));

     // parse status value from header->status
    json_object_object_get_ex(responseObj, "header", &subObj);
    json_object_object_get_ex(subObj, "status", &valueObj);
    speechResponse->status =  (char *)json_object_get_string(valueObj);
    printf("status = %s\r\n", speechResponse->status);

    if (strcmp(speechResponse->status, "error") == 0)
    {
        printf("Parse speech error.");
    }
    else
    {
        // parse status value from header->lexical
        json_object_object_get_ex(subObj, "lexical", &valueObj);
        speechResponse->text =  (char *)json_object_get_string(valueObj);
        printf("speech text = %s\r\n", speechResponse->text);

        // parse confidence value from results[0]->confidence
        json_object_object_get_ex(responseObj, "results", &subObj);
        array_list * array = json_object_get_array(valueObj);
        bestResult = json_object_array_get_idx(subObj, 0);
        json_object_object_get_ex(bestResult, "confidence", &valueObj);

        speechResponse->confidence = (double)json_object_get_double(valueObj);
        printf("confidence = %f\r\n", speechResponse->confidence);
    }

    free(guid);
    delete speechRequest;
    
    return speechResponse;
}

int SpeechInterface::convertTextToSpeech(char * text, int length, char * audioFileBinary, int audioLen)
{
    return 0;
}

int SpeechInterface::setupRealTime(void)
{
    NTPClient ntp(*_wifi);
    int result = 0;
    do {
        result = ntp.setTime("0.pool.ntp.org");
    } while (result != 0);
    return result;
}