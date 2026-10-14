#include "MemoryForensicsEvasion.h"
#include "Logger.h"

bool MemoryForensicsEvasion::ProtectAgainstMemoryDump() {
    Logger::Info("Protegendo contra memory dump...");

    // Allocate sensitive memory with NOACCESS protection
    LPVOID pSensitive = VirtualAlloc(
        NULL,
        4096,
        MEM_COMMIT | MEM_RESERVE,
        PAGE_NOACCESS
    );

    if (!pSensitive) {
        Logger::Error("Falha ao alocar memória protegida");
        return false;
    }

    // To use: temporarily change protection
    // VirtualProtect(pSensitive, 4096, PAGE_READWRITE, &oldProtect);
    // ... use ...
    // VirtualProtect(pSensitive, 4096, PAGE_NOACCESS, &oldProtect);

    Logger::Success("Memória sensível alocada com proteção NOACCESS");
    return true;
}

bool MemoryForensicsEvasion::EncryptSensitiveMemory(LPVOID address, SIZE_T size) {
    if (!address || size == 0) return false;

    Logger::Info("Encriptando memória sensível...");

    // Simplified: XOR encryption with random key
    BYTE key = (BYTE)(rand() % 256);
    BYTE* pData = (BYTE*)address;

    for (SIZE_T i = 0; i < size; i++) {
        pData[i] ^= key;
    }

    // In real impl: store key securely (HSM, TPM, or derived from system data)

    Logger::Success("Memória encriptada com chave: 0x" +
                   std::to_string((DWORD)key));
    return true;
}

bool MemoryForensicsEvasion::DecryptSensitiveMemory(LPVOID address, SIZE_T size) {
    if (!address || size == 0) return false;

    Logger::Info("Descriptografando memória sensível...");

    // Simplified: XOR is symmetric
    // In real impl: retrieve key from secure location

    BYTE key = 0x42;  // Example key (would be retrieved)
    BYTE* pData = (BYTE*)address;

    for (SIZE_T i = 0; i < size; i++) {
        pData[i] ^= key;
    }

    Logger::Success("Memória descriptografada");
    return true;
}

std::string MemoryForensicsEvasion::ObfuscateString(const std::string& original) {
    // Simple XOR obfuscation
    BYTE key = 0x42;
    std::string obfuscated;

    for (char c : original) {
        obfuscated += (char)(c ^ key);
    }

    return obfuscated;
}

std::string MemoryForensicsEvasion::DeobfuscateString(const std::string& obfuscated) {
    // XOR is symmetric
    return ObfuscateString(obfuscated);
}
