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

	//Asset save ���� & ���ؽ�Ʈ �޴��� ���� ���� & Ư�� ���� �ε� �� ���� ����
	UFUNCTION(BlueprintCallable, Category = "SD_API")
	void Save_Texture_As_Asset( FString TextureName,FString SaveFilePath);

protected:
	
	UPROPERTY( BlueprintReadWrite, meta=(BindWidget), Category = "SD_API")
	UImage* ResultImage;

	UPROPERTY(BlueprintReadWrite, Category = "SD_API")
	TArray<UTexture*> MakedImageHistory;
	
};
