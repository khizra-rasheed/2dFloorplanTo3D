// Copyright by CodeParrots 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Interfaces/IHttpRequest.h"
#include "SDAI_Subsystem.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "SDAPI_DeveloperSettings.h"
#include "AsyncTask_Text2Img_AI.generated.h"


class UTexture2DDynamic;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSDA_TextToImgDel,const TArray<UTexture2DDynamic*>&, Textures);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSDA_TextToImgDel2,const TArray<UTexture2D*>&, Textures);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSDA_SetModelDel, FSDAPI_OptionDataStruct, SubSystemOptions);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSDA_GetLoraInfoDel, FSDAPI_Response_LoraInfoData, LoraInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FSDA_GetModelsDel, FSDAPI_OptionDataStruct,SubSystemOptions, FSDAPI_Response_ModelListData, CheckPointDatas, FSDAPI_Controlnet_ModelList, ControlnetModelNames);


/**
 * 
 */
UCLASS()
class STABLEDIFFUSIONSUBSYSTEM_API UAsyncTask_SDAPI_Text2Img : public UBlueprintAsyncActionBase
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UAsyncTask_SDAPI_Text2Img* SDAPI_Text2Img(FString URL, FString Prompts, FString Negative_Prompt, FString TargetModelName = "", FSDAPI_Parameters SubParam = FSDAPI_Parameters(), FSDAPI_Controlnet_Parameters Controlent_Param = FSDAPI_Controlnet_Parameters());
	
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UAsyncTask_SDAPI_Text2Img* SDAPI_Text2Img_BySubSystem(USDAI_Subsystem* SubSystem, FString Prompts, FString Negative_Prompt, FString TargetModelName, FSDAPI_Parameters SubParam, FSDAPI_Controlnet_Parameters Controlent_Param = FSDAPI_Controlnet_Parameters(), bool bUseLCMMode = false, bool bUseLCMDefaultParam = false);

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UAsyncTask_SDAPI_Text2Img* SDAPI_Img2Img(FString TextureEncoded, FString URL, FString Prompts, FString Negative_Prompt, FString TargetModelName = "", FSDAPI_Parameters SubParam = FSDAPI_Parameters(), FSDAPI_Controlnet_Parameters Controlent_Param = FSDAPI_Controlnet_Parameters());

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UAsyncTask_SDAPI_Text2Img* SDAPI_Img2Img_BySubSystem(UTexture2D* Texture, USDAI_Subsystem* SubSystem, FString Prompts, FString Negative_Prompt, FString TargetModelName, FSDAPI_Parameters SubParam, FSDAPI_Controlnet_Parameters Controlent_Param = FSDAPI_Controlnet_Parameters(), bool bUseLCMMode = false, bool bUseLCMDefaultParam = false);

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UAsyncTask_SDAPI_Text2Img* SDAPI_RenderTarget2Img(UTextureRenderTarget2D* Texture, FString URL, FString Prompts, FString Negative_Prompt, FString TargetModelName = "", FSDAPI_Parameters SubParam = FSDAPI_Parameters(), FSDAPI_Controlnet_Parameters Controlent_Param = FSDAPI_Controlnet_Parameters());

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UAsyncTask_SDAPI_Text2Img* SDAPI_RenderTarget2Img_BySubSystem(UTextureRenderTarget2D* Texture, USDAI_Subsystem* SubSystem, FString Prompts, FString Negative_Prompt, FString TargetModelName, FSDAPI_Parameters SubParam, FSDAPI_Controlnet_Parameters Controlent_Param = FSDAPI_Controlnet_Parameters(), bool bUseLCMMode = false, bool bUseLCMDefaultParam = false);

	UFUNCTION()
	void UpdateProgressImage(FString URL);

	UFUNCTION()
	void RequestUpdateProgressImage(FString URL);


	int32 Count = 0;
public:

	UPROPERTY(BlueprintAssignable)
	FSDA_TextToImgDel2 Success;

	UPROPERTY(BlueprintAssignable)
	FSDA_TextToImgDel2 Fail;

	UPROPERTY(BlueprintAssignable)
	FSDA_TextToImgDel2 ProgressImage;

protected:

	FTimerHandle ProgressCheckTimer;

	bool OnGetResponed = false;

	FString SavedURLForInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,meta = (ClampMin = "0", ClampMax = "100"), Category = "SD_API")
	int32 ProgressPercent = 0;
public:

	void Start(FString URL, FString Prompts,FString Negative_Prompt, FString TargetModelName = "", FSDAPI_Parameters SubParam = FSDAPI_Parameters(), FSDAPI_Controlnet_Parameters Controlent_Param = FSDAPI_Controlnet_Parameters());

	void Start(FString TextureEncoded, FString URL, FString Prompts, FString Negative_Prompt, FString TargetModelName = "", FSDAPI_Parameters SubParam = FSDAPI_Parameters(), FSDAPI_Controlnet_Parameters Controlent_Param = FSDAPI_Controlnet_Parameters());

private:


	static void SetLCMSetting(USDAI_Subsystem* SubSystem, FString& Prompt, FSDAPI_Parameters& SubParam, bool bUseLCMMode = false, bool bUseLCMDefaultParam = false);
	/** Handles image requests coming from the web */
	void HandleImageRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	/** Handles image requests coming from the web */
	void HandleProgressImage(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	UTexture2D* CreateTexture2D(const TArray64<uint8>& PixelData, int32 InSizeX, int32 InSizeY, EPixelFormat InFormat, FName BaseName);


	
};

/**
 *
 */
UCLASS()
class STABLEDIFFUSIONSUBSYSTEM_API UAsyncTask_SDAPI_ConnectAndInitialize : public UBlueprintAsyncActionBase
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UAsyncTask_SDAPI_ConnectAndInitialize* SDAPI_ConnectAndInitialize(USDAI_Subsystem* SubSystem, FString URLKey);

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UAsyncTask_SDAPI_ConnectAndInitialize* SDAPI_ConnectAndInitialize_ManualURL(USDAI_Subsystem* SubSystem, FString URL);

public:

	UPROPERTY(BlueprintAssignable)
	FSDA_GetModelsDel OnSuccess;

	UPROPERTY(BlueprintAssignable)
	FSDA_GetModelsDel OnFail;

	
public:

	void Start(USDAI_Subsystem* SubSystem, FString URL);

private:

	USDAI_Subsystem* RecivedSubSystem = nullptr;

	bool RecivedModelList = false;
	bool RecivedLoraList = false;
	bool RecivedConfig = false;
	bool RecivedControlnet = false;
	bool SuccessGetModelList = false;
	bool SuccessGetLoraList = false;
	bool SuccessGetConfig = false;

	FSDAPI_Controlnet_ModelList ControlentModelData;
	FSDAPI_Response_ModelListData ResponseModelData;
	FSDAPI_Response_LoraListData ResponseLoraData;
	FSDAPI_OptionDataStruct ConfigData;

	/** Handles requests coming from the web */
	void HandleConfigRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	void HandleModelListRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	void HandleLoraListRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	void HandleControlnetModelListRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	void CheckConnectionComplete();
};



/// Apply After Next query.
UCLASS()

class STABLEDIFFUSIONSUBSYSTEM_API UAsyncTask_SDAPI_SetModelAndConfig : public UBlueprintAsyncActionBase
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UAsyncTask_SDAPI_SetModelAndConfig* SDAPI_SetModelAndConfig(USDAI_Subsystem* SubSystem, FSDAPI_OptionDataStruct Option);

public:

	UPROPERTY(BlueprintAssignable)
	FSDA_SetModelDel OnSuccess;

	UPROPERTY(BlueprintAssignable)
	FSDA_SetModelDel OnFail;

public:

	FTimerHandle InitializeTimer;

	USDAI_Subsystem* RecivedSubSystem = nullptr;

	void Start(USDAI_Subsystem* SubSystem,  FSDAPI_OptionDataStruct Option);

private:

	/** Handles requests coming from the web */
	void HandleConfigRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
};

/**
 *
 */
UCLASS()
class STABLEDIFFUSIONSUBSYSTEM_API UAsyncTask_SDAPI_GetLoraInfo : public UBlueprintAsyncActionBase
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UAsyncTask_SDAPI_GetLoraInfo* SDAPI_GetLoraInfo(USDAI_Subsystem* SubSystem, FString LoraName);

public:

	UPROPERTY(BlueprintAssignable)
	FSDA_GetLoraInfoDel OnSuccess;

	UPROPERTY(BlueprintAssignable)
	FSDA_GetLoraInfoDel OnFail;

	FSDAPI_Response_LoraInfoData InfoData;

	FString RecivedLoraName = "";
public:

	void GetLoraInfo(USDAI_Subsystem* SubSystem, FString LoraName);
private:

	USDAI_Subsystem* RecivedSubSystem = nullptr;
	void HandleLoraInfoRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
};