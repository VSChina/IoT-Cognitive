#include "mbed.h"
#include "NTPClient.h"
#include "SPWFSAInterface5.h"
#include "https_request.h"
#include "http_request.h"
#include "wav.h"

#define DURATION 2
#define SAMPLE_RATE 8000
#define BITS_PER_SAMPLE 8
AnalogIn Micrphone(A0);

Serial pc(USBTX, USBRX, 115200);
SPWFSAInterface5 spwf(D8, D2, false);
const char *ssid = "Ruff_R0101965"; // Ruff_R0101965, laptop_jiaqi
const char *pwd = "Password01!";
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

int file_size = 0;
int setupRealTime(void)
{
    NTPClient ntp(spwf);
    ntp.setTime("be.pool.ntp.org");
    return 0;
}

Ticker ticker;
char * tmp;
int records = 0;

void readMic() {
    if (records >= DURATION * SAMPLE_RATE) return;
    int x = 0;
    //if (records < 16000) x = fake[records];
    if (BITS_PER_SAMPLE == 16) x = Micrphone.read_u16();
    else x = Micrphone.read_u16() / 256;
    memcpy(tmp, &x, BITS_PER_SAMPLE / 8);
    tmp += BITS_PER_SAMPLE / 8;
    records++;
}

char* getWav() {
    WaveHeader * hdr = genericWAVHeader(SAMPLE_RATE, BITS_PER_SAMPLE, 1);
    char* file = recordWAV(hdr, DURATION, &file_size);
    if (file == NULL) {
        printf("Error the wav file did not created \r\n");
        return NULL;
    }
    tmp = file + 44;
    printf("//record\r\n");
    ticker.attach(&readMic, 1.0 / SAMPLE_RATE);
    wait(DURATION);
    printf("//file size %d,  %d \r\n", file_size, tmp - file);
    file[44 + file_size] = 0x0;
    for (int i = 0; i < 45 + file_size; ++i) {
        if (i % 50 == 0) printf("\n");
        printf("%d,", file[i]);
    }
    printf("\n");
    return file;
}

void dump_response(HttpResponse* res) {
    mbedtls_printf("Status: %d - %s\n", res->get_status_code(), res->get_status_message().c_str());

    mbedtls_printf("Headers:\n");
    for (size_t ix = 0; ix < res->get_headers_length(); ix++) {
        mbedtls_printf("\t%s: %s\n", res->get_headers_fields()[ix].c_str(), res->get_headers_values()[ix].c_str());
    }
    mbedtls_printf("\nBody:\n\n%s\n", res->get_body().c_str());
}
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

int main(void)
{
    char * file = getWav();
    printf("Start...\n");
    while(true) {
        printf("Try to connect %s ...\n", ssid);
        if(spwf.connect(ssid, pwd, NSAPI_SECURITY_WPA2) == NSAPI_ERROR_OK) {
            printf("connected %s\r\n", spwf.get_mac_address());
            break;
        }
        else {
            printf("failed.\r\n");
        }
    }
    //setupRealTime();

    HttpRequest* guidRequest = new HttpRequest(&spwf, HTTP_GET, "http://www.fileformat.info/tool/guid.htm?count=1&format=text");
    //req->set_debug(true);
    HttpResponse* response = guidRequest->send();
    if (!response) {
        printf("HttpRequest failed\n");
        return 1;
    }
    string body = response->get_body();
    char *requestUri = (char *)malloc(300);
    sprintf(requestUri, "https://speech.platform.bing.com/recognize?scenarios=smd&appid=D4D52672-91D7-4C74-8AD8-42B1D98141A5&locale=en-us&device.os=bot\
&form=BCSSTT&version=3.0&format=json&instanceid=0E08849D-51AE-4C0E-81CD-21FE3A419868&requestid=%s-%s-%s-%s-%s", 
        body.substr(0, 8).c_str(), body.substr(8, 4).c_str(), body.substr(12, 4).c_str(), body.substr(16, 4).c_str(), body.substr(20, 12).c_str());
    printf("%s\r\n", requestUri);

    HttpsRequest* tokenRequest = new HttpsRequest(&spwf, CERT, HTTP_POST, "https://api.cognitive.microsoft.com/sts/v1.0/issueToken");
    tokenRequest->set_header("Ocp-Apim-Subscription-Key", "a4b1a40cb1f74ab1af757a0e700fa847");
    response = tokenRequest->send();
    if (!tokenRequest) {
        printf("HttpRequest failed (error code %d)\n", tokenRequest->get_error());
        return 1;
    }
    body = response->get_body();
    printf("token<%s>\r\n", body.c_str());
    delete tokenRequest;

    HttpsRequest* speechRequest = new HttpsRequest(&spwf, CERT, HTTP_POST, requestUri);
    speechRequest->set_header("Authorization", "Bearer " + body);
    speechRequest->set_header("Content-Type", "plain/text");
    response = speechRequest->send(file, 45 + file_size);
    body = response->get_body();
    printf("result <%s>\r\n", body.c_str());
    return 0;
}

