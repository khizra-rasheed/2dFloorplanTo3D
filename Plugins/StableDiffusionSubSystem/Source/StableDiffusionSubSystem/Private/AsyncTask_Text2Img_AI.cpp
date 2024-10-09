// Copyright by CodeParrots 2023. All Rights Reserved.


#include "AsyncTask_Text2Img_AI.h"
#include "Modules/ModuleManager.h"
#include "Rendering/Texture2DResource.h"
#include "ImageUtils.h"
#include "Engine/Texture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/Texture2DDynamic.h"
#include "Engine/Engine.h"

#if WITH_EDITOR
#include "Editor.h"
#include "Editor/EditorEngine.h"
#endif
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Json.h"
#include "SDAPI_DeveloperSettings.h"
#include "SDAI_Subsystem.h"
#include "SDAPI_Statics.h"
#include "TimerManager.h"
#include "Interfaces/IHttpResponse.h"
#include "JsonUtilities.h"
#include "JsonObjectConverter.h"
#include "HttpModule.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AsyncTask_Text2Img_AI)

//----------------------------------------------------------------------//
// AsyncTask_Text2Img_AI
//----------------------------------------------------------------------//

UAsyncTask_SDAPI_Text2Img::UAsyncTask_SDAPI_Text2Img(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	if (HasAnyFlags(RF_ClassDefaultObject) == false)
	{
		AddToRoot();
	}
}

UAsyncTask_SDAPI_Text2Img* UAsyncTask_SDAPI_Text2Img::SDAPI_Text2Img_BySubSystem(USDAI_Subsystem* SubSystem, FString Prompts, FString Negative_Prompt, FString TargetModelName, FSDAPI_Parameters SubParam, FSDAPI_Controlnet_Parameters Controlent_Param, bool bUseLCMMode, bool bUseLCMDefaultParam)
{
	UAsyncTask_SDAPI_Text2Img* DownloadTask = NewObject<UAsyncTask_SDAPI_Text2Img>();
	if (IsValid(SubSystem) && SubSystem->IsInitialized())
	{
		SetLCMSetting(SubSystem, Prompts, SubParam, bUseLCMMode, bUseLCMDefaultParam);
		DownloadTask->Start(SubSystem->GetCurrentConenctedUrl() + USDAPI_DeveloperSettings::Get()->GetText2ImgCommand(), Prompts, Negative_Prompt, TargetModelName, SubParam, Controlent_Param);
		DownloadTask->RequestUpdateProgressImage(SubSystem->GetCurrentConenctedUrl());
	}
	else
	{
		DownloadTask->Start("", Prompts, Negative_Prompt, TargetModelName, SubParam, Controlent_Param);
	}

	return DownloadTask;
}

UAsyncTask_SDAPI_Text2Img* UAsyncTask_SDAPI_Text2Img::SDAPI_Text2Img(FString URL, FString Prompts, FString Negative_Prompt, FString TargetModelName, FSDAPI_Parameters SubParam, FSDAPI_Controlnet_Parameters Controlent_Param)
{
	UAsyncTask_SDAPI_Text2Img* DownloadTask = NewObject<UAsyncTask_SDAPI_Text2Img>();
	DownloadTask->Start(URL + USDAPI_DeveloperSettings::Get()->GetText2ImgCommand(), Prompts, Negative_Prompt, TargetModelName, SubParam, Controlent_Param);
	DownloadTask->RequestUpdateProgressImage(URL);
	return DownloadTask;
}

void UAsyncTask_SDAPI_Text2Img::Start(FString URL, FString Prompts, FString Negative_Prompt, FString TargetModelName, FSDAPI_Parameters SubParam, FSDAPI_Controlnet_Parameters Controlent_Param)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	TSharedPtr<FJsonObject> Override_Settings = MakeShareable(new FJsonObject);
	JsonObject->SetStringField("prompt", Prompts);
	JsonObject->SetStringField("negative_prompt", Negative_Prompt);
	
	Override_Settings->SetStringField("sd_model_checkpoint", TargetModelName);
	JsonObject->SetObjectField("override_settings", Override_Settings);


	SubParam.AddQueryField(JsonObject);
	if (Controlent_Param.model != "none")
	{
		Controlent_Param.AddQueryField(JsonObject);
	}

	// JsonObject를 FString으로 직렬화
	FString JsonString;
	TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&JsonString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter);

	// FString을 TArray<uint8>으로 변환
	TArray<uint8> Content;
	FTCHARToUTF8 Converter(*JsonString);
	Content.Append((uint8*)TCHAR_TO_UTF8(*JsonString), Converter.Length());


	// Create the Http request and add to pending request list
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UAsyncTask_SDAPI_Text2Img::HandleImageRequest);
	HttpRequest->SetURL(URL);
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json;"));
	HttpRequest->SetContent(Content);
	HttpRequest->SetVerb(TEXT("POST"));
	HttpRequest->ProcessRequest();
}

UAsyncTask_SDAPI_Text2Img* UAsyncTask_SDAPI_Text2Img::SDAPI_Img2Img(FString TextureEncoded, FString URL, FString Prompts, FString Negative_Prompt, FString TargetModelName, FSDAPI_Parameters SubParam, FSDAPI_Controlnet_Parameters Controlent_Param)
{
	UAsyncTask_SDAPI_Text2Img* DownloadTask = NewObject<UAsyncTask_SDAPI_Text2Img>();
	FString Img2ImgString = USDAPI_DeveloperSettings::Get()->GetImg2ImgCommand();
	if (URL.Contains(Img2ImgString) == false)
	{
		URL =URL + USDAPI_DeveloperSettings::Get()->GetImg2ImgCommand();
	}
	DownloadTask->Start(TextureEncoded, URL, Prompts, Negative_Prompt, TargetModelName, SubParam, Controlent_Param);

	URL = URL.Replace(*Img2ImgString,TEXT(""));

	DownloadTask->RequestUpdateProgressImage(URL);

	return DownloadTask;
}

UAsyncTask_SDAPI_Text2Img* UAsyncTask_SDAPI_Text2Img::SDAPI_Img2Img_BySubSystem(UTexture2D* Texture, USDAI_Subsystem* SubSystem, FString Prompts, FString Negative_Prompt, FString TargetModelName, FSDAPI_Parameters SubParam, FSDAPI_Controlnet_Parameters Controlent_Param, bool bUseLCMMode, bool bUseLCMDefaultParam)
{
	UAsyncTask_SDAPI_Text2Img* DownloadTask = NewObject<UAsyncTask_SDAPI_Text2Img>();
	if (IsValid(SubSystem) && SubSystem->IsInitialized())
	{
		if (IsValid(Texture) == false)
		{
			TArray< UTexture2D*> TextureArray;
			DownloadTask->Fail.Broadcast(TextureArray);
		}

		FString TextureEncoded = USDAPI_Statics::MakeTexture2Base64EncodeData(Texture);

		SetLCMSetting(SubSystem, Prompts, SubParam, bUseLCMMode, bUseLCMDefaultParam);

		DownloadTask->Start(TextureEncoded, SubSystem->GetCurrentConenctedUrl() + USDAPI_DeveloperSettings::Get()->GetImg2ImgCommand(), Prompts, Negative_Prompt, TargetModelName, SubParam, Controlent_Param);
		DownloadTask->RequestUpdateProgressImage(SubSystem->GetCurrentConenctedUrl());
	}
	else
	{
		DownloadTask->Start("","", Prompts, Negative_Prompt, TargetModelName, SubParam, Controlent_Param);
	}

	return DownloadTask;
}

UAsyncTask_SDAPI_Text2Img* UAsyncTask_SDAPI_Text2Img::SDAPI_RenderTarget2Img(UTextureRenderTarget2D* RenderTarget, FString URL, FString Prompts, FString Negative_Prompt, FString TargetModelName, FSDAPI_Parameters SubParam, FSDAPI_Controlnet_Parameters Controlent_Param)
{
	FString TextureEncoded = USDAPI_Statics::MakeRenderTarget2Base64EncodeData(RenderTarget);
	if (URL.Contains(USDAPI_DeveloperSettings::Get()->GetImg2ImgCommand()) == false)
	{
		URL = URL + USDAPI_DeveloperSettings::Get()->GetImg2ImgCommand();
	}
	return SDAPI_Img2Img(TextureEncoded, URL, Prompts, Negative_Prompt, TargetModelName, SubParam, Controlent_Param);
}

UAsyncTask_SDAPI_Text2Img* UAsyncTask_SDAPI_Text2Img::SDAPI_RenderTarget2Img_BySubSystem(UTextureRenderTarget2D* RenderTarget, USDAI_Subsystem* SubSystem, FString Prompts, FString Negative_Prompt, FString TargetModelName, FSDAPI_Parameters SubParam, FSDAPI_Controlnet_Parameters Controlent_Param, bool bUseLCMMode, bool bUseLCMDefaultParam)
{
	if (IsValid(SubSystem) && SubSystem->IsInitialized())
	{
		FString TextureEncoded = USDAPI_Statics::MakeRenderTarget2Base64EncodeData(RenderTarget);
		SetLCMSetting(SubSystem, Prompts, SubParam, bUseLCMMode, bUseLCMDefaultParam);

		return SDAPI_Img2Img(TextureEncoded, SubSystem->GetCurrentConenctedUrl() + USDAPI_DeveloperSettings::Get()->GetImg2ImgCommand(), Prompts, Negative_Prompt, TargetModelName, SubParam, Controlent_Param);
	}
	else
	{
		FString TextureEncoded = USDAPI_Statics::MakeRenderTarget2Base64EncodeData(RenderTarget);
		return SDAPI_Img2Img(TextureEncoded, "", Prompts, Negative_Prompt, TargetModelName, SubParam, Controlent_Param);
	}	
}

void UAsyncTask_SDAPI_Text2Img::UpdateProgressImage(FString URL)
{
	USDAPI_DeveloperSettings* Settings = USDAPI_DeveloperSettings::Get();
	if (IsValid(Settings))
	{
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> GetSD_ModelListRequest = FHttpModule::Get().CreateRequest();

		GetSD_ModelListRequest->OnProcessRequestComplete().BindUObject(this, &UAsyncTask_SDAPI_Text2Img::HandleProgressImage);
		GetSD_ModelListRequest->SetURL(URL + Settings->GetProgressCommand());
		GetSD_ModelListRequest->SetVerb(TEXT("GET"));
		GetSD_ModelListRequest->ProcessRequest();
	}
}

void UAsyncTask_SDAPI_Text2Img::RequestUpdateProgressImage(FString URL)
{
	SavedURLForInstance = URL;
	USDAPI_DeveloperSettings* Settings = USDAPI_DeveloperSettings::Get();
	if (IsValid(Settings))
	{
		if (OnGetResponed || Settings->GetUseProgressImage() == false)
		{
			return;
		}

		FTimerDelegate TimeDel;
		TimeDel.BindUFunction(this, FName("UpdateProgressImage"), URL);

		FTimerManager TimerManager;

#if WITH_EDITOR
		GEditor->GetTimerManager().Get().SetTimer(ProgressCheckTimer, TimeDel, Settings->GetProgressIamgeUpdateTime(), false);
		return;
#endif

		if (GEngine->GetWorldContexts().Num() > 0)
		{
			UWorld* World = GEngine->GetWorldContexts()[0].World();

			if (IsValid(World))
			{
				World->GetTimerManager().SetTimer(ProgressCheckTimer, TimeDel, Settings->GetProgressIamgeUpdateTime(), false);
				return;
			}
		}
	}
}

void UAsyncTask_SDAPI_Text2Img::Start(FString TextureEncoded, FString URL, FString Prompts, FString Negative_Prompt, FString TargetModelName, FSDAPI_Parameters SubParam, FSDAPI_Controlnet_Parameters Controlent_Param)
{

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	TSharedPtr<FJsonObject> Override_Settings = MakeShareable(new FJsonObject);
	Override_Settings->SetStringField("sd_model_checkpoint", TargetModelName);
	JsonObject->SetStringField("prompt", Prompts);
	JsonObject->SetStringField("negative_prompt", Negative_Prompt);

	JsonObject->SetObjectField("override_settings", Override_Settings);
	SubParam.AddQueryField(JsonObject);

	if (Controlent_Param.model != "none")
	{
		Controlent_Param.AddQueryField(JsonObject);
	}
 
	FString Base64String= FString::Printf(TEXT("data:image/jpg;base64,%s"), *TextureEncoded);
	
	// Base64String을 FJsonValueString으로 변환
	TSharedPtr<FJsonValueString> EncodedStringJson = MakeShared<FJsonValueString>(Base64String);

	// 이미지 배열에 추가
	TArray<TSharedPtr<FJsonValue>> ImageArray;
	ImageArray.Add(EncodedStringJson);
	JsonObject->SetArrayField("init_images", ImageArray);


	FString JsonString;
	TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&JsonString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter);

	// Create the Http request and add to pending request list
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UAsyncTask_SDAPI_Text2Img::HandleImageRequest);
	HttpRequest->SetURL(URL);
	HttpRequest->SetContentAsString(JsonString);
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json;"));
	HttpRequest->SetVerb(TEXT("POST"));
	HttpRequest->ProcessRequest();
}

void UAsyncTask_SDAPI_Text2Img::HandleImageRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	TArray< UTexture2D*> TextureArray;
	RemoveFromRoot();
	if (HttpResponse == nullptr)
	{
		
		Fail.Broadcast(TextureArray);
		return;
	}

	OnGetResponed = true;
	bool bResult = false;
	FString ResponseStr = HttpResponse->GetContentAsString();
	FSDAPI_Response_ImageData Data;

	FJsonObjectConverter::JsonObjectStringToUStruct(ResponseStr, &Data, 0, 0);
	if (bSucceeded && HttpResponse.IsValid() && HttpResponse->GetContentLength() > 0)
	{
		IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
		TSharedPtr<IImageWrapper> ImageWrappers[3] =
		{
			ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG),
			ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG),
			ImageWrapperModule.CreateImageWrapper(EImageFormat::BMP),
		};

		for (FString Image : Data.images)
		{

			TArray<uint8> ImageData;
			FBase64::Decode(Image, ImageData);

			for (TSharedPtr<IImageWrapper> ImageWrapper : ImageWrappers)
			{
				if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(ImageData.GetData(), ImageData.Num()))
				{
					TArray64<uint8> RawData;
					const ERGBFormat InFormat = ERGBFormat::BGRA;
					if (ImageWrapper->GetRaw(InFormat, 8, RawData))
					{
						UTexture2D* Texture = CreateTexture2D(RawData, ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), PF_B8G8R8A8, *FString::FromInt(Count++));
						Texture->AddToRoot();
						TextureArray.Add(Texture);
					}
				}
			}
		}
	}

	if (TextureArray.Num() > 0)
	{
		Success.Broadcast(TextureArray);
		return;
	}

	if (ResponseStr.Len() < 10)
	{
		return;
	}

	Fail.Broadcast(TextureArray);
}

void UAsyncTask_SDAPI_Text2Img::HandleProgressImage(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	
	RemoveFromRoot();

	TArray< UTexture2D*> TextureArray;
	if (bSucceeded && HttpResponse.IsValid() && HttpResponse->GetContentLength() > 0)
	{
		FString ResponseStr = HttpResponse->GetContentAsString();
		FSDAPI_Response_ImageData Data;
		FJsonObjectConverter::JsonObjectStringToUStruct(ResponseStr, &Data, 0, 0);

		IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
		TSharedPtr<IImageWrapper> ImageWrappers[3] =
		{
			ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG),
			ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG),
			ImageWrapperModule.CreateImageWrapper(EImageFormat::BMP),
		};

		FString Image = Data.current_Image;
		
		TArray<uint8> ImageData;
		FBase64::Decode(Image, ImageData);

		for (auto ImageWrapper : ImageWrappers)
		{
			if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(ImageData.GetData(), ImageData.Num()))
			{
				TArray64<uint8> RawData;
				const ERGBFormat InFormat = ERGBFormat::BGRA;
				if (ImageWrapper->GetRaw(InFormat, 8, RawData))
				{
					UTexture2D* Texture = CreateTexture2D(RawData, ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), PF_B8G8R8A8, *FString::FromInt(Count++));
					Texture->AddToRoot();
					ProgressPercent = (int32)Data.progress * 100;
					TextureArray.Add(Texture);
				}
			}
		}
	}

	if (TextureArray.Num() > 0)
	{
		ProgressImage.Broadcast(TextureArray);
	}

	RequestUpdateProgressImage(SavedURLForInstance);
}



UTexture2D* UAsyncTask_SDAPI_Text2Img::CreateTexture2D(const TArray64<uint8>& PixelData, int32 InSizeX, int32 InSizeY, EPixelFormat InFormat,FName BaseName)
{
	UTexture2D * NewTexture = nullptr;
	if (InSizeX > 0 && InSizeY > 0 &&
		(InSizeX % GPixelFormats[InFormat].BlockSizeX) == 0 &&
		(InSizeY % GPixelFormats[InFormat].BlockSizeY) == 0)
	{
		NewTexture = NewObject<UTexture2D>(this,BaseName, RF_Public | RF_Standalone | RF_MarkAsRootSet);

			NewTexture->SetPlatformData(new FTexturePlatformData());
			NewTexture->GetPlatformData()->SizeX = InSizeX;
			NewTexture->GetPlatformData()->SizeY = InSizeY;
			NewTexture->GetPlatformData()->PixelFormat = InFormat;
			NewTexture->SRGB = true;
			//NewTexture->CompressionSettings = TC_HDR;
			// ALLocate first mipmap.
			int32 NumBlocksX = InSizeX / GPixelFormats[InFormat].BlockSizeX;
			int32 NumBlocksY = InSizeY / GPixelFormats[InFormat].BlockSizeY; 
			FTexture2DMipMap* Mip = new FTexture2DMipMap();
			NewTexture->GetPlatformData()->Mips.Add(Mip);
			Mip->SizeX = InSizeX;
			Mip->SizeY = InSizeY;
			Mip->BulkData.Lock(LOCK_READ_WRITE);
			void* TextureData = Mip->BulkData.Realloc(NumBlocksX * NumBlocksY * GPixelFormats[InFormat].BlockBytes);
			FMemory::Memcpy(TextureData, PixelData.GetData(), PixelData.Num());
			Mip->BulkData.Unlock();
			NewTexture->UpdateResource();
	}
	else
	{
		//UE_LOG(LogImageManager, Warning, TEXT("Invalid parameters specified for ImageManager::CreateTexture2D()"));
	}
	return NewTexture;
}

void UAsyncTask_SDAPI_Text2Img::SetLCMSetting(USDAI_Subsystem* SubSystem,FString& Prompt, FSDAPI_Parameters& SubParam, bool bUseLCMMode, bool bUseLCMDefaultParam)
{
	if (bUseLCMMode)
	{
		Prompt.Append(SubSystem->GetLoraFormatForLCM());
		if (bUseLCMDefaultParam)
		{
			FSDAPI_Parameters_LCM LCMParam = SubSystem->GetLCMDefaultParam();

			SubParam.Cfg_Scale = LCMParam.Cfg_Scale;
			SubParam.CLIP_stop_at_last_layers = LCMParam.CLIP_stop_at_last_layers;
			SubParam.sampler_name = LCMParam.sampler_name;
			SubParam.Steps = LCMParam.Steps;
		}
	}
}

UAsyncTask_SDAPI_ConnectAndInitialize::UAsyncTask_SDAPI_ConnectAndInitialize(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	if (HasAnyFlags(RF_ClassDefaultObject) == false)
	{
		AddToRoot();
	}
}

UAsyncTask_SDAPI_ConnectAndInitialize* UAsyncTask_SDAPI_ConnectAndInitialize::SDAPI_ConnectAndInitialize(USDAI_Subsystem* SubSystem, FString URLKey)
{
	if (IsValid(SubSystem) && SubSystem->GetSubsystemDeveloperSettings())
	{
		FString URL = SubSystem->GetSubsystemDeveloperSettings()->API_URLMap[URLKey];

		return SDAPI_ConnectAndInitialize_ManualURL(SubSystem, URL);
	}

	return nullptr;
}

UAsyncTask_SDAPI_ConnectAndInitialize* UAsyncTask_SDAPI_ConnectAndInitialize::SDAPI_ConnectAndInitialize_ManualURL(USDAI_Subsystem* SubSystem, FString URL)
{
	UAsyncTask_SDAPI_ConnectAndInitialize* InitailizedTask = NewObject<UAsyncTask_SDAPI_ConnectAndInitialize>();
	InitailizedTask->Start(SubSystem, URL);

	return InitailizedTask;
}

void UAsyncTask_SDAPI_ConnectAndInitialize::Start(USDAI_Subsystem* SubSystem,FString URL)
{
	USDAPI_DeveloperSettings* Settings = USDAPI_DeveloperSettings::Get();
	if (IsValid(Settings)&& IsValid(SubSystem))
	{
		RecivedSubSystem = SubSystem;
		SubSystem->CurrentConenctedUrl = URL;
		SubSystem->CurrentState = ESDAPI_SubsystemState::API_NOT_CONNECTED;

		//. 
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> GetSD_ModelListRequest = FHttpModule::Get().CreateRequest();

		GetSD_ModelListRequest->OnProcessRequestComplete().BindUObject(this, &UAsyncTask_SDAPI_ConnectAndInitialize::HandleModelListRequest);
		GetSD_ModelListRequest->SetURL(URL + Settings->GetSd_ModelsCommand());
		GetSD_ModelListRequest->SetVerb(TEXT("GET"));
		GetSD_ModelListRequest->ProcessRequest();

		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> GetSD_LoraListRequest = FHttpModule::Get().CreateRequest();

		GetSD_LoraListRequest->OnProcessRequestComplete().BindUObject(this, &UAsyncTask_SDAPI_ConnectAndInitialize::HandleLoraListRequest);
		GetSD_LoraListRequest->SetURL(URL + Settings->GetSd_LorasCommand());
		GetSD_LoraListRequest->SetVerb(TEXT("GET"));
		GetSD_LoraListRequest->ProcessRequest();

		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> GetConfigRequest = FHttpModule::Get().CreateRequest();

		GetConfigRequest->OnProcessRequestComplete().BindUObject(this, &UAsyncTask_SDAPI_ConnectAndInitialize::HandleConfigRequest);
		GetConfigRequest->SetURL(URL + Settings->GetOptionCommand());
		GetConfigRequest->SetVerb(TEXT("GET"));
		GetConfigRequest->ProcessRequest();

		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> GetControlnetRequest = FHttpModule::Get().CreateRequest();

		GetControlnetRequest->OnProcessRequestComplete().BindUObject(this, &UAsyncTask_SDAPI_ConnectAndInitialize::HandleControlnetModelListRequest);
		GetControlnetRequest->SetURL(Settings->GetControlnetURL(URL) + Settings->GetControlnetModelListCommand());
		GetControlnetRequest->SetVerb(TEXT("GET"));
		GetControlnetRequest->ProcessRequest();
	}
	else
	{
		OnFail.Broadcast(FSDAPI_OptionDataStruct(), FSDAPI_Response_ModelListData(), FSDAPI_Controlnet_ModelList());
	}
}

void UAsyncTask_SDAPI_ConnectAndInitialize::HandleConfigRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	//.쿼리가 모델 목록을 가져다주면 그걸 배열로 저장해서 SubSystem에 전달.
	bool bResult = false;
	RecivedConfig = true;
	RemoveFromRoot();
	if (IsValid(RecivedSubSystem))
	{
		if (bSucceeded && HttpResponse.IsValid() && HttpResponse->GetContentLength() > 0)
		{
			FString ResponseStr = HttpResponse->GetContentAsString();
			
			TSharedPtr<FJsonObject> JsonObject;
			TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(ResponseStr);
			if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
			{
				FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &ConfigData, 0, 0);
				RecivedSubSystem->SetCurrentAPIconfig(ConfigData);
				SuccessGetConfig = true;
			}
		}
	}
	CheckConnectionComplete();
}

void UAsyncTask_SDAPI_ConnectAndInitialize::HandleModelListRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	bool bResult = false;
	RemoveFromRoot();
	RecivedModelList = true;
	if (HttpResponse.IsValid())
	{
		FString ResponseStr = HttpResponse->GetContentAsString();
		TArray<TSharedPtr<FJsonValue>> JsonArray;
		TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(ResponseStr);

		if (IsValid(RecivedSubSystem))
		{
			if (FJsonSerializer::Deserialize(JsonReader, JsonArray))
			{
				for (const auto& JsonValue : JsonArray)
				{
					// FStruct로 변환
					FSDAPI_Response_ModelData MyStruct;
					FJsonObjectConverter::JsonObjectToUStruct(JsonValue->AsObject().ToSharedRef(), &MyStruct, 0, 0);
					ResponseModelData.Payload.Add(MyStruct);
				}
				RecivedSubSystem->SetAvailableModel(ResponseModelData.Payload);
				SuccessGetModelList = true;
			}
		}
	}
	CheckConnectionComplete();
}

void UAsyncTask_SDAPI_ConnectAndInitialize::HandleLoraListRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	bool bResult = false;
	RemoveFromRoot();
	RecivedLoraList = true;
	if (HttpResponse.IsValid())
	{
		FString ResponseStr = HttpResponse->GetContentAsString();
		TArray<TSharedPtr<FJsonValue>> JsonArray;
		TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(ResponseStr);
		
		if (IsValid(RecivedSubSystem))
		{
			if (FJsonSerializer::Deserialize(JsonReader, JsonArray))
			{
				for (const auto& JsonValue : JsonArray)
				{
					// FStruct로 변환
					FSDAPI_Response_LoraData MyStruct;
					FJsonObjectConverter::JsonObjectToUStruct(JsonValue->AsObject().ToSharedRef(), &MyStruct, 0, 0);
					ResponseLoraData.Payload.Add(MyStruct);
				}
				RecivedSubSystem->SetAvailableLoras(ResponseLoraData.Payload);
				SuccessGetLoraList = true;
			}
		}
	}
	CheckConnectionComplete();
}

void UAsyncTask_SDAPI_ConnectAndInitialize::HandleControlnetModelListRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{

	bool bResult = false;
	RemoveFromRoot();
	RecivedControlnet = true;

	if (HttpResponse.IsValid())
	{
		FString ResponseStr = HttpResponse->GetContentAsString();
	
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(ResponseStr);
		
		if (IsValid(RecivedSubSystem))
		{
			if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
			{
				FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &ControlentModelData, 0, 0);
				RecivedSubSystem->SetAvailableControlnetModel(ControlentModelData.model_list);

				if (ControlentModelData.model_list.Num() > 0)
				{
					RecivedSubSystem->SetCanUseControlnet(true);
				}
				else
				{
					RecivedSubSystem->SetCanUseControlnet(false);
				}
			}
		}
	}
	CheckConnectionComplete();
}

UAsyncTask_SDAPI_GetLoraInfo::UAsyncTask_SDAPI_GetLoraInfo(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	if (HasAnyFlags(RF_ClassDefaultObject) == false)
	{
		AddToRoot();
	}
}

UAsyncTask_SDAPI_GetLoraInfo* UAsyncTask_SDAPI_GetLoraInfo::SDAPI_GetLoraInfo(USDAI_Subsystem* SubSystem, FString LoraName)
{
	UAsyncTask_SDAPI_GetLoraInfo* DownloadTask = NewObject<UAsyncTask_SDAPI_GetLoraInfo>();
	if (IsValid(SubSystem) && SubSystem->IsInitialized())
	{
		DownloadTask->GetLoraInfo(SubSystem,LoraName);
	}
	return DownloadTask;
}

void UAsyncTask_SDAPI_GetLoraInfo::GetLoraInfo(USDAI_Subsystem* SubSystem, FString LoraName)
{
	USDAPI_DeveloperSettings* Settings = USDAPI_DeveloperSettings::Get();
	if (IsValid(Settings) && IsValid(SubSystem))
	{
		RecivedLoraName = LoraName;

		RecivedSubSystem = SubSystem;
		//. 
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> GetSD_ModelListRequest = FHttpModule::Get().CreateRequest();

		GetSD_ModelListRequest->OnProcessRequestComplete().BindUObject(this, &UAsyncTask_SDAPI_GetLoraInfo::HandleLoraInfoRequest);
		GetSD_ModelListRequest->SetURL(Settings->GetLoraInfoURL(RecivedSubSystem->GetCurrentConenctedUrl(), LoraName));
		GetSD_ModelListRequest->SetVerb(TEXT("GET"));
		GetSD_ModelListRequest->ProcessRequest();

		return;
	}

	OnFail.Broadcast(InfoData);
}

void UAsyncTask_SDAPI_GetLoraInfo::HandleLoraInfoRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	if (HttpResponse.IsValid())
	{
		FString ResponseStr = HttpResponse->GetContentAsString();

		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(ResponseStr);

		if (IsValid(RecivedSubSystem))
		{
			if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
			{
				
				FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &InfoData, 0, 0);

				InfoData.activation_text = JsonObject->GetStringField("activation text");
				InfoData.preferred_weight = JsonObject->GetStringField("preferred weight");
				InfoData.sd_version = JsonObject->GetStringField("sd version");

				RecivedSubSystem->SetLoraInfoData(RecivedLoraName, InfoData);

				OnSuccess.Broadcast(InfoData);
				return;
			}
		}
	}

	OnFail.Broadcast(InfoData);
}

void UAsyncTask_SDAPI_ConnectAndInitialize::CheckConnectionComplete()
{
	if (RecivedConfig && RecivedModelList && RecivedControlnet && RecivedLoraList)
	{
		if (SuccessGetModelList && SuccessGetConfig)
		{
			if (IsValid(RecivedSubSystem))
			{
				RecivedSubSystem->CurrentState = ESDAPI_SubsystemState::API_CONNECTED_AND_INITAILZED;
			}
			OnSuccess.Broadcast(ConfigData, ResponseModelData, ControlentModelData);
			
		}
		else
		{
			if (IsValid(RecivedSubSystem))
			{
				RecivedSubSystem->CurrentState = ESDAPI_SubsystemState::API_NOT_CONNECTED;
				RecivedSubSystem->CurrentConenctedUrl = "";
			}
			OnFail.Broadcast(FSDAPI_OptionDataStruct(), FSDAPI_Response_ModelListData(), FSDAPI_Controlnet_ModelList());
			
		}
	}
}

UAsyncTask_SDAPI_SetModelAndConfig::UAsyncTask_SDAPI_SetModelAndConfig(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	if (HasAnyFlags(RF_ClassDefaultObject) == false)
	{
		AddToRoot();
	}
}

UAsyncTask_SDAPI_SetModelAndConfig* UAsyncTask_SDAPI_SetModelAndConfig::SDAPI_SetModelAndConfig(USDAI_Subsystem* SubSystem, FSDAPI_OptionDataStruct Option)
{
	UAsyncTask_SDAPI_SetModelAndConfig* InitailizedTask = NewObject<UAsyncTask_SDAPI_SetModelAndConfig>();
	InitailizedTask->Start(SubSystem, Option);

	return InitailizedTask;
}

void UAsyncTask_SDAPI_SetModelAndConfig::Start(USDAI_Subsystem* SubSystem, FSDAPI_OptionDataStruct Option)
{
	USDAPI_DeveloperSettings* Settings = USDAPI_DeveloperSettings::Get();

	if (IsValid(Settings) && IsValid(SubSystem))
	{
		RecivedSubSystem = SubSystem;

		// FStruct를 JSON으로 직렬화
		FString JsonString;
		FJsonObjectConverter::UStructToJsonObjectString(Option.StaticStruct(), &Option, JsonString, 0, 0);

		// JSON String을 Payload로 설정
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> GetSD_SetConfigRequest = FHttpModule::Get().CreateRequest();
		GetSD_SetConfigRequest->OnProcessRequestComplete().BindUObject(this, &UAsyncTask_SDAPI_SetModelAndConfig::HandleConfigRequest);
		GetSD_SetConfigRequest->SetURL(SubSystem->CurrentConenctedUrl + Settings->GetOptionCommand());
		GetSD_SetConfigRequest->SetContentAsString(JsonString);
		GetSD_SetConfigRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json;"));
		GetSD_SetConfigRequest->SetVerb(TEXT("POST"));
		GetSD_SetConfigRequest->ProcessRequest();
	}
	else
	{
		OnFail.Broadcast(SubSystem->GetCurrentAPIconfig());

		RecivedSubSystem->CurrentState = ESDAPI_SubsystemState::API_NOT_CONNECTED;
	}
}

void UAsyncTask_SDAPI_SetModelAndConfig::HandleConfigRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	RemoveFromRoot();
	FSDAPI_OptionDataStruct Data;
	UE_LOG(LogTemp, Log, TEXT("UAPI_SetModelAndConfig::HandleConfigRequest called"));
	if (HttpResponse.IsValid())
	{
		if (HttpResponse->GetResponseCode() == 200)
		{
			if (IsValid(RecivedSubSystem))
			{
				RecivedSubSystem->CurrentState = ESDAPI_SubsystemState::API_CONNECTED_AND_INITAILZED;
				OnSuccess.Broadcast(RecivedSubSystem->GetCurrentAPIconfig());
				return;
			}
		}
	}
	RecivedSubSystem->CurrentState = ESDAPI_SubsystemState::API_NOT_CONNECTED;
	OnFail.Broadcast(FSDAPI_OptionDataStruct());
}
