#pragma once
#include "CoreMinimal.h"
#include "Domain/NexusDomain.h"
class AActor;
class UWorld;
/** Owns only transient actors created by this instance. Failed staging leaves old output intact. */
class NEXUSV8COMPILER_API FNexusV8PreviewRenderer
{
public:
    ~FNexusV8PreviewRenderer();
    bool Replace(UWorld* World, const nexus::Document& Document, const nexus::SourceScan& Source,
        const nexus::PreviewSnapshot& Snapshot, bool bCatalogMeshes,
        const TFunction<bool()>& Canceled, FString& Error);
    void Clear();
    void SetStale(bool bStale);
    FString GetHash() const { return Hash; }
private:
    TArray<TWeakObjectPtr<AActor>> Actors;
    FString Hash;
};
