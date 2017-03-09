#ifndef _SASTOKEN_H_
#define _SASTOKEN_H_

#include <string.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <SPI.h>
#include "utils/cdecode.h"
#include "utils/cencode.h"
#include "sha.h"

#include "config.h"

class SASToken {
  char value[1024];
  int expire;
  const int timeToLive;

  void renew(int currentTime);
  void createSASToken(char *serverName, char *deviceId, char *accessKey,
                      int expire, char *output);

 public:
  SASToken(const int ttl = 120);
  ~SASToken();
  const char *getValue(int currentTime);
  const int getExpire() { return expire; };
  void print();
};

#endif
