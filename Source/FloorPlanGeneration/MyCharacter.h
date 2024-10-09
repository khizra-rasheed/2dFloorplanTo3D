#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "opencv2/highgui.hpp"
#include "opencv2/xfeatures2d/nonfree.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/core.hpp"
#include "opencv2/dnn.hpp"
#include "MyCharacter.generated.h"

USTRUCT(BlueprintType)
struct FBlackPixelRange {
	GENERATED_BODY()
		UPROPERTY(BlueprintReadOnly)
		int32 Start;

	UPROPERTY(BlueprintReadOnly)
		int32 End;

	UPROPERTY(BlueprintReadOnly)
		TArray<int32> Rows;

	FBlackPixelRange() : Start(0), End(0) {}
};


USTRUCT(BlueprintType)
struct FPoint
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 x;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 y;

	// Constructor
	FPoint() : x(0), y(0) {}
};

USTRUCT(BlueprintType)
struct FWallsData
{
	GENERATED_BODY()
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FPoint> WallsArray;
	
};

USTRUCT(BlueprintType)
struct FBoxes
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 x;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 y;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 w;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 h;

	// Constructor
	FBoxes() : x(0), y(0), w(0), h(0) {}
};

USTRUCT(BlueprintType)
struct FCornerIdentifier {
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 boxNum;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 tl;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 tr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 bl;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 br;

	FCornerIdentifier() : boxNum(0) , tl(0) , tr(0) , bl(0) , br(0) {}
};

UCLASS()
class FLOORPLANGENERATION_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyCharacter();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:


	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	

	

	UPROPERTY(BlueprintReadWrite)
		int32 rowsToEscape;

	UPROPERTY(BlueprintReadWrite)
		int32 colsToEscape; 

	UPROPERTY(BlueprintReadWrite)
	TArray<int32> rows;

	UPROPERTY(BlueprintReadWrite)
	TArray<int32> cols;

	UPROPERTY(BlueprintReadWrite)
	TMap<int32,FWallsData> WallMap;
	
	UPROPERTY(BlueprintReadWrite)
	TArray<FBlackPixelRange> boxData;

	UPROPERTY(BlueprintReadWrite)
	TArray<FBoxes> boxArr;

	UPROPERTY(BlueprintReadWrite)
	TArray<FBoxes> boxEnhancedArr;



	UPROPERTY(BlueprintReadWrite)
	TArray<FBoxes> winArr;

	UPROPERTY(BlueprintReadWrite)
	TArray<FBoxes> doorArr;

	UPROPERTY(BlueprintReadWrite)
	TArray<FCornerIdentifier> doorCorners;

	TArray<FPoint> pointsData;

	
	cv::Mat& RemoveTextFromImage(cv::Mat& img, cv::Mat& output);

	

	UFUNCTION(BlueprintCallable, Category = "Image Processing")
	void ReadImageBP(const FString& Image, const FString& modelPath, int kernelSize);

	UFUNCTION(BlueprintCallable, Category = "Image Processing")
	void ReadImageFromBP(const FString& Image, const FString& modelPath);

	

	cv::Mat& ResizeImage(cv::Mat& img,cv::Mat& output);
	TArray<FBlackPixelRange> PixelToBoxData(cv::Mat& img);
	int isPointInRect(int x, int y, int h, int w, int p1,int p2);
	int32 isPointt(int32 x, int32 y, int32 h, int32 w, int32 p1, int32 p2,cv::Mat & tmp);


};
