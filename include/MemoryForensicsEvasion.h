#pragma once
#include <windows.h>
#include <string>

class MemoryForensicsEvasion {
public:
    // Protect against memory dump
    static bool ProtectAgainstMemoryDump();

    // Encrypt sensitive memory
    static bool EncryptSensitiveMemory(LPVOID address, SIZE_T size);

    // Decrypt sensitive memory
    static bool DecryptSensitiveMemory(LPVOID address, SIZE_T size);

    // Obfuscate strings in memory
    static std::string ObfuscateString(const std::string& original);
    static std::string DeobfuscateString(const std::string& obfuscated);
};
