#pragma once
#include <windows.h>
#include <string>
#include "AntiReverseEngineering.h"
#include "HookEvasion.h"
#include "BehavioralEvasion.h"
#include "MemoryForensicsEvasion.h"
#include "ArtifactElimination.h"
#include "StringEncryption.h"

class EnterpriseInjector {
private:
    bool evasionEnabled = true;
    bool cloakingEnabled = true;
    bool cleanupEnabled = true;

public:
    EnterpriseInjector();

    // Main injection function with full evasion
    bool InjectWithFullEvasion(DWORD pid, const std::string& dllPath);

    // Enable/disable evasion features
    void SetEvasionEnabled(bool enabled) { evasionEnabled = enabled; }
    void SetCloakingEnabled(bool enabled) { cloakingEnabled = enabled; }
    void SetCleanupEnabled(bool enabled) { cleanupEnabled = enabled; }

    // Individual evasion steps
    bool PreInjectDetection();
    bool PerformStealthInjection(DWORD pid, const std::string& dllPath);
    bool PostInjectCleanup();
};
