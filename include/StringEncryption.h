#pragma once
#include <windows.h>
#include <string>
#include <vector>

class StringEncryption {
public:
    // XOR encryption
    static std::string XOREncrypt(const std::string& text, BYTE key);
    static std::string XORDecrypt(const std::string& encrypted, BYTE key);

    // ROT13
    static std::string ROT13(const std::string& text);

    // Base64
    static std::string Base64Encode(const std::string& text);
    static std::string Base64Decode(const std::string& encoded);

    // Simple hash
    static DWORD HashString(const std::string& text);
};
