
#pragma once
#include "CoreMinimal.h"
#include "GeneratedProto.pb.h"
#include "GeneratedStructs.generated.h"


USTRUCT(BlueprintType,Blueprintable)
struct STNET_API FMyMessage
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere) int32 ID;
    UPROPERTY(BlueprintReadWrite, EditAnywhere) FString Name;
};


