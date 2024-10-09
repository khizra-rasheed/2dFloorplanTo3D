// Copyright by CodeParrots 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Interfaces/IHttpRequest.h"
#include "SDAI_Subsystem.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Materials/MaterialInstance.h"
#include "Materials/MaterialInstanceConstant.h"

#if WITH_EDITOR
#include "Subsystems/EditorAssetSubsystem.h"
#include "Factories/Texture2dFactoryNew.h"
#include "Factories/MaterialInstanceConstantFactoryNew.h"
#include "AssetImportTask.h"
#include "AssetToolsModule.h"
#include "Editor/MaterialEditor/Public/MaterialEditingLibrary.h"
#include "EditorAssetLibrary.h"
#endif

#include "SDAPI_Statics.generated.h"

/**
 * 
 */

class UImage;

UCLASS(config = SDAPI, Category = "SD_API")
class STABLEDIFFUSIONSUBSYSTEM_API USDAPI_Statics : public UObject
{

	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, Category = "SD_API")
	static FString MakeTexture2Base64EncodeData(UTexture2D* Texture);
	UFUNCTION(BlueprintCallable, Category = "SD_API")
	static UTexture2D* ConvertPixelFormatToB8G8R8A8(UTexture2D* SourceTexture);
	UFUNCTION(BlueprintCallable, Category = "SD_API")
	static FString MakeRenderTarget2Base64EncodeData(UTextureRenderTarget2D* RenderTarget);

	UFUNCTION(BlueprintCallable, Category = "SD_API")
	UTexture2D* Texture2DDynamicToTexture2D(UTexture2DDynamic* Texture);

	
#if WITH_EDITOR

	UFUNCTION(BlueprintCallable, Category = "SD_API")
		static UTexture2D* SaveAsImageFileAndImportAsset(UTexture2D* ConvertedTexture, FString SaveFilePath, FString FileName = "");
	
	UFUNCTION(BlueprintCallable, Category = "SD_API")
	static void SaveAsImageFIle(UTexture2D* Texture, FString& OutFilePath);

	UFUNCTION(BlueprintCallable, Category = "SD_API")
	static UTexture2D* Save_Texture_As_Asset(FString TextureName, UTexture2D* Texture, FString RawSaveFilePath, FString ProjectFilePath);


	static FString RefineSaveFilePath(FString SaveFilePath);

	static UAssetImportTask* CreateImportTask(FString FilePath, FString DestinationPath, UFactory* ExtraFactory, UObject* ExtraOptions, bool& bOutSuccess, FString& OutInfoMessage);

#if WITH_EDITOR

	static UObject* ProcessImportTask(UAssetImportTask* ImportTask,bool& bOutSuccess, FString& OutInfoMessage);

#endif
	UFUNCTION(BlueprintCallable, Category = "SD_API")
	static UMaterialInstanceConstant* CreateInstanceMaterial(TSoftObjectPtr<UMaterial> MasterMaterial_SoftRef, UTexture2D* Texture, FString TextureParameterName, const FString& InstanceDestination,  FString& AssetName);

#endif
	UFUNCTION(BlueprintCallable, Category = "SD_API")
	static UTexture2D* GetTextureFromMaterial(UMaterialInterface* MaterialInterface);
};

// MyButtonAction.h
// 
// #pragma once
// 
// #include "CoreMinimal.h"
// #include "UObject/Interface.h"
// 
// class UMG_API IMyButtonAction : public IInterface
// {
// public:
// 	virtual void ExecuteAction(UButton* Button) = 0;
// };
// 
// MyButtonAction.cpp
// 
// #include "MyButtonAction.h"
// 
// class UMyButtonAction_Implementation : public IMyButtonAction
// {
// public:
// 	virtual void ExecuteAction(UButton* Button) override
// 	{
// 		// Do something with the button
// 	}
// };
// 
// RegisterUIAction.cpp
// 
// void RegisterUIAction()
// {
// 	// Register the MyButtonAction class with the context menu
// 	FUIAction UIAction;
// 	UIAction.Name = "MyButtonAction";
// 	UIAction.Class = UMyButtonAction_Implementation::StaticClass();
// 	FUIActionSet::Get().RegisterAction(UIAction);
// }
