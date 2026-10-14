#pragma once
#include <windows.h>

class ArtifactElimination {
public:
    // Clean up all artifacts
    static bool CleanupAllArtifacts();

    // Remove injected modules
    static bool CleanupInjectedModules();

    // Sanitize stack trace
    static bool SanitizeStackTrace();

    // Clear event logs
    static bool ClearEventLogs();

    // Remove temporary files
    static bool RemoveTemporaryFiles();
};
