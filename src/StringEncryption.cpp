#include "StringEncryption.h"
#include <algorithm>
#include <cctype>

std::string StringEncryption::XOREncrypt(const std::string& text, BYTE key) {
    std::string encrypted;
    for (char c : text) {
        encrypted += (c ^ key);
    }
    return encrypted;
}

std::string StringEncryption::XORDecrypt(const std::string& encrypted, BYTE key) {
    return XOREncrypt(encrypted, key);
}

std::string StringEncryption::ROT13(const std::string& text) {
    std::string result;
    for (char c : text) {
        if (c >= 'a' && c <= 'z') {
            result += (char)((c - 'a' + 13) % 26 + 'a');
        } else if (c >= 'A' && c <= 'Z') {
            result += (char)((c - 'A' + 13) % 26 + 'A');
        } else {
            result += c;
        }
    }
    return result;
}

static const char BASE64_TABLE[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

std::string StringEncryption::Base64Encode(const std::string& text) {
    std::string encoded;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];
    int i = 0;

    for (size_t n = 0; n < text.length(); n++) {
        char_array_3[i++] = text[n];

        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = (char_array_3[2] & 0x3f);

            for (i = 0; i < 4; i++)
                encoded += BASE64_TABLE[char_array_4[i]];
            i = 0;
        }
    }

    if (i) {
        for (int j = i; j < 3; j++)
            char_array_3[j] = '\0';

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);

        for (int j = 0; j <= i; j++)
            encoded += BASE64_TABLE[char_array_4[j]];

        while (i++ < 3)
            encoded += '=';
    }

    return encoded;
}

std::string StringEncryption::Base64Decode(const std::string& encoded) {
    std::string decoded;
    std::vector<int> T(256, -1);

    for (int i = 0; i < 64; i++) T[BASE64_TABLE[i]] = i;

    int val = 0, valb = -6;

    for (unsigned char c : encoded) {
        if (T[c] == -1) break;
        val = (val << 6) + T[c];
        valb += 6;

        if (valb >= 0) {
            decoded.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }

    return decoded;
}

DWORD StringEncryption::HashString(const std::string& text) {
    DWORD hash = 5381;
    for (char c : text) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}
