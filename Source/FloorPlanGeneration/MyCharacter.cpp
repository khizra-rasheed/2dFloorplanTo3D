#include "MyCharacter.h"

using namespace cv;
using namespace cv::dnn;
using namespace std;
//using namespace dnn;

// Sets default values

std::vector<std::vector<cv::Point>> contours;
std::vector<cv::Vec4i> hierarchy;
std::vector<cv::Point> approx;
//cv::dnn::Net network;
vector<Mat> outputss;
std::vector<cv::Rect> boxes;
std::vector<int> classIds;
std::vector<float> confidences;
std::vector<cv::String> outputLayerNames;
std::vector<cv::Mat> layerOuts;
cv::dnn::Net result;
std::vector<cv::Vec2f> lines;
const float INPUT_WIDTH = 608;
const float INPUT_HEIGHT = 608;
const float SCORE_THRESHOLD = 0.5;
const float NMS_THRESHOLD = 0.45;
const float CONFIDENCE_THRESHOLD = 0.5;

const float FONT_SCALE = 0.7;
const int FONT_FACE = FONT_HERSHEY_SIMPLEX;
const int THICKNESS = 1;

Scalar BLACK = Scalar(0, 0, 0);
Scalar BLUE = Scalar(255, 178, 50);
Scalar YELLOW = Scalar(0, 255, 255);
Scalar RED = Scalar(0, 0, 255);
const std::vector<cv::Scalar> colors = { cv::Scalar(255, 255, 0), cv::Scalar(0, 255, 0), cv::Scalar(0, 255, 255), cv::Scalar(255, 0, 0) };

AMyCharacter::AMyCharacter()
{
	// Set this character to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();

	/*FString path = "E:/stable-diffusion-webui-master/webui.bat";
	FString cmd = "--deepdanbooru --nowebui --xformers";
	const TCHAR* ccmd = *cmd;
	const TCHAR* ppath = *path;
	void* ReadPipe = nullptr;
	void* WritePipe = nullptr;
	FPlatformProcess::CreatePipe(ReadPipe, WritePipe);
	FPlatformProcess::CreateProc(ppath, ccmd, true, false, false, nullptr, 0, nullptr, WritePipe);*/
	
	
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}






cv::Mat& AMyCharacter::RemoveTextFromImage(cv::Mat& img, cv::Mat& output)
{
	// TODO: insert return statement here
	if (true) {

		Mat grad, bw, connected;
		Mat morphKernel = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
		morphologyEx(img, grad, MORPH_GRADIENT, morphKernel);


		threshold(grad, bw, 0.0, 255.0, THRESH_BINARY | THRESH_OTSU);


		morphKernel = getStructuringElement(MORPH_RECT, Size(10, 1));
		morphologyEx(bw, connected, MORPH_CLOSE, morphKernel);


		Mat mask = Mat::zeros(bw.size(), CV_8UC1);
		Mat mask2 = Mat::zeros(img.size(), CV_8UC1);
		vector<cv::Rect> txtRect;
		vector<vector<Point> > txtContour;
		

		findContours(connected, contours, hierarchy, cv::RETR_CCOMP,
			cv::CHAIN_APPROX_SIMPLE, Point(0, 0));

		if (!contours.empty()) {
			for (int i = 0; i >= 0; i = hierarchy[i][0]) {
				cv::Rect rect = boundingRect(contours[i]);
				Mat maskROI(mask, rect);
				maskROI = Scalar(0, 0, 0);

				drawContours(mask, contours, i, Scalar(255, 255, 255), FILLED);

				double r = (double)countNonZero(maskROI) / (rect.width * rect.height);

				/* assume at least 45% of the area is filled if it contains text */
				if (r > .45 && (rect.height < 1000 && rect.width < 1000)) {
					rectangle(img, rect, Scalar(255), FILLED);
					txtRect.push_back(rect);
					txtContour.push_back(contours[i]);
				}

			}


		}



		cv::threshold(img, output, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);

		cv::Mat kernel2 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3,3));
		cv::morphologyEx(output, output, MORPH_OPEN, kernel2, Point(-1, -1), 2);
		dilate(output, output, kernel2, Point(-1, -1), 1);


		
		

		if (output.rows > 600 || output.cols > 600) {
			cv::Mat resizedImage;
			ResizeImage(output, output);

			
			UE_LOG(LogTemp, Warning, TEXT("image rows: %d"), resizedImage.rows);
			UE_LOG(LogTemp, Warning, TEXT("image cols: %d"), resizedImage.cols);
		}
		else {
			cv::imshow("img", output);
			cv::waitKey(0);
			cv::destroyAllWindows();
		}

		
		UE_LOG(LogTemp, Warning, TEXT("image rows: %d"), output.rows);
		UE_LOG(LogTemp, Warning, TEXT("image cols: %d"), output.cols);


		//refining missing corners
		
	}
	return output;
}



void AMyCharacter::ReadImageBP(const FString& Image, const FString& modelPath, int kernelSize)
{
	FString DLLPath = FPaths::ProjectDir() + *Image;
	string MyStdString = TCHAR_TO_UTF8(*DLLPath);
	String MyCvString(MyStdString.c_str());
	Mat img = imread(MyCvString, IMREAD_GRAYSCALE);
	cv::Mat outputImage(cv::Size(600, 600), img.type());
	Mat beforePimg;

	FString cfgPath = FPaths::ProjectDir() + "ThirdParty/yolov4-obj.cfg";
	string cfgCV = TCHAR_TO_UTF8(*cfgPath);
	cv::String cfg = cfgCV.c_str();
	FString weightPath = FPaths::ProjectDir() + "ThirdParty/yolov4-obj_last.weights";
	string weightCV = TCHAR_TO_UTF8(*weightPath);
	cv::String weight = weightCV.c_str();
	std::vector<std::string> class_list;
	FString classesPath = FPaths::ProjectDir();
	classesPath.Append(TEXT("ThirdParty/classes.txt"));
	TArray<FString> classes;

	//need to read file data in a list of strings
	FFileHelper::LoadFileToStringArray(classes, *classesPath);


	for (FString c : classes) {
		string wCV = TCHAR_TO_UTF8(*c);
		cv::String w = wCV.c_str();
		class_list.push_back(w);
		UE_LOG(LogTemp, Warning, TEXT("class id %s"), *c);
	}

	result = readNetFromDarknet(cfg, weight);
	result.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
	result.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
	auto model = cv::dnn::DetectionModel(result);
	model.setInputParams(1. / 255, cv::Size(608, 608), cv::Scalar(), true);
	Mat frame = imread(MyCvString);
	
	int frame_count = 0;
	float fps = -1;
	int total_frames = 0;
	if (!frame.empty()) {
		model.detect(frame, classIds, confidences, boxes, .2, .4);
		frame_count++;
		total_frames++;
		int detections = classIds.size();
		for (int i = 0; i < detections; ++i) {

			auto box = boxes[i];
			auto classId = classIds[i];
			const auto color = colors[classId % colors.size()];
			cv::rectangle(frame, box, color, 1);
			cv::rectangle(frame, cv::Point(box.x, box.y - 20), cv::Point(box.x + box.width, box.y), color, cv::FILLED);
			cv::putText(frame, class_list[classId].c_str(), cv::Point(box.x, box.y - 5), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0));
		}
		cv::imwrite("C:/Users/khizra/Documents/UnrealProjects/FloorPlanGeneration/images/mask2.png", frame);
		//cv::resize(frame, frame, Size(800, 800));
		cv::imshow("detectedWindow", frame);
		cv::waitKey(0);
		cv::destroyAllWindows();
	}


	vector<cv::Rect> dArr;
	for (int i = 0; i < classIds.size(); i++) {
		if (classIds[i] == 0) {
			FBoxes t;
			t.x = boxes[i].x; t.y = boxes[i].y; t.w = boxes[i].width; t.h = boxes[i].height;
			dArr.push_back(boxes[i]);
			doorArr.Add(t);
		}
	}

	//cv::imwrite("C:/Users/khizra/Documents/UnrealProjects/FloorPlanGeneration/images/output.png", mask);
	//cv::resize(mask, mask, Size(600, 600));


	for (cv::Rect d : boxes) {
		for (int i = d.x; i < d.x + d.width; i++) {
			for (int k = d.y; k < d.y + d.height; k++) {
				img.at<uchar>(k, i) = 255;
			}
		}
	}
	//ResizeImage(img, img);
	Mat binaryImage; // Binary image to store the result
	cv::threshold(img, binaryImage, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);

	cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(kernelSize, kernelSize));
	cv::morphologyEx(binaryImage, binaryImage, MORPH_OPEN, kernel, Point(-1, -1), 2);
	dilate(binaryImage, binaryImage, kernel, Point(-1, -1), 1);


	// Display the resized image
	cv::imshow("Resized Image", binaryImage);
	cv::waitKey(0);
	cv::destroyAllWindows();

	

	boxData.Empty();
	boxData = PixelToBoxData(binaryImage);
	rowsToEscape = (600 - binaryImage.rows) / 2;
	colsToEscape = (600 - binaryImage.cols) / 2;
}








void AMyCharacter::ReadImageFromBP(const FString& Image,const FString& modelPath)
{

	FString DLLPath = FPaths::ProjectDir() + *Image;
	string MyStdString = TCHAR_TO_UTF8(*DLLPath);
	String MyCvString(MyStdString.c_str()); 
	Mat img = imread(MyCvString,IMREAD_GRAYSCALE);
	cv::Mat outputImage(cv::Size(600, 600), img.type()); 
	Mat beforePimg;

	FString cfgPath = FPaths::ProjectDir() + "ThirdParty/yolov4-obj.cfg";
	string cfgCV = TCHAR_TO_UTF8(*cfgPath);
	cv::String cfg = cfgCV.c_str();
	FString weightPath = FPaths::ProjectDir() + "ThirdParty/yolov4-obj_last.weights";
	string weightCV = TCHAR_TO_UTF8(*weightPath);
	cv::String weight = weightCV.c_str();
	std::vector<std::string> class_list;
	FString classesPath = FPaths::ProjectDir();
	classesPath.Append(TEXT("ThirdParty/classes.txt"));

	TArray<FString> classes;
	

	//need to read file data in a list of strings
	FFileHelper::LoadFileToStringArray(classes,*classesPath);
	

	for (FString c : classes) {
		string wCV = TCHAR_TO_UTF8(*c);
		cv::String w = wCV.c_str();
		class_list.push_back(w);
		UE_LOG(LogTemp, Warning, TEXT("class id %s"), *c);
	}
	
	result = readNetFromDarknet(cfg, weight);
	result.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
	result.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
	auto model = cv::dnn::DetectionModel(result);
	model.setInputParams(1. / 255, cv::Size(608, 608), cv::Scalar(), true);
	Mat frame = imread(MyCvString);
	if (frame.rows >= 600 && frame.cols >= 600) {
		//ResizeImage(frame, frame);
	}

	int frame_count = 0;
	float fps = -1;
	int total_frames = 0;
	if (!frame.empty()) {
		model.detect(frame, classIds, confidences, boxes, .2, .4);
		frame_count++;
		total_frames++;
		int detections = classIds.size();
		for (int i = 0; i < detections; ++i) {

			auto box = boxes[i];
			auto classId = classIds[i];
			const auto color = colors[classId % colors.size()];
			cv::rectangle(frame, box, color, 1);
			cv::rectangle(frame, cv::Point(box.x, box.y - 20), cv::Point(box.x + box.width, box.y), color, cv::FILLED);
			cv::putText(frame, class_list[classId].c_str(), cv::Point(box.x, box.y - 5), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0));
		}
		cv::imwrite("C:/Users/khizra/Documents/UnrealProjects/FloorPlanGeneration/images/mask2.png", frame);
		cv::resize(frame, frame, Size(800, 800));
		cv::imshow("detectedWindow", frame);
		cv::waitKey(0);
		cv::destroyAllWindows();
	}


	vector<cv::Rect> dArr;
	for (int i = 0; i < classIds.size(); i++) {
		if (classIds[i] == 0) {
			FBoxes t;
			t.x = boxes[i].x; t.y = boxes[i].y; t.w = boxes[i].width; t.h = boxes[i].height;
			dArr.push_back(boxes[i]);
			doorArr.Add(t);
		}
	}
	Mat corn, out_norm, out_scaled;
	int ct = 0;
	
	cv::Mat mask(frame.rows, frame.cols, CV_8UC3, cv::Scalar(255,255,255));
	cv::Mat mask2(frame.rows, frame.cols, CV_8UC3, cv::Scalar(255, 255, 255));

	cv::Mat& mt= mask;
	cv::Mat pointImg(mask.rows, mask.cols, CV_8UC3, cv::Scalar(255, 255, 255));

	//for (cv::Rect b : dArr) {
	//	ct++;
	//	//cv::Rect b(box.x,box.y,box.w,box.h);
	//	Mat roi = frame(b);
	//	roi.copyTo(mask(b));
	//	Mat gray, bw;
	//	cvtColor(roi, gray, COLOR_BGR2GRAY);
	//	cv::cornerHarris(gray, corn, 2, 3, 0.04);
	//	cv::normalize(corn, out_norm, 0, 255, NORM_MINMAX, CV_32FC1, Mat());
	//	convertScaleAbs(out_norm, out_scaled);
	//	int cir = 0;
	//	TArray<Point> cornersArr;
	//	for (int j = 0; j < out_norm.rows; j++) {
	//		for (int i = 0; i < out_norm.cols; i++) {
	//			if (out_norm.at<float>(j, i) > 185) {
	//				cir++;
	//				cv::circle(roi, Point(i, j), 1, Scalar(0, 0, 255), 2, 8, 0);
	//				roi.copyTo(mask(b));

	//				cornersArr.Add(Point(i, j));
	//				//UE_LOG(LogTemp, Warning, TEXT("pixel no where corner is located for box: %d at x: %d y: %d"), ct, i, j);
	//			}
	//		}
	//	}

	//	mask.copyTo(mt);
	//	FCornerIdentifier f;
	//	f.boxNum = ct;
	//	UE_LOG(LogTemp, Warning, TEXT("corner array size: %d"), cornersArr.Num());
	//	TArray<Point> finalCorners;
	//	for (Point p : cornersArr) {
	//		int func = isPointInRect(b.x, b.y, b.height, b.width, b.x + p.x, b.y + p.y);
	//		isPointt(b.x, b.y, b.height, b.width, b.x + p.x, b.y + p.y, mask);
	//		if (func!=0) {
	//			if (func == 1) {
	//				f.tl++;
	//				UE_LOG(LogTemp, Warning, TEXT("final corner for box: %d is in top left corner"), ct);
	//			}
	//			if (func == 2) {
	//				f.tr++;
	//				UE_LOG(LogTemp, Warning, TEXT("final corner for box: %d is in top right corner"), ct);
	//			}
	//			if (func == 3) {
	//				f.bl++;
	//				UE_LOG(LogTemp, Warning, TEXT("final corner for box: %d is in bottom left corner"), ct);
	//			}
	//			if (func == 4) {
	//				f.br++;
	//				UE_LOG(LogTemp, Warning, TEXT("final corner for box: %d is in bottom right corner"), ct);
	//			}
	//			//isPointt(b.x, b.y, b.height, b.width, b.x + p.x, b.y + p.y, mt);
	//			finalCorners.Add(p);
	//			//UE_LOG(LogTemp, Warning, TEXT("final corner array index for box: %d is x: %d and y: %d"), ct,p.x+b.x,p.y+b.y);
	//			cv::circle(mask, Point(p.x + b.x, p.y + b.y), 1, Scalar(0, 255, 0), 2, 8, 0);
	//		}
	//	}
	//	doorCorners.Add(f);
	//	UE_LOG(LogTemp, Warning, TEXT("final corner array size: %d"), finalCorners.Num());
	//	
	//	for (Point p : finalCorners) {
	//		cv::circle(pointImg, Point(p.x + b.x, p.y + b.y), 1, Scalar(0, 255, 0), 2, 8, 0);
	//		pointImg.at<uchar>(p.x + b.x, p.y + b.y) = (255, 0, 0);
	//	}
	//}
	
	cv::imwrite("C:/Users/khizra/Documents/UnrealProjects/FloorPlanGeneration/images/output.png", mask);
	//cv::resize(mask, mask, Size(600, 600));

	
	for (cv::Rect d : boxes) {
		for (int i = d.x; i < d.x + d.width; i++) {
			for (int k = d.y; k < d.y + d.height; k++) {
				img.at<uchar>(k, i) = 255;
			}
		}
	}
	ResizeImage(img, img);
	Mat binaryImage; // Binary image to store the result
	cv::threshold(img, binaryImage, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);

	cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));
	cv::morphologyEx(binaryImage, binaryImage, MORPH_OPEN, kernel, Point(-1, -1), 2);
	dilate(binaryImage, binaryImage, kernel, Point(-1, -1), 1);


	// Display the resized image
	cv::imshow("Resized Image", binaryImage);
	cv::waitKey(0);
	cv::destroyAllWindows();

	boxData.Empty();
	boxData = PixelToBoxData(binaryImage);
	rowsToEscape = (600 - binaryImage.rows) / 2;
	colsToEscape = (600 - binaryImage.cols) / 2;

	

	/*if (img.rows >= 600 || img.cols >= 600) {
		RemoveTextFromImage(img, outputImage);
	}
	else {
		img.copyTo(outputImage);
	}*/
	
	//int bCount = 0;
	//for (int ii = 0; ii < classIds.size(); ii++) {
	//	cv::Rect b = boxes[ii];
	//	bCount++;
	//	FBoxes t;
	//	t.x = b.x;
	//	t.y = b.y;
	//	t.w = b.width;
	//	t.h = b.height;
	//	boxArr.Add(t);
	//	bool xAxis = false, yAxis = false;
	//	TArray<int> xRange, xRange2, yRange, yRange2;
	//	int x = b.x-2;
	//	int w = b.width+2;
	//	int y = b.y -2;
	//	int h = b.height+2;
	//	int cnt1 = 0, cnt2 = 0, cnt3 = 0, cnt4 = 0;
	//	
	//	for (int i = b.x; i < b.x+b.width; i++) {
	//		
	//		if (outputImage.at<uchar>(b.y, i) == 255 && xRange.Num() == 0) {
	//			xRange.Add(i);
	//			cnt1++;
	//		}
	//		else {
	//			if ((outputImage.at<uchar>(b.y, i) == 255 && i - xRange[xRange.Num() - 1] == 1)) {
	//				//xAxis = true;
	//				xRange.Add(i);
	//				cnt1++;
	//			}
	//		}
	//		
	//		if (outputImage.at<uchar>(y+h, i) == 255 && xRange2.Num() == 0) {
	//			xRange2.Add(i);
	//			cnt2++;
	//		}
	//		else {
	//			if ((outputImage.at<uchar>(y+h, i) == 255 && i - xRange2[xRange2.Num() - 1] == 1)) {
	//				//xAxis = true;
	//				xRange2.Add(i);
	//				cnt2++;
	//			}
	//		}
	//		
	//		
	//	}
	//	for (int i = b.y; i < b.y+b.height; i++) {
	//		/*outputImage.at<uchar>(i, x) = (0, 0, 255);
	//		outputImage.at<uchar>(i, x+w) = (0, 0, 255);*/
	//		if (outputImage.at<uchar>(i,x) == 255 && yRange.Num() == 0) {
	//			yRange.Add(i);
	//			cnt3++;
	//		}
	//		else {
	//			if ((outputImage.at<uchar>( i,x) == 255 && i - yRange[yRange.Num() - 1] == 1)) {
	//				//xAxis = true;
	//				yRange.Add(i);
	//				cnt3++;
	//			}
	//		}
	//		if (outputImage.at<uchar>( i,x+w) == 255 && yRange2.Num() == 0) {
	//			yRange2.Add(i);
	//			cnt4++;
	//		}
	//		else {
	//			if ((outputImage.at<uchar>( i,x+w) == 255 && i - yRange2[yRange2.Num() - 1] == 1)) {
	//				//xAxis = true;
	//				yRange2.Add(i);
	//				cnt4++;
	//			}
	//		}
	//		
	//		
	//	}
	//	for (auto a : xRange) {
	//		UE_LOG(LogTemp, Warning, TEXT("xRange for box :%d %d"), a, bCount);
	//	}
	//	for (auto a : xRange2) {
	//		UE_LOG(LogTemp, Warning, TEXT("xRange2 for box :%d %d"), a, bCount);
	//	}
	//	for (auto a : yRange) {
	//		UE_LOG(LogTemp, Warning, TEXT("yRange for box :%d %d"), a, bCount);
	//	}
	//	for (auto a : yRange2) {
	//		UE_LOG(LogTemp, Warning, TEXT("yRange2 for box :%d %d"), a, bCount);
	//	}
	//	FBoxes tmp;
	//	if (xRange.Num() == 0 || xRange2.Num() == 0) {
	//		xAxis = false;
	//		yAxis = true;
	//	}
	//	if (yRange.Num() == 0 || yRange2.Num() == 0) {
	//		yAxis = false;
	//		xAxis = true;
	//	}
	//	if (xAxis == true && yAxis==false) {
	//		UE_LOG(LogTemp, Warning, TEXT("x axis true"));
	//		if (cnt1 < cnt2 && cnt1!=0) {
	//			if (xRange.Num()!=0) {
	//				tmp.x = xRange[0];
	//				tmp.w = xRange[xRange.Num() - 1] - xRange[0];
	//				if (tmp.w == 0) {
	//					tmp.w = 2;
	//				}
	//			}
	//			
	//			tmp.y = b.y;
	//			tmp.h = b.height;
	//			//take xRange
	//		}
	//		else {
	//			//take xRange2
	//			if (xRange2.Num() != 0 && xRange.Num()) {
	//				if (cnt2 < cnt1 || ((xRange[0] == xRange2[0]) && (xRange[xRange.Num() - 1] == xRange2[xRange2.Num() - 1]))) {
	//					tmp.x = xRange2[0];
	//					tmp.w = xRange2[xRange2.Num() - 1] - xRange2[0];
	//					if (tmp.w == 0) {
	//						tmp.w = 2;
	//					}
	//					tmp.y = b.y;
	//					tmp.h = b.height;
	//				}
	//				
	//			}
	//			if (xRange.Num() == 0 && xRange2.Num() != 0) {
	//				tmp.x = xRange2[0];
	//				tmp.w = xRange2[xRange2.Num() - 1] - xRange2[0];
	//				if (tmp.w == 0) {
	//					tmp.w = 2;
	//				}
	//				tmp.y = b.y;
	//				tmp.h = b.height;
	//			}
	//			if (xRange2.Num() == 0 && xRange.Num() != 0) {
	//				tmp.x = xRange[0];
	//				tmp.w = xRange[xRange.Num() - 1] - xRange[0];
	//				if (tmp.w == 0) {
	//					tmp.w = 2;
	//				}
	//				tmp.y = b.y;
	//				tmp.h = b.height;
	//			}
	//			
	//		}
	//		
	//	}
	//	else if (yAxis == true && xAxis==false) {
	//		UE_LOG(LogTemp, Warning, TEXT("y axis true"));
	//		if (cnt3 < cnt4 && cnt3!=0) {
	//			//take yRange
	//			if (yRange.Num() != 0) {
	//				tmp.y = yRange[0];
	//				tmp.h = yRange[yRange.Num() - 1] - yRange[0];
	//				if (tmp.h == 0) {
	//					tmp.h = 2;
	//				}
	//			}
	//			tmp.x = b.x;
	//			tmp.w = b.width;
	//		}
	//		else {
	//			//take yRange2
	//			if (yRange2.Num() != 0 && yRange.Num()!=0) {
	//				if (cnt4 < cnt3 || ((yRange[0] == yRange2[0]) && (yRange[yRange.Num() - 1] == yRange2[yRange2.Num() - 1]))) {
	//					tmp.y = yRange2[0];
	//					tmp.h = yRange2[yRange2.Num() - 1] - yRange2[0];
	//					if (tmp.h == 0) {
	//						tmp.h = 2;
	//					}
	//					tmp.x = b.x;
	//					tmp.w = b.width;
	//				}
	//				
	//			}
	//			if (yRange2.Num() == 0 && yRange.Num() != 0) {
	//				tmp.y = yRange[0];
	//				tmp.h = yRange[yRange.Num() - 1] - yRange[0];
	//				if (tmp.h == 0) {
	//					tmp.h = 2;
	//				}
	//				tmp.x = b.x;
	//				tmp.w = b.width;
	//			}
	//			if (yRange.Num() == 0 && yRange2.Num() != 0) {
	//				tmp.y = yRange2[0];
	//				tmp.h = yRange2[yRange2.Num() - 1] - yRange2[0];
	//				if (tmp.h == 0) {
	//					tmp.h = 2;
	//				}
	//				tmp.x = b.x;
	//				tmp.w = b.width;
	//			}
	//			
	//		}
	//		
	//	}
	//	if (classIds[ii] == 1 || classIds[ii] == 3) {
	//		winArr.Add(tmp);
	//	}
	//	if (classIds[ii] == 0 || classIds[ii] == 2) {
	//		doorArr.Add(tmp);
	//	}
	//	boxEnhancedArr.Add(tmp);
	//}




	//if (img.rows <= 600 && img.cols <= 600) {
	//	Mat binaryImage; // Binary image to store the result
	//	cv::threshold(img, binaryImage, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);

	//	cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));
	//	cv::morphologyEx(binaryImage, binaryImage, MORPH_OPEN, kernel, Point(-1, -1), 2);
	//	dilate(binaryImage, binaryImage, kernel, Point(-1, -1), 1);

	//	
	//	// Display the resized image
	//	cv::imshow("Resized Image", binaryImage);
	//	cv::waitKey(0);
	//	cv::destroyAllWindows();

	//	boxData.Empty();
	//	boxData = PixelToBoxData(binaryImage);
	//	rowsToEscape = (600 - binaryImage.rows) / 2;
	//	colsToEscape = (600 - binaryImage.cols) / 2;
	//	
	//}
	//else {


	//	ResizeImage(img, beforePimg);
	//	cv::imshow("img", beforePimg);
	//	cv::waitKey(0);
	//	cv::destroyAllWindows();
	//	RemoveTextFromImage(img, outputImage);
	//	cv::imshow("remtxtimg", outputImage);
	//	cv::waitKey(0);
	//	cv::destroyAllWindows();
	//	UE_LOG(LogTemp, Warning, TEXT("image rows: %d"), outputImage.rows);
	//	UE_LOG(LogTemp, Warning, TEXT("image cols: %d"), outputImage.cols);
	//	
	//	boxData.Empty();
	//	boxData = PixelToBoxData(outputImage);
	//	rowsToEscape = (600 - outputImage.rows) / 2;
	//	colsToEscape = (600 - outputImage.cols) / 2;


	//	
	//	
	//}
	//
	
	
}

Mat& AMyCharacter::ResizeImage(Mat& img, cv::Mat& output)
{
	if (img.empty()) {
		std::cout << "Error: Unable to read the input image!" << std::endl;
		return img;
	}

	// Get the size of the large image
	int largeWidth = img.cols;
	int largeHeight = img.rows;

	// Calculate the aspect ratio of the large image
	double largeAspectRatio = static_cast<double>(largeWidth) / largeHeight;

	// Define the size of the smaller output image (e.g., 600x600)
	int outputSize = 600;

	// Calculate the new width and height of the output image based on the aspect ratio
	int newWidth, newHeight;
	if (largeAspectRatio > 1) {
		newWidth = outputSize;
		newHeight = static_cast<int>(outputSize / largeAspectRatio);
	}
	else {
		newWidth = static_cast<int>(outputSize * largeAspectRatio);
		newHeight = outputSize;
	}

	// Resize the large image to the new size without stretching
	cv::Mat resizedImage;
	cv::resize(img, output, cv::Size(newWidth, newHeight));

	

	return output;
}

TArray<FBlackPixelRange> AMyCharacter::PixelToBoxData(cv::Mat& img)
{
	rows.Empty();
	cols.Empty();
	for (int32 Row = 0; Row < img.rows; ++Row)
	{
		for (int32 Col = 0; Col < img.cols; ++Col)
		{
			if (img.at<uchar>(Row, Col) == 255) {
				rows.Add(Row);
				cols.Add(Col);
			}

		}
	}

	TArray<FBlackPixelRange> blackPixelRanges;

	for (int32 row = 0; row < img.rows; ++row) {
		const uchar* rowData = img.ptr<uchar>(row);
		bool inRange = false;
		int32 rangeStart = -1;

		for (int32 col = 0; col < img.cols; ++col) {
			if (rowData[col] == 255) { 
				if (!inRange) {
					inRange = true;
					rangeStart = col;
				}
			}
			else {
				if (inRange) {
					inRange = false;
					FBlackPixelRange NewRange;
					NewRange.Start = rangeStart;
					NewRange.End = col - 1;
					NewRange.Rows.Add(row);
					blackPixelRanges.Add(NewRange);
				}
			}
		}

		if (inRange) {
			FBlackPixelRange NewRange;
			NewRange.Start = rangeStart;
			NewRange.End = img.cols - 1;
			NewRange.Rows.Add(row);
			blackPixelRanges.Add(NewRange);
		}
	}


	TArray<FBlackPixelRange> CombinedRanges;
	// Iterate through the ranges and combine rows with the same range
	for (const FBlackPixelRange& range : blackPixelRanges) {
		bool Found = false;

		for (FBlackPixelRange& combinedRange : CombinedRanges) {
			if (range.Start == combinedRange.Start && range.End == combinedRange.End) {
				combinedRange.Rows.Append(range.Rows);
				
				Found = true;
				break;
			}
		}

		if (!Found) {
			CombinedRanges.Add(range);
		}
	}
	TArray<FBlackPixelRange> CombinedSplittedRanges;
	//iterating through the rows and spliting it if rows are not consecutive
	for (const FBlackPixelRange& combinedRange : CombinedRanges) {
		bool consecutive=true;
		
		TArray<int32> split;
		split.Empty();
		for (int32 in = 1; in < combinedRange.Rows.Num(); in++) {
			if (combinedRange.Rows[in] - combinedRange.Rows[in - 1] == 1) {
				//consecutive = true;
			}
			else {
				split.Add(in);
				consecutive = false;
			}
		}
		for (int32 a : split) {
			//UE_LOG(LogTemp, Warning, TEXT("split at, with value %d %d"), a, combinedRange.Start);

		}
		if (consecutive) {
			CombinedSplittedRanges.Add(combinedRange);
			//UE_LOG(LogTemp, Warning, TEXT("consecutive value in if %d "), consecutive);
		}
		else {
			if (split.Num() > 0) {
				//UE_LOG(LogTemp, Warning, TEXT("consecutive value in else %d "), consecutive);
				int32 s = 0;
				FBlackPixelRange temp;
				for (int32 i : split) {
					temp.Start = combinedRange.Start;
					temp.End = combinedRange.End;
					temp.Rows.Empty();
					temp.Rows.Add(combinedRange.Rows[s]);
					temp.Rows.Add(combinedRange.Rows[i - 1]);
					s = i;
					CombinedSplittedRanges.Add(temp);
				}
				temp.Start = combinedRange.Start;
				temp.End = combinedRange.End;
				temp.Rows.Empty();
				temp.Rows.Add(combinedRange.Rows[s]);
				temp.Rows.Add(combinedRange.Rows[combinedRange.Rows.Num()-1]);
				CombinedSplittedRanges.Add(temp);
			}
			
			
		}
	}
	for (const FBlackPixelRange& combinedRange : CombinedSplittedRanges) {
		FString RangeString = FString::Printf(TEXT("Range %d-%d, Rows: "), combinedRange.Start, combinedRange.End);
		for (int32 row : combinedRange.Rows) {
			RangeString += FString::Printf(TEXT("%d "), row);
		}
		//UE_LOG(LogTemp, Warning, TEXT("%s"), *RangeString);
	}

	for (const FBlackPixelRange& combinedRange : CombinedRanges) {
		FString RangeString = FString::Printf(TEXT("Range %d-%d, Rows: "), combinedRange.Start, combinedRange.End);
		for (int32 row : combinedRange.Rows) {
			RangeString += FString::Printf(TEXT("%d "), row);
		}
		//UE_LOG(LogTemp, Warning, TEXT("%s"), *RangeString);
	}

	return CombinedSplittedRanges;
}

int AMyCharacter::isPointInRect(int x, int y, int h, int w, int p1, int p2)
{
	int hOffset = 20;
	int wOffset = 20;
	if (h - w >= 10) {
		wOffset = 45;
	}
	if (w - h >=10) {
		hOffset = 45;
	}
	
	if (p1 > x && p1<(x + hOffset) && p2>y && p2 < (y + wOffset)) {
		return 1;
	}
	else if (p1 > ((x + w) - hOffset) && p1<(x + w) && p2>y && p2 < (y + wOffset)) {
		return 2;
	}
	else if (p1 > (x ) && p1< (x+hOffset) && p2>((y + h)- wOffset) && p2 < (y + h)) {
		return 3;
	}
	else if (p1 > ((x + w) - hOffset) && p1<(x + w) && p2>((y + h) - wOffset) && p2 < (y + h)) {
		return 4;
	}
	return 0;
}

int32 AMyCharacter:: isPointt(int32 x, int32 y, int32 h, int32 w, int32 p1, int32 p2, cv::Mat& tmp)
{
	int hOffset = 10;
	int wOffset = 10;
	if (h - w>=10) {
		wOffset = 45;
	}
	if (w - h>=10) {
		hOffset = 45;
	}
	cv::rectangle(tmp, cv::Rect(x, y, hOffset, wOffset), (0, 255, 0), 1, LINE_AA);
	cv::rectangle(tmp, cv::Rect(((x + w) - hOffset), y, hOffset, wOffset), (0, 255, 0), 1, LINE_AA);
	cv::rectangle(tmp, cv::Rect((x), (y + h- wOffset), hOffset, wOffset), (0, 255, 0), 1, LINE_AA);
	cv::rectangle(tmp, cv::Rect(((x + w) - hOffset), ((y + h) - wOffset), hOffset, wOffset), (0, 255, 0), 1, LINE_AA);
	//cv::rectangle(tmp, cv::Rect(x,y,w,h), (255, 0, 0), 1, LINE_AA);
	


	return 0;
}











