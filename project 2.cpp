#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <openssl/evp.h>
#include <iostream>
#include <map>
#include <cmath>
#include <time.h>

using namespace std;

#define MAX_MESSAGE_LENGTH 1024  // 最大消息长度
#define HASH_LENGTH 32           // SM3哈希值长度
#define DIGEST_LENGTH 2          // SM3哈希摘要长度

void calculate_sm3_hash(const unsigned char* input_message, size_t message_len, unsigned char* digest) {
    EVP_MD_CTX* md_ctx = EVP_MD_CTX_new();
    const EVP_MD* md = EVP_sm3();
    EVP_DigestInit_ex(md_ctx, md, NULL);
    EVP_DigestUpdate(md_ctx, input_message, message_len);
    EVP_DigestFinal_ex(md_ctx, digest, NULL);
    EVP_MD_CTX_free(md_ctx);
}

bool are_digests_equal(const unsigned char* digest1, const unsigned char* digest2, int length) {
    return memcmp(digest1, digest2, length) == 0;
}

int main() {
    unsigned char input_message[MAX_MESSAGE_LENGTH] = "1234567890";
    unsigned char input_message2[MAX_MESSAGE_LENGTH] = "1234567890";
    unsigned char digest[DIGEST_LENGTH];
    unsigned char tmp_digest[DIGEST_LENGTH];

    int i = 0;

    calculate_sm3_hash(input_message, strlen((const char*)input_message), digest);
    calculate_sm3_hash(input_message2, strlen((const char*)input_message2), tmp_digest);

    while (++i) {
        if (i % 1024 == 0)
            cout << ".";

        if (i % 16 == 0) {
            memcpy(input_message2, tmp_digest, HASH_LENGTH);
            calculate_sm3_hash(tmp_digest, strlen((const char*)tmp_digest), tmp_digest);
            if (are_digests_equal(digest, tmp_digest, DIGEST_LENGTH))
                break;
        }

        memcpy(input_message, digest, HASH_LENGTH);
        calculate_sm3_hash(digest, strlen((const char*)digest), digest);

        if (are_digests_equal(digest, tmp_digest, DIGEST_LENGTH))
            break;
    }

    cout << endl << "digest:";
    for (int t = 0; t < DIGEST_LENGTH; t++) {
        printf("%02x  ", digest[t]);
    }

    cout << endl << "digest2:";
    for (int t = 0; t < DIGEST_LENGTH; t++) {
        printf("%02x  ", tmp_digest[t]);
    }

    cout << endl << "message1:";
    for (int t = 0; t < HASH_LENGTH; t++) {
        printf("%02x  ", input_message[t]);
    }

    cout << endl << "message2:";
    for (int t = 0; t < HASH_LENGTH; t++) {
        printf("%02x  ", input_message2[t]);
    }

    cout << endl << "hash:";
    for (int t = 0; t < DIGEST_LENGTH; t++) {
        printf("%02x  ", digest[t]);
    }

    return 0;
}
