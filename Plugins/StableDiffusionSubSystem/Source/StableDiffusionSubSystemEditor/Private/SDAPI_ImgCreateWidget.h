// Copyright by CodeParrots 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/Image.h"
#include "EditorUtilityWidget.h"
#include "SDAPI_ImgCreateWidget.generated.h"

/**
 * 
 */
UCLASS()
class USDAPI_ImgCreateWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "SD_API")
	void SetImageTexture(UTexture* Texture);


	UFUNCTION(BlueprintCallable, Category = "SD_API")
	void ClearTexture();

	//Asset save 지원 & 컨텍스트 메뉴로 띄우기 지원 & 특정 에셋 로드 및 저장 지원
	UFUNCTION(BlueprintCallable, Category = "SD_API")
	void Save_Texture_As_Asset( FString TextureName,FString SaveFilePath);

protected:
	
	UPROPERTY( BlueprintReadWrite, meta=(BindWidget), Category = "SD_API")
	UImage* ResultImage;

	UPROPERTY(BlueprintReadWrite, Category = "SD_API")
	TArray<UTexture*> MakedImageHistory;
	
};
