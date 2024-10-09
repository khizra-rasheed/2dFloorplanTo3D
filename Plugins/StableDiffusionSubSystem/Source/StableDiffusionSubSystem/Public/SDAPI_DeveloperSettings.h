// Copyright by CodeParrots 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "SDAPI_DeveloperSettings.generated.h"


USTRUCT(BlueprintType)
struct FSDAPI_Parameters_LCM
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, meta = (ClampMin = "1", ClampMax = "12", UIMin = "1", UIMax = "12", SliderExponent = 1, Category = "SD_API"))
	int32 CLIP_stop_at_last_layers = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "1", UIMin = "1", UIMax = "150", SliderExponent = 1, Category = "SD_API"))
	int32 Steps = 6;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "1", UIMin = "1", UIMax = "30", SliderExponent = 1, Category = "SD_API"))
	int32 Cfg_Scale = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SD_API")
	FString sampler_name = "LCM Test";
};

USTRUCT(BlueprintType)
struct FSDAPI_Parameters
{
	GENERATED_USTRUCT_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite,AdvancedDisplay , meta = (ClampMin = "1", ClampMax = "12", UIMin = "1", UIMax = "12", SliderExponent = 1,Category = "SD_API"))
	int32 CLIP_stop_at_last_layers = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "1", UIMin = "1", UIMax = "150", SliderExponent = 1,Category = "SD_API"))
	int32 Steps = 20;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "1",UIMin = "1", UIMax = "30", SliderExponent = 1,Category = "SD_API"))
	int32 Cfg_Scale = 7;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0", UIMax = "1", ClampMax = "1", SliderExponent = 0.01,Category = "SD_API"))
	float Img2Img_Denoising_Strength =0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "1", UIMin = "1", UIMax = "1024", SliderExponent = 1,Category = "SD_API"))
	int32 width = 512;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "1", UIMin = "1", UIMax = "1024", SliderExponent = 1,Category = "SD_API"))
	int32 height = 512;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SD_API")
	FString sampler_name = "Euler";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "SD_API")
	TArray<FString> Styles;

// 	//.Lock_Unlock NSFW Image;../ this query removed SDAPI At some point
// 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
// 	bool Filter_NSFW = false;

	void AddQueryField(TSharedPtr<FJsonObject> JsonObject);

};


UENUM(BlueprintType)
enum class ESDAPI_resize_mode : uint8
{
	Just_Resize = 0,
	Scale_to_Fit = 1,
	Envelope = 2
};

UENUM(BlueprintType)
enum class ESDAPI_Controlnet_Module : uint8
{
	none,
	//Outline
	canny,
	depth,
	depth_leres,
	pidinet_scribble,
	fake_scribble,
	hed,
	mlsd,
	normal_map,
	openpose,
	openpose_hand,
	openpose_full,
	segmentation,
	LineArt_Anime,
	LineArt_Standard,
	LineArt_Realistic,
	Invert,
	SoftEdge_pidnet,
	Shuffle,
	tile,
	inpaint_only,
	IP2P,
	Reference_only,
	T2IA_Style_clipvision,
	//Contrast Black&White
	binary,
	//.Low quality
	color
};


USTRUCT(BlueprintType)
struct FSDAPI_Controlnet_Parameters
{

	GENERATED_USTRUCT_BODY()
public:

	//.if you don't use Preprocessor, you must one of Input_image as preprocessed Data
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SD_API")
	UTexture2D* input_image_Texture = nullptr;
	//.if you don't use Preprocessor, you must one of Input_image as preprocessed Data
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SD_API")
	UTextureRenderTarget2D* input_image_RenderTarget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SD_API")
	ESDAPI_Controlnet_Module Module = ESDAPI_Controlnet_Module::none;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SD_API")
	FString model = "None";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.01", ClampMax = "2", UIMin = "0.0", UIMax = "2.0", SliderExponent = 2,Category = "SD_API"))
	float weight = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SD_API")
	ESDAPI_resize_mode resize_mode = ESDAPI_resize_mode::Just_Resize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SD_API")
	bool lowvram = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,AdvancedDisplay, meta = (ClampMin = "0.01", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0", SliderExponent = 2,Category = "SD_API"))
	float guidance_start = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, meta = (ClampMin = "0.01", UIMin = "0.0", UIMax = "1.0", SliderExponent = 2,Category = "SD_API"))
	float guidance_end = 1.0f;

// query API example:
// 	{
// 		"init_images": ["base64..."] ,
// 			"sampler_name" : "Euler",
// 			"alwayson_scripts" : {
// 			"controlnet": {
// 				"args": [
// 				{
// 					"module": "depth",
// 					"model" : "diff_control_sd15_depth_fp16 [978ef0a1]"
// 				}
// 				]
// 			}
// 		}
// 	}
	void AddQueryField(TSharedPtr<FJsonObject> JsonObject);
};

/**
 * 
 */
UCLASS(config = SDAPI, defaultconfig, meta = (DisplayName = "SDAPI Settings",Category = "SD_API"))
class USDAPI_DeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, config, Category = SDAPI)
		FString DefaultAPI_URL_Key = "Local";
	
#pragma region CommandText

	UPROPERTY(EditAnywhere, config, Category = SDAPI_CommandText)
	FString ProgressCommand = "/progress";

	UPROPERTY(EditAnywhere, config, Category = SDAPI_CommandText)
	FString Text2ImgCommand = "/txt2img";

	UPROPERTY(EditAnywhere, config, Category = SDAPI_CommandText)
	FString Img2ImgCommand = "/img2img";

	UPROPERTY(EditAnywhere, config, Category = SDAPI_CommandText)
	FString OptionCommand = "/options";

	UPROPERTY(EditAnywhere, config, Category = SDAPI_CommandText)
	FString GetSd_ModelsCommandUri = "/sd-models";

	UPROPERTY(EditAnywhere, config, Category = SDAPI_CommandText)
	FString GetSd_LoraListCommandUri = "/loras";

	UPROPERTY(EditAnywhere, config, Category = SDAPI_CommandText)
	FString ControlnetModelListCommand = "/model_list";

	UPROPERTY(EditAnywhere, config, Category = SDAPI_CommandText)
	FString ControlnetModelDetectCommand = "/detect";

public:
	FString GetDefaultURLKey() { return DefaultAPI_URL_Key; }
	FString GetText2ImgCommand() { return Text2ImgCommand; }
	FString GetImg2ImgCommand() { return Img2ImgCommand; }
	FString GetProgressCommand() { return ProgressCommand; }
	FString GetOptionCommand() { return OptionCommand; }
	FString GetSd_ModelsCommand() { return GetSd_ModelsCommandUri; }
	FString GetSd_LorasCommand() { return GetSd_LoraListCommandUri; }
	FString GetControlnetModelListCommand() { return ControlnetModelListCommand; }
	FString GetControlnetURL(FString URL) {FString ControlnetURL = URL.Replace(TEXT("sdapi/v1"), TEXT("controlnet")); return ControlnetURL;}
	FString GetLoraInfoURL(FString URL, FString LoraName) { FString LoraInfoURL = URL.Replace(TEXT("sdapi/v1"), TEXT("tacapi/v1/lora-info")); return LoraInfoURL + "/" + LoraName; }
	//.UsageRate = 0~1.f
	FString GetLoraFormat(FString LoraName, float UsageRate) { FStringFormatOrderedArguments args = { LoraName,UsageRate }; args.Add(FStringFormatArg(UsageRate)); return FString::Format(TEXT("<lora:{0}:{1}>"), args); }
	FString GetLCMLoraFormat() { return GetLoraFormat(LCMLoraName, 1.f); }
	FString GetControlnetPreprocessorName(ESDAPI_Controlnet_Module Module) 
	{
		if (ControlnetModuleName.Contains(Module))
		{
			return ControlnetModuleName[Module];

		}
		else
		{
			return FString("none");
		}
	}

	FString GetControlnetModelKeywordToSearch(ESDAPI_Controlnet_Module Module) 
	{
		if (ControlnetModelKeywordFromModuleName.Contains(Module))
		{
			return ControlnetModelKeywordFromModuleName[Module];
		}

		return FString("none");
	}

#pragma endregion

#pragma region DataText
	UPROPERTY(EditAnywhere, config, Category = SDAPI_Defaults)
	float LoraDefaultUsage = 0.8f;

	UPROPERTY(EditAnywhere, config, Category = SDAPI_Defaults)
	FString LCMLoraName = "LCM_LoRA_Weights_SD15";

	UPROPERTY(EditAnywhere, config, Category = SDAPI_CommandText)
	FString DefaultSavePath = "Game/SDAPI_Results";

	//. WebUI.bat File
	UPROPERTY(EditAnywhere, config, Category = SDAPI_CommandText,meta =( FilePathFilter = "bat"))
	FFilePath Automatic1111LocalAIPath ;

	UPROPERTY(EditAnywhere, config, Category = SDAPI_CommandText)
	FString AIStatupCommandLineArgs = "--deepdanbooru --nowebui --xformers";

	void SetLocalAIPath(FString InPath) { Automatic1111LocalAIPath.FilePath = InPath; }

	FString GetStatupCommandLineArg() { return AIStatupCommandLineArgs; }


	FFilePath GetLocalAIPath() { return Automatic1111LocalAIPath; }

	FString GetDefaultSavePath() { return DefaultSavePath; }
#pragma endregion
	//. Time Interval of ProgressImage GET Request. If used frequently, image creation will be slower.
	UPROPERTY(EditAnywhere, config, Category = SDAPI_Progress)
	bool bUseProgressImage = true;
	//. Time Interval of ProgressImage GET Request. If used frequently, image creation will be slower.
	UPROPERTY(EditAnywhere, config, Category = SDAPI_Progress)
	float ProgressImageUpdateTime = 1.5f;

	void SetUseProgressImage(bool InUseProgressImage) { bUseProgressImage = InUseProgressImage; }
	bool GetUseProgressImage() { return bUseProgressImage; }

	void SetProgressIamgeUpdateTime(float InProgressImageUpdateTime) { ProgressImageUpdateTime = InProgressImageUpdateTime; }
	float GetProgressIamgeUpdateTime() { return ProgressImageUpdateTime; }

	void SetControlNet_LowVRamMode(bool InControlNet_LowVRamMode) { ControlNet_LowVRamMode = InControlNet_LowVRamMode; }
	bool GetControlNet_LowVRamMode() { return ControlNet_LowVRamMode; }

	UPROPERTY(EditAnywhere, config, Category = SDAPI_CommandText)
	bool ControlNet_LowVRamMode = false;

public:
#pragma region Defaults
	UPROPERTY(EditAnywhere, config, Category = SDAPI_Defaults)
	FSDAPI_Parameters DefaultParams;

	UPROPERTY(EditAnywhere, config, Category = SDAPI_Defaults)
	FSDAPI_Parameters_LCM DefaultLCMParams;

	UPROPERTY(EditAnywhere, config, Category = SDAPI_Defaults)
	TArray<FString> AvailableSamplers = 
	{ 
		"Euler a", "Euler", "LMS", "Heun", "DPM2", "DPM2 a", "DPM++ 2S a", "DPM++ 2M", "DPM++ SDE", "DPM fast", "DPM adaptive", "LMS Karras", "DPM2 Karras", "DPM2 a Karras", "DPM++ 2S a Karras", "DPM++ 2M Karras", "DPM++ SDE Karras", "LCM Test"
	};

	UPROPERTY(EditAnywhere, config, Category = SDAPI_Defaults)
	TArray<FString> AvailableStyles = { "Test" };

	UPROPERTY(EditAnywhere, config, Category = SDAPI_Defaults)
	TMap<FString, FString> API_URLMap =
	{
		{"Local", "http://127.0.0.1:7861/sdapi/v1"},
		{"Test", "http://localhost:7861/sdapi/v1"},
	};

	UPROPERTY(EditAnywhere, config, Category = SDAPI_Defaults)
	TMap<ESDAPI_Controlnet_Module,FString>  ControlnetModuleName =
	{ 
		{ESDAPI_Controlnet_Module::none, "none"},
		{ESDAPI_Controlnet_Module::canny,"canny"},
		{ESDAPI_Controlnet_Module::binary,"binary"},
		{ESDAPI_Controlnet_Module::color,"color"},
		{ESDAPI_Controlnet_Module::depth,"depth"},
		{ESDAPI_Controlnet_Module::depth_leres,"depth_leres"},
		{ESDAPI_Controlnet_Module::fake_scribble,"fake_scribble"},
		{ESDAPI_Controlnet_Module::pidinet_scribble,"pidinet_scribble"},
		{ESDAPI_Controlnet_Module::hed,"hed"},
		{ESDAPI_Controlnet_Module::normal_map,"normal_bae"},
		{ESDAPI_Controlnet_Module::mlsd,"mlsd"},
		{ESDAPI_Controlnet_Module::segmentation,"segmentation"},
		{ESDAPI_Controlnet_Module::openpose,"openpose"},
		{ESDAPI_Controlnet_Module::openpose_full,"openpose_full"},
		{ESDAPI_Controlnet_Module::openpose_hand,"openpose_hand"},
		{ESDAPI_Controlnet_Module::LineArt_Anime,"lineart_anime"},
		{ESDAPI_Controlnet_Module::LineArt_Realistic,"lineart_realistic"},
		{ESDAPI_Controlnet_Module::LineArt_Standard,"lineart_standard"},
		{ESDAPI_Controlnet_Module::Invert,"Invert"},
		{ESDAPI_Controlnet_Module::SoftEdge_pidnet,"softedge_pidinet"},
		{ESDAPI_Controlnet_Module::Shuffle,"shuffle"},
		{ESDAPI_Controlnet_Module::tile,"tile_resample"},
		{ESDAPI_Controlnet_Module::inpaint_only,"identity"},
		{ESDAPI_Controlnet_Module::IP2P,"IP2P"},
		{ESDAPI_Controlnet_Module::Reference_only,"reference_only"},
		{ESDAPI_Controlnet_Module::T2IA_Style_clipvision,"T2IA_Style_clipvision"}
	};

	UPROPERTY(EditAnywhere, config, Category = SDAPI_Defaults)
	TMap<ESDAPI_Controlnet_Module,FString> ControlnetModelKeywordFromModuleName =
	{ 
		{ESDAPI_Controlnet_Module::none, "None"},
		{ESDAPI_Controlnet_Module::canny,"canny"},
		{ESDAPI_Controlnet_Module::binary,"binary"},
		{ESDAPI_Controlnet_Module::color,"color"},
		{ESDAPI_Controlnet_Module::depth,"depth"},
		{ESDAPI_Controlnet_Module::depth_leres,"depth"},
		{ESDAPI_Controlnet_Module::fake_scribble,"scribble"},
		{ESDAPI_Controlnet_Module::pidinet_scribble,"scribble"},
		{ESDAPI_Controlnet_Module::hed,"hed"},
		{ESDAPI_Controlnet_Module::normal_map,"normal"},
		{ESDAPI_Controlnet_Module::mlsd,"mlsd"},
		{ESDAPI_Controlnet_Module::segmentation,"seg"},
		{ESDAPI_Controlnet_Module::openpose,"openpose"},
		{ESDAPI_Controlnet_Module::openpose_hand,"openpose"},
		{ESDAPI_Controlnet_Module::openpose_full,"openpose"},
		{ESDAPI_Controlnet_Module::LineArt_Anime,"lineart_anime"},
		{ESDAPI_Controlnet_Module::LineArt_Realistic,"LineArt"},
		{ESDAPI_Controlnet_Module::LineArt_Standard,"LineArt"},
		{ESDAPI_Controlnet_Module::Invert,"LineArt"},
		{ESDAPI_Controlnet_Module::SoftEdge_pidnet,"SoftEdge"},
		{ESDAPI_Controlnet_Module::Shuffle,"Shuffle"},
		{ESDAPI_Controlnet_Module::tile,"tile"},
		{ESDAPI_Controlnet_Module::inpaint_only,"inpaint"},
		{ESDAPI_Controlnet_Module::IP2P,"IP2P"},
		{ESDAPI_Controlnet_Module::Reference_only,"none"},
		{ESDAPI_Controlnet_Module::T2IA_Style_clipvision,"T2IA"}
	};
#pragma endregion

#pragma region UtilityFunctions
	FString GetTxt2ImgUrl(FString APIURLKey = "Local") {
		return GetAPIUrl(APIURLKey) + Text2ImgCommand;
	}

	FString GetImg2ImgUrl(FString APIURLKey = "Local") {
		return  GetAPIUrl(APIURLKey) + Img2ImgCommand;
	}

	FString GetConfigUrl(FString APIURLKey = "Local") {
		return GetAPIUrl(APIURLKey) + OptionCommand;
	}

	FString GetModelsCommandUrl(FString APIURLKey = "Local") {
		return  GetAPIUrl(APIURLKey) + GetSd_ModelsCommandUri;
	}

	FString GetAPIUrl(FString APIURLKey = "Local")
	{
		return  API_URLMap[APIURLKey];
	}

	static USDAPI_DeveloperSettings* Get() { return CastChecked<USDAPI_DeveloperSettings>(USDAPI_DeveloperSettings::StaticClass()->GetDefaultObject()); }
#pragma endregion

};
