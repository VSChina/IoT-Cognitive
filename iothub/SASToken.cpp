#include "SASToken.h"

void urlEncode(char *in, char *out) {
  char *hex = "0123456789abcdef";
  char *src = in;
  char *dst = out;
  char c;
  while (c = *src++) {
    if (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') ||
        ('0' <= c && c <= '9')) {
      *dst++ = c;
    } else {
      *dst++ = '%';
      *dst++ = hex[c >> 4];
      *dst++ = hex[c & 15];
    }
  }
  *dst = 0;
};

SASToken::SASToken(const int ttl) : timeToLive(ttl), expire(0) {
  *value = 0;
  //   renew();
};

SASToken::~SASToken(){};

void SASToken::createSASToken(char *serverName, char *deviceId, char *accessKey,
                              int expireTime, char *output) {
  // payload = serverName + \n + deviceID
  char payload[64];
  sprintf(payload, "%s/devices/%s\n%d", serverName, deviceId, expireTime);

  // key: base64_decode accessKey
  char decodedKey[SHA256HashSize];
  base64_decodestate b64_dec;
  base64_init_decodestate(&b64_dec);
  int dec_len = (ssize_t)base64_decode_block(accessKey, strlen(accessKey),
                                             decodedKey, &b64_dec);

  // sig: hmac_sha256(key, text)
  uint8_t sigRaw[SHA256HashSize];
  hmac(SHA256, (const unsigned char *)payload, strlen(payload),
       (const unsigned char *)decodedKey, SHA256HashSize, (uint8_t *)sigRaw);

  // sigRaw --(base64_encode)-> b64Sig:   maxSize = 4/3 * oldSize
  char b64Sig[(SHA256HashSize / 3 + SHA256HashSize % 3) * 4];
  base64_encodestate b64_enc;
  base64_init_encodestate(&b64_enc);
  int res =
      base64_encode_block((char *)sigRaw, SHA256HashSize, b64Sig, &b64_enc);
  int res2 = base64_encode_blockend(&b64Sig[res], &b64_enc);
  b64Sig[res + res2 - 1] = '\0';

  // b64Sig --(urlencode)-> urlB64Sig:   maxSize = 3 * oldSize
  char urlB64Sig[128 * 3];
  urlEncode(b64Sig, urlB64Sig);

  // assemble SAS Token --> output
  sprintf(output, "SharedAccessSignature sr=%s/devices/%s&sig=%s&se=%d",
          serverName, deviceId, urlB64Sig, expireTime);
};

void SASToken::renew(int currentTime) {
  expire = currentTime + timeToLive;
  createSASToken(IOTHUB_HOST, DEVICE_ID, SHARED_ACCESS_KEY, expire, value);
};

const char *SASToken::getValue(int currentTime) {
  print();
  if (currentTime + 30 > expire) {
    renew(currentTime);
  }
  return value;
};

void SASToken::print() {
  printf("=SAS=\nv:%s\ne:%d\nttl:%d\nct:\n===\n", value, expire, timeToLive);
};