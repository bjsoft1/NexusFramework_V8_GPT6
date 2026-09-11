#pragma once
#include "CoreMinimal.h"
#include "Domain/NexusDomain.h"
class UWorld;
/** Read-only scanner. Never calls Modify, Rename, UpdateSplinePoints or Landscape writes. */
class NEXUSV8EDITOR_API FNexusV8SourceReader
{
public:
    static nexus::SourceScan Scan(UWorld* World, bool bSelectedActorsOnly, nexus::Issues& Issues);
};
