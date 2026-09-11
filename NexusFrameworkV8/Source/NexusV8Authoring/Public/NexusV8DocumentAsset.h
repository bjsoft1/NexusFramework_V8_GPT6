#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Domain/NexusDomain.h"
#include "NexusV8DocumentAsset.generated.h"

/** Native transaction payload; source geometry is deliberately absent. */
UCLASS(BlueprintType)
class NEXUSV8AUTHORING_API UNexusV8DocumentAsset : public UDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(VisibleAnywhere, Category="Nexus V8|Document")
    TArray<uint8> DocumentBytes;

    bool Read(nexus::Document& Out, FString& Error) const;
    bool Write(const nexus::Document& In, FString& Error);
};
