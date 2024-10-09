// Copyright by CodeParrots 2023. All Rights Reserved.

#include "SDAPI_ImgCreateWidget.h"
#include "SDAPI_Statics.h"
#include "Misc/PackageName.h"
#include "Engine/Texture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Misc/FileHelper.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Rendering/Texture2DResource.h"
#include "ImageUtils.h"
#include "Components/Image.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Engine/Texture2DDynamic.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "UObject/NoExportTypes.h"

void USDAPI_ImgCreateWidget::SetImageTexture(UTexture* Texture)
{
	ResultImage->SetBrushResourceObject(Texture);
	MakedImageHistory.Add(Texture);
}

void USDAPI_ImgCreateWidget::ClearTexture()
{
	ResultImage->Brush.SetResourceObject(nullptr);
}

void USDAPI_ImgCreateWidget::Save_Texture_As_Asset(FString TextureName, FString SaveFilePath)
{
	if (ResultImage != nullptr && ResultImage->Brush.GetResourceObject() != nullptr)
	{
		UTexture* ResourceTexture = Cast<UTexture>(ResultImage->Brush.GetResourceObject());
		UTexture2D* ConvertedTexture = Cast<UTexture2D>(ResourceTexture);
		UTexture2DDynamic* ConvertedDynamic = Cast<UTexture2DDynamic>(ResourceTexture);

		if (IsValid(ConvertedTexture) == false && IsValid(ConvertedDynamic) == false)
		{
			return;
		}

		if (IsValid(ConvertedTexture))
		{
			USDAPI_Statics::SaveAsImageFileAndImportAsset(ConvertedTexture, SaveFilePath , TextureName);
		}
	}
}
