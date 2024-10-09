// Copyright by CodeParrots 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ActorActionUtility.h"
#include "AsyncTask_Text2Img_AI.h"
#include "SDAPI_ActorActionUtility.generated.h"

/**
 * 
 */
UCLASS()
class USDAPI_ActorActionUtility : public UActorActionUtility
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintReadWrite, Category = "SD_API")
	FString MaterialDefaultParam;

	/** Return the class that this actor action supports. Leave this blank to support all actor classes. */
// 	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Assets")
// 	UTexture2D* MakeAITexture(FString Prompt, FString NegaitvePrompt, FSDAPI_Parameters Param);
// 

};
