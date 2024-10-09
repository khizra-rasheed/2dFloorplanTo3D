// Copyright by CodeParrots 2023. All Rights Reserved.


#include "SDAPI_Statics.h"
#include "Engine/Texture.h"
#include "Engine/Texture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/Texture2DDynamic.h"
#include "Misc/FileHelper.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Rendering/Texture2DResource.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/Texture2DDynamic.h"
#include "Kismet/KismetSystemLibrary.h"

#if WITH_EDITOR

#include "Developer/AssetTools/Private/AssetTools.h"
#include "FileHelpers.h"
#endif

#include "Serialization/BulkData.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "ImageUtils.h"
#include "Components/Image.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "SDAI_Subsystem.h"
#include "Interfaces/IHttpResponse.h"

#include "HttpModule.h"



FString USDAPI_Statics::MakeRenderTarget2Base64EncodeData(UTextureRenderTarget2D* RenderTarget)
{
	if (RenderTarget == nullptr)
	{
		return FString();
	}

	FImage Image;
	FImageUtils::GetRenderTargetImage(RenderTarget, Image);
	TArray64<uint8> CompressedData;
	if (FImageUtils::CompressImage(CompressedData, TEXT("JPG"), Image))
	{
		FString Base64String;
		Base64String = FBase64::Encode(CompressedData.GetData(), CompressedData.Num());
		return Base64String;
	}
	return FString();
}


FString USDAPI_Statics::MakeTexture2Base64EncodeData(UTexture2D* Texture)
{
	if (!Texture || !Texture->GetResource() || !Texture->GetResource()->TextureRHI)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid texture"));
		return FString();
	}

	// Only supporting uncompressed textures for now
	if (Texture->GetPlatformData() == nullptr)
	{
		//. maybe Dxt1 to PNG
		return FString();
	}

	if (Texture->GetPlatformData()->PixelFormat != PF_B8G8R8A8)
	{
		Texture = ConvertPixelFormatToB8G8R8A8(Texture);
	}

	// Texture2D의 너비와 높이를 가져옴
	const int32 Width = Texture->GetSizeX();
	const int32 Height = Texture->GetSizeY();

	// Texture2D의 픽셀 데이터를 FColor 배열로 캡처
	TArray<FColor> Pixels;
	Pixels.SetNumUninitialized(Width * Height);
	FTexture2DMipMap& Mip = Texture->GetPlatformData()->Mips[0];
	// Ensure texture has data loaded

#if WITH_EDITOR
	if (!Mip.BulkData.IsBulkDataLoaded())
	{
		Mip.BulkData.LoadBulkDataWithFileReader();
	}

#endif

	void* Data = nullptr;

	if (Mip.BulkData.IsLocked() == false)
	{

		Data = Mip.BulkData.Lock(LOCK_READ_ONLY);
	}
	else
	{
		Mip.BulkData.Unlock();
		Data = Mip.BulkData.Lock(LOCK_READ_ONLY);
	}
	// Check if the texture data is valid before copying
	if (Data)
	{
		FMemory::Memcpy(Pixels.GetData(), Data, Width * Height * sizeof(FColor));
		Mip.BulkData.Unlock();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to lock texture data"));
		return FString();
	}

	// ImageWrapperModule를 사용하여 JPG 이미지를 인코딩
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);
	ImageWrapper->SetRaw(Pixels.GetData(), Pixels.GetAllocatedSize(), Width, Height, ERGBFormat::BGRA, 8);
	const TArray64<uint8>& EncodedData = ImageWrapper->GetCompressed(100);

	FString Base64String;
	Base64String = FBase64::Encode(EncodedData.GetData(), EncodedData.Num());

	return Base64String;
}

UTexture2D* USDAPI_Statics::ConvertPixelFormatToB8G8R8A8(UTexture2D* MyTexture2D)
{
	TextureCompressionSettings OldCompressionSettings = MyTexture2D->CompressionSettings;
	bool OldSRGB = MyTexture2D->SRGB;
	MyTexture2D->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
#if WITH_EDITOR
	MyTexture2D->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
#endif
	MyTexture2D->SRGB = false;
	MyTexture2D->UpdateResource();

	int32 width = MyTexture2D->GetPlatformData()->SizeX;
	int32 height = MyTexture2D->GetPlatformData()->SizeY;

	const FColor* FormatedImageData = static_cast<const FColor*>(MyTexture2D->GetPlatformData()->Mips[0].BulkData.LockReadOnly());
	MyTexture2D->GetPlatformData()->Mips[0].BulkData.Unlock();

	int32 Width = MyTexture2D->GetPlatformData()->SizeX;
	int32 Height = MyTexture2D->GetPlatformData()->SizeY;
	TArray<FColor> ColorArray;
	ColorArray.SetNumUninitialized(Width * Height);

	for (int32 X = 0; X < Width; X++)
	{
		for (int32 Y = 0; Y < Height; Y++)
		{
			FColor PixelColor = FormatedImageData[Y * Width + X];
			ColorArray[Y * Width + X] = PixelColor;
		}
	}

	if (ColorArray.Num() != Width * Height)
	{
		UE_LOG(LogTemp, Error, TEXT("ColorArray size does not match the specified width and height."));
		return nullptr;
	}

	UTexture2D* NewTexture = UTexture2D::CreateTransient(Width, Height, PF_B8G8R8A8);
	if (!NewTexture)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create transient texture."));
		return nullptr;
	}

	FTexture2DMipMap& MipMap = NewTexture->GetPlatformData()->Mips[0];
	uint8* DestPixels = static_cast<uint8*>(MipMap.BulkData.Lock(LOCK_READ_WRITE));

	for (int32 y = 0; y < Height; y++)
	{
		for (int32 x = 0; x < Width; x++)
		{
			int32 Index = (y * Width + x);
			FColor Color = ColorArray[Index];
			reinterpret_cast<FColor*>(DestPixels)[Index] = Color;
		}
	}

	MipMap.BulkData.Unlock();

	NewTexture->UpdateResource();

	return NewTexture;
}

UTexture2D* USDAPI_Statics::Texture2DDynamicToTexture2D(UTexture2DDynamic* DynamicTexture)
{
	UTexture2D* NewTexture = nullptr;

	if (IsValid(DynamicTexture) && DynamicTexture->GetResource() != nullptr)
	{
		NewTexture = UTexture2D::CreateTransient(DynamicTexture->GetResource()->GetSizeX(), DynamicTexture->GetResource()->GetSizeY(), PF_B8G8R8A8);
		if (!NewTexture)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to create transient texture."));
			return nullptr;
		}

		NewTexture->SetResource(DynamicTexture->GetResource());
		NewTexture->UpdateResource();

	}

	return NewTexture;
}
#if WITH_EDITOR


// UEditorAssetSubsystem* EditorAssetSubsystem = GEditor->GetEditorSubsystem<UEditorAssetSubsystem>();
// 
// if (IsValid(Texture))
// {
// 	FString FilePath = SaveDirectory;
// 	FilePath += TextureName;
// 	bool bAlreadyExist = EditorAssetSubsystem->DoesAssetExist(FilePath);
// 
// 	int32 Num = 0;
// 	while (bAlreadyExist)
// 	{
// 		FilePath = FilePath + "_" + FString::FromInt(Num++);
// 		bAlreadyExist = EditorAssetSubsystem->DoesAssetExist(FilePath);
// 	}
// 
// 	UPackage* Package = CreatePackage(NULL, *FilePath);
// 	UObject* NewObject = Texture;
// 	NewObject->SetFlags(EObjectFlags::RF_Public | EObjectFlags::RF_Standalone);
// 	NewObject->SetExternalPackage(Package);
// 	FAssetRegistryModule::AssetCreated(NewObject);
// 	NewObject->MarkPackageDirty();
// 	NewObject->PostEditChange();
// 	Package->FullyLoad();
// 	FString PackageFileName = FPackageName::LongPackageNameToFilename(FilePath, FPackageName::GetAssetPackageExtension());
// 	bResult = UPackage::SavePackage(Package, NewObject, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone, *PackageFileName, GError, nullptr, true, true, SAVE_None);
// 
// 
// }

UTexture2D* USDAPI_Statics::SaveAsImageFileAndImportAsset(UTexture2D* ConvertedTexture, FString SaveFilePath, FString FileName)
{
	FString ContentFolderPath = FPaths::ProjectContentDir();

	SaveFilePath = RefineSaveFilePath(SaveFilePath);

	FString RawSaveFilePath = ContentFolderPath + SaveFilePath;

	FString TextureFileName = "";

	if (FileName == "")
	{
		TextureFileName = RawSaveFilePath + "/" + ConvertedTexture->GetName();
	}
	else
	{
		TextureFileName = RawSaveFilePath + "/" + FileName;
	}

	USDAPI_Statics::SaveAsImageFIle(ConvertedTexture, TextureFileName);


#if WITH_EDITOR

	UTexture2D* Texture = FImageUtils::ImportFileAsTexture2D(TextureFileName);
	if (IsValid(Texture))
	{
		Texture->AddToRoot();

		/*FString FilePath = SaveFilePath;*/
		TextureFileName.RemoveFromEnd(".png");
		FString CleanFileName = FPaths::GetCleanFilename(TextureFileName);
		Texture = Cast<UTexture2D>(USDAPI_Statics::Save_Texture_As_Asset(CleanFileName, Texture, RawSaveFilePath, SaveFilePath));
	}

#endif
	return Texture;
}

void USDAPI_Statics::SaveAsImageFIle(UTexture2D* Texture, FString& OutFilePath)
{
	if (!Texture || !Texture->GetResource() /*|| !Texture->GetResource()->TextureRHI*/)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid texture"));
		return;
	}

	// Only supporting uncompressed textures for now
	if (Texture->GetPlatformData() == nullptr ||
		Texture->GetPlatformData()->PixelFormat != PF_B8G8R8A8)
	{
		Texture = ConvertPixelFormatToB8G8R8A8(Texture);
	}

	FTexture2DMipMap& Mip = Texture->GetPlatformData()->Mips[0];
	const void* Data = Mip.BulkData.LockReadOnly();
	int32 Width = Mip.SizeX;
	int32 Height = Mip.SizeY;
	TArray<FColor> PixelData;
	PixelData.SetNumUninitialized(Width * Height);
	FMemory::Memcpy(PixelData.GetData(), Data, Width * Height * sizeof(FColor));
	Mip.BulkData.Unlock();

	EPixelFormat TextureFormat = Texture->GetPixelFormat(0);

	// Convert FColor data to RGBA8 data
	TArray<uint8> RawData_PNG32;
	RawData_PNG32.SetNumUninitialized(PixelData.Num() * sizeof(FColor));
	uint8* DestPtr = RawData_PNG32.GetData();
	const FColor* SrcPtr = PixelData.GetData();
	for (int32 i = 0; i < PixelData.Num(); i++)
	{
		*DestPtr++ = (uint8)(SrcPtr->B);
		*DestPtr++ = (uint8)(SrcPtr->G);
		*DestPtr++ = (uint8)(SrcPtr->R);
		*DestPtr++ = (uint8)(SrcPtr->A);
		SrcPtr++;
	}

	// Create the image wrapper
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);

	// Set the raw data
	ImageWrapper->SetRaw(RawData_PNG32.GetData(), RawData_PNG32.Num(), Width, Height, ERGBFormat::BGRA, 8);

	// Encode the image
	TArray64<uint8> CompressedData = ImageWrapper->GetCompressed();

	FString ExtentsionWord = ".png";
	bool bExist = FPaths::FileExists(OutFilePath + ExtentsionWord);

	int32 Num = 0;

	while (bExist)
	{
		FString TestPath = OutFilePath + "_" + FString::FromInt(++Num);
		bExist = FPaths::FileExists(TestPath + ExtentsionWord);
		if (bExist == false)
		{
			OutFilePath = OutFilePath + "_" + FString::FromInt(Num);
		}
	}

	OutFilePath += ".png";
	FFileHelper::SaveArrayToFile(CompressedData, *OutFilePath);
}

UTexture2D* USDAPI_Statics::Save_Texture_As_Asset(FString TextureName, UTexture2D* Texture, FString RawSaveFilePath, FString ProjectFilePath)
{
	FString ContentFolderPath = FPaths::ProjectContentDir();

	RawSaveFilePath = RefineSaveFilePath(RawSaveFilePath);

	FString ProjectDir = ProjectFilePath;
	ProjectDir.RemoveFromStart(ContentFolderPath);
	ProjectDir = "/Game/" + ProjectDir;

	bool bResult = false;
	FString ImportFilePath = RawSaveFilePath + "/" + TextureName+".png";
	FString Message;
	UObject* Object = nullptr;

#if WITH_EDITOR
	UAssetImportTask* Task = CreateImportTask(ImportFilePath, ProjectDir, nullptr, nullptr, bResult, Message);
	if (bResult)
	{
		Task->bAsync = false;
		Object = ProcessImportTask(Task, bResult, Message);
	}
#endif

	return Cast< UTexture2D>(Object);
}

FString USDAPI_Statics::RefineSaveFilePath(FString SaveFilePath)
{
	if (SaveFilePath == "" && USDAPI_DeveloperSettings::Get())
	{
		return USDAPI_DeveloperSettings::Get()->GetDefaultSavePath();
	}

	SaveFilePath.RemoveFromStart("/");
	SaveFilePath.RemoveFromStart("Game/");
	SaveFilePath.RemoveFromStart("Content/");

	SaveFilePath.RemoveFromEnd("/");

	return SaveFilePath;
}

UAssetImportTask* USDAPI_Statics::CreateImportTask(FString FilePath, FString DestinationPath, UFactory* ExtraFactory, UObject* ExtraOptions, bool& bOutSuccess, FString& OutInfoMessage)
{
	UAssetImportTask* RetTask = NewObject<UAssetImportTask>();

	if (RetTask == nullptr)
	{
		bOutSuccess = false;

		OutInfoMessage = FString::Printf(TEXT("Create Import Task Failed"));
		return nullptr;
	}

	FString FilePathCopied = FilePath;

	RetTask->Filename = FilePath;
	FilePathCopied.RemoveFromEnd(".png");
	RetTask->DestinationPath = DestinationPath;
	RetTask->DestinationName = FPaths::GetCleanFilename(FilePathCopied);

	RetTask->bSave = true;
	RetTask->bAutomated = true;
	RetTask->bAsync = false;
	RetTask->bReplaceExisting = true;
	RetTask->bReplaceExistingSettings = false;
	RetTask->bAsync = false;
	RetTask->Factory = ExtraFactory;
	RetTask->Options = ExtraOptions;

	bOutSuccess = true;
	OutInfoMessage = FString::Printf(TEXT("Create Import Task Success"));

	return RetTask;
}

#if WITH_EDITOR
UObject* USDAPI_Statics::ProcessImportTask(UAssetImportTask* ImportTask, bool& bOutSuccess, FString& OutInfoMessage)
{
	if (ImportTask == nullptr)
	{
		bOutSuccess = false;
		OutInfoMessage = FString::Printf(TEXT("Create Import Task Failed - task invalied"));
	}

	FAssetToolsModule* AssetTools = FModuleManager::LoadModulePtr<FAssetToolsModule>("AssetTools");

	if (AssetTools == nullptr)
	{
		bOutSuccess = false;
		OutInfoMessage = FString::Printf(TEXT("Create Import Task Failed - AssetTool Module is Invalid"));
	}

	AssetTools->Get().ImportAssetTasks({ ImportTask });
	if (ImportTask->GetObjects().Num() == 0)
	{
		bOutSuccess = false;
		OutInfoMessage = FString::Printf(TEXT("Create Import Task Failed - AssetType Unsurpported"));
	}

	UObject* ImportedAsset = StaticLoadObject(UObject::StaticClass(), nullptr, *FPaths::Combine(ImportTask->DestinationPath, ImportTask->DestinationName));
	if (IsValid(ImportedAsset))
	{
		bOutSuccess = true;
		return ImportedAsset;
	}
	else
		return nullptr;
}
#endif

UMaterialInstanceConstant* USDAPI_Statics::CreateInstanceMaterial(TSoftObjectPtr<UMaterial> MasterMaterial_SoftRef, UTexture2D* Texture, FString TextureParameterName,const FString& InstanceDestination,  FString& AssetName)
{

	UTexture2D* AssetTexture = SaveAsImageFileAndImportAsset(Texture, InstanceDestination);
	FString RefinedInstanceDestination = InstanceDestination;
	//FString RefinedInstanceDestination = RefineSaveFilePath(InstanceDestination);

	while (RefinedInstanceDestination.RemoveFromStart("/"))
	{

	}

	RefinedInstanceDestination = "/" + RefinedInstanceDestination;

	if (IsValid(AssetTexture) == false)
	{
		return nullptr;
	}

	FString MasterMaterialPath = MasterMaterial_SoftRef.ToSoftObjectPath().ToString();

	UMaterialInterface* MasterMaterial = Cast<UMaterialInterface>(UEditorAssetLibrary::LoadAsset(MasterMaterialPath));
	if (MasterMaterial == nullptr)
		return nullptr;


	if (AssetName.IsEmpty())
	{
		AssetName = AssetTexture->GetName() + "_Mat";
	}

	bool bIsExist = false;

	bIsExist = UEditorAssetLibrary::DoesAssetExist(RefinedInstanceDestination + "/" + AssetName);

	int32 Num = 0;
	while (bIsExist)
	{
		bIsExist = UEditorAssetLibrary::DoesAssetExist(RefinedInstanceDestination + "/" + AssetName + "_" + FString::FromInt(Num));
		if (bIsExist == false)
		{
			Num++;
		}
		else
		{
			AssetName = AssetName + "_" + FString::FromInt(Num);
		}
	}

	IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
	UMaterialInstanceConstantFactoryNew* Factory = NewObject<UMaterialInstanceConstantFactoryNew>();
	UObject* Obj = AssetTools.CreateAsset(AssetName, RefinedInstanceDestination, UMaterialInstanceConstant::StaticClass(), Factory);
	UMaterialInstanceConstant* MaterialInstance = Cast<UMaterialInstanceConstant>(Obj);

	// 	UPackage* Package = CreatePackage(NULL, *FilePath);
// 	UObject* NewObject = Texture;
// 	NewObject->SetFlags(EObjectFlags::RF_Public | EObjectFlags::RF_Standalone);
// 	NewObject->SetExternalPackage(Package);
// 	FAssetRegistryModule::AssetCreated(NewObject);
// 	NewObject->MarkPackageDirty();
// 	NewObject->PostEditChange();
// 	Package->FullyLoad();

	if (IsValid(MaterialInstance))
	{
		UMaterialEditingLibrary::SetMaterialInstanceParent(MaterialInstance, MasterMaterial);
		if (MaterialInstance)
		{
			FMaterialParameterInfo ParameterInfo(*TextureParameterName);
		
			MaterialInstance->SetTextureParameterValueEditorOnly(ParameterInfo, AssetTexture);
			
			MaterialInstance->SetFlags(RF_Standalone);
			MaterialInstance->MarkPackageDirty();
			MaterialInstance->PostEditChange();
		}
	}
	FAssetRegistryModule::AssetCreated(MaterialInstance);
	MaterialInstance = Cast<UMaterialInstanceConstant>(UEditorAssetLibrary::LoadAsset(RefinedInstanceDestination +"/"+ AssetName));
	return MaterialInstance;
}

#endif
// UMaterial에서 UTexture 또는 UTexture2D 소스를 추출하는 함수
UTexture2D* USDAPI_Statics::GetTextureFromMaterial(UMaterialInterface* MaterialInterface)
{
	// 소스 텍스처를 저장할 UTexture2D 포인터를 초기화합니다.
	UTexture2D* SourceTexture = nullptr;
	UTexture* BaseTexture = nullptr;


	UMaterial* Mat = Cast< UMaterial>(MaterialInterface);
	if (IsValid(Mat))
	{
		FHashedMaterialParameterInfo Info;
		Mat->GetTextureParameterValue(FName("BaseTexure"), BaseTexture, false);

		SourceTexture = Cast< UTexture2D>(BaseTexture);
		if (IsValid(SourceTexture))
		{
			return SourceTexture;
		}
	}
	else
	{
		UMaterialInstance* MatInst = Cast<UMaterialInstance>(MaterialInterface);

		if (IsValid(MatInst) && MatInst->TextureParameterValues.Num() > 0)
		{
			FTextureParameterValue Value= MatInst->TextureParameterValues[0];

			SourceTexture = Cast< UTexture2D>(Value.ParameterValue);
			if (IsValid(SourceTexture))
			{
				return SourceTexture;
			}
		}
	}


	TArray<UTexture*> Textures;
	
	if (IsValid(MaterialInterface))
	{
		

		MaterialInterface->GetUsedTextures(Textures, EMaterialQualityLevel::High, true, ERHIFeatureLevel::Num, true);

		if (Textures.Num() > 0)
		{
			for (UTexture* Texture : Textures)
			{
				SourceTexture = Cast< UTexture2D>(Texture);
				if (IsValid(SourceTexture))
				{
					return SourceTexture;
				}
			}
		}
	}

	// 소스 텍스처를 반환합니다.
	return SourceTexture;
}
