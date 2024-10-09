// Copyright by CodeParrots 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine.h"
#include "Subsystems/EngineSubsystem.h"
#include "SDAI_Subsystem.generated.h"

class USDAPI_DeveloperSettings;

UENUM(BlueprintType)
enum class ESDAPI_SubsystemState : uint8
{
	API_NOT_CONNECTED = 0,
	API_CONNECTED_NOT_INITAILZED =1,
	API_CONNECTED_AND_INITAILZED = 2
};

USTRUCT(BlueprintType)
struct FSDAPI_OptionDataStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SD_API")
	FString sd_model_checkpoint = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SD_API")
	FString sd_vae = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SD_API")
	TArray<FString> hide_samplers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SD_API")
	float sd_checkpoint_cache =0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SD_API")
	float sd_vae_checkpoint_cache = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SD_API")
	bool img2img_color_correction = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SD_API")
	bool img2img_fix_steps = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SD_API")
	FString img2img_background_color = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SD_API")
	bool send_seed = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SD_API")
	bool send_size = false;

};

USTRUCT()
struct FSDAPI_Response_ImageData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	TArray<FString> images;

	UPROPERTY()
	FString current_Image = "";

	UPROPERTY()
	float progress = 0.f;
};

USTRUCT(BlueprintType)
struct FSDAPI_Response_LoraInfoData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SD_API")
	FString description = "";
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SD_API")
	FString sd_version = "SD1";
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SD_API")
	FString	activation_text = "";
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SD_API")
	FString	preferred_weight = "0.8";
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SD_API")
	FString notes = "";
};

USTRUCT(BlueprintType)
struct FSDAPI_Response_LoraData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SD_API")
	FString name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SD_API")
	FString path;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SD_API")
	FSDAPI_Response_LoraInfoData LoraInfo;

	bool operator !=(const FSDAPI_Response_LoraData& Other) const
	{
		return name != Other.name;
	}
};

USTRUCT(BlueprintType)
struct FSDAPI_Response_ModelData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SD_API")
	FString title;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SD_API")
	FString model_name;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SD_API")
	FString hash;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SD_API")
	FString filename;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SD_API")
	FString config;

};

USTRUCT(BlueprintType)
struct FSDAPI_Response_LoraListData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SD_API")
	TArray< FSDAPI_Response_LoraData> Payload;
};

USTRUCT(BlueprintType)
struct FSDAPI_Response_ModelListData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SD_API")
	TArray< FSDAPI_Response_ModelData> Payload;
};

USTRUCT(BlueprintType)
struct FSDAPI_Controlnet_ModelList
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SD_API")
	TArray< FString> model_list;
};

/**
 */
UCLASS(config = SDAPI, defaultconfig, meta = (DisplayName = "SDAPI SubSystem"), Category = "SD_API")
class STABLEDIFFUSIONSUBSYSTEM_API USDAI_Subsystem : public UEngineSubsystem
{
	GENERATED_BODY()
	
public:

	static USDAI_Subsystem* Get() { return CastChecked<USDAI_Subsystem>(GEngine->GetEngineSubsystemBase(USDAI_Subsystem::StaticClass())); }

	void InitializedDefaultSettingsToAPI();



#pragma  region API_FUNCTION

	UFUNCTION(BlueprintCallable, Category = "SD_API")
	FString GetCurrentConenctedUrl() { return CurrentConenctedUrl; }

	//.ToAsync?
	UFUNCTION(BlueprintCallable, Category = "SD_API")
	TArray<FSDAPI_Response_ModelData> GetAvailableModels() { return AvailableModelList;  }

	UFUNCTION(BlueprintCallable, Category = "SD_API")
	FString FindCheckPointModelFromAvailableModels(FString ContainWord);

	//.From DefaultSDAPI.ini
	UFUNCTION(BlueprintCallable, Category = "SD_API")
	TArray<FString> GetURLKeys();

	UFUNCTION(BlueprintCallable, Category = "SD_API")
	FString GetURLByKey(FString Key);

	UFUNCTION(BlueprintCallable, Category = "SD_API")
	TArray<FString> GetAvailableControlnetModels() { return AvailableControlnetModelList;  }

	UFUNCTION(BlueprintCallable, Category = "SD_API")
	TArray<FString> GetAvailableSamplers();

	UFUNCTION(BlueprintCallable, Category = "SD_API")
	FString FindSamplerFromAvailableSamplers(FString ContainWord);

	UFUNCTION(BlueprintCallable, Category = "SD_API")
	USDAPI_DeveloperSettings* GetSubsystemDeveloperSettings() ;

	UFUNCTION(BlueprintCallable, Category = "SD_API")
	FString GetStatupCommandLineArg();

	UFUNCTION(BlueprintCallable, Category = "SD_API")
	bool IsInitialized() { return CurrentState == ESDAPI_SubsystemState::API_CONNECTED_AND_INITAILZED; }

	bool bInitializing = false;

	UFUNCTION(BlueprintCallable, Category = "SD_API")
	void GetErrorMsg(){}

	UFUNCTION(BlueprintCallable, Category = "SD_API")
	FSDAPI_OptionDataStruct GetCurrentAPIconfig() { return CurrentOptions; }

	UFUNCTION(BlueprintCallable, Category = "SD_API")
	FFilePath GetLocalAIPath();

	UFUNCTION(BlueprintCallable, Category = "SD_API")
	void SetLocalAIPath();

	UFUNCTION(BlueprintCallable, Category = "SD_API")
	void StartLocalAIProgram();

	UFUNCTION(BlueprintCallable, Category = "SD_API")
	void StopLocalAIProgram();

	UFUNCTION(BlueprintCallable, Category = "SD_API")
	bool GetControlNet_LowVRamMode();
	UFUNCTION(BlueprintCallable, Category = "SD_API")
	void SetControlNet_LowVRamMode(bool bControlNet_LowVRamMode);

#pragma  endregion
#pragma region Should Setted By API or DeveloperSetting

	UPROPERTY(EditAnywhere, Category = SDAPI)
	FString LastSubmittedModelName = "Local";

	UPROPERTY(EditAnywhere, Category = SDAPI)
	FString LastSubmittedSamplerName = "Local";

	UPROPERTY(EditAnywhere, Category = SDAPI)
	FString CurrentConenctedUrl;

	UFUNCTION(BlueprintCallable, Category = "SD_API")
	FString GetDefaultURLKey();

	UPROPERTY(BlueprintReadOnly, Category = SDAPI)
	ESDAPI_SubsystemState CurrentState = ESDAPI_SubsystemState::API_NOT_CONNECTED;

	UFUNCTION()
	void SetAvailableModel(TArray< FSDAPI_Response_ModelData> InModels) { AvailableModelList = InModels; }

	UFUNCTION()
	void SetAvailableControlnetModel(TArray< FString> InModels) { AvailableControlnetModelList = InModels; }

	UFUNCTION()
	void SetCurrentAPIconfig(FSDAPI_OptionDataStruct InOptions) { CurrentOptions = InOptions; }

	UFUNCTION()
	void SetCanUseControlnet(bool CanUse) { bCanUseControlnet = CanUse;} 

	UFUNCTION(BlueprintCallable, Category = "SD_API")
	bool CanUseControlnet() {return bCanUseControlnet;} 

	UFUNCTION(BlueprintCallable, Category = "SD_API")
	FString FindFirstControlnetModelContainsName(ESDAPI_Controlnet_Module Module) ;

	UFUNCTION(BlueprintCallable, Category = "SD_API")
	TArray<FString> FindAllControlnetModelContainsName(ESDAPI_Controlnet_Module Module) ;

	UFUNCTION(BlueprintCallable, Category = "SD_API")
	void ResetConnectionState();

#pragma  endregion



#pragma region Lora

	UFUNCTION()
	void SetAvailableLoras(TArray< FSDAPI_Response_LoraData> InModels) { AvailableLoraList = InModels; AvailableLoraList.Sort([](const FSDAPI_Response_LoraData& A, const FSDAPI_Response_LoraData& B) { return (A.path == B.path) ? (A.path > B.path) : (A.path > B.path); });}

	UFUNCTION(BlueprintCallable, Category = "SD_API_Lora")
	TArray<FSDAPI_Response_LoraData> GetAvailableLoras() { return AvailableLoraList;  }

	UFUNCTION(BlueprintCallable, Category = "SD_API_Lora")
	FSDAPI_Response_LoraData FindLoraFromAvailableLorasByName(FString ContainWord);

	void SetLoraInfoData(FString LoraName,FSDAPI_Response_LoraInfoData LoraInfo);

	UFUNCTION(BlueprintCallable, Category = "SD_API_Lora")
	FString GetLoraUsageFormat(FString Name);

	UFUNCTION(BlueprintCallable, Category = "SD_API_Lora")
	FString GetLoraInfoFromName(FString Name);

#pragma  endregion

#pragma region LCM

	UFUNCTION(BlueprintCallable, Category = "SD_API_LCM")
	bool CanUseLCMLora();

	UFUNCTION(BlueprintCallable, Category = "SD_API_LCM")
	void SetLCMDefaultParam(FSDAPI_Parameters_LCM InLCMDefaultParam);

	UFUNCTION(BlueprintCallable, Category = "SD_API_LCM")
	FSDAPI_Parameters_LCM GetLCMDefaultParam();

	UFUNCTION(BlueprintCallable, Category = "SD_API_LCM")
	FString GetLoraFormatForLCM();
#pragma  endregion

protected:

	bool bCanUseControlnet = false;

	TArray<FString> AvailableControlnetModelList;

	TArray<FSDAPI_Response_ModelData> AvailableModelList;
	
	TArray<FSDAPI_Response_LoraData> AvailableLoraList;

	FSDAPI_OptionDataStruct CurrentOptions;
};
