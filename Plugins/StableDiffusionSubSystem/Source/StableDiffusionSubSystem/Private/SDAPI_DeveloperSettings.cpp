// Copyright by CodeParrots 2023. All Rights Reserved.

#include "SDAPI_DeveloperSettings.h"
#include "SDAPI_Statics.h"


void FSDAPI_Controlnet_Parameters::AddQueryField(TSharedPtr<FJsonObject> JsonObject)
{
	TSharedPtr<FJsonObject> alwayson_scriptsValue;
	if (JsonObject->HasField("alwayson_scripts"))
	{
		alwayson_scriptsValue = JsonObject->GetObjectField("alwayson_scripts");
	}
	else
	{
		alwayson_scriptsValue = MakeShareable(new FJsonObject);
		JsonObject->SetObjectField("alwayson_scripts", alwayson_scriptsValue);
	}


	TArray<TSharedPtr<FJsonValue>> args;
	TSharedPtr<FJsonObject> controlnetValue = MakeShareable(new FJsonObject);

	TSharedPtr<FJsonObject> argsValue = MakeShareable(new FJsonObject);
	if (input_image_Texture != nullptr)
	{
		FString BasedImage = USDAPI_Statics::MakeTexture2Base64EncodeData(input_image_Texture);
		argsValue->SetStringField("input_image", BasedImage);
		
	}
	else if (input_image_RenderTarget)
	{
		FString BasedImage = USDAPI_Statics::MakeRenderTarget2Base64EncodeData(input_image_RenderTarget);
		argsValue->SetStringField("input_image", BasedImage);
		
	}

	FString module = USDAPI_DeveloperSettings::Get()->GetControlnetPreprocessorName(Module);
	argsValue->SetStringField("module", module);


	argsValue->SetStringField("model", model);
	argsValue->SetNumberField("weight", weight);
	argsValue->SetNumberField("resize_mode", (int32)resize_mode);
	argsValue->SetBoolField("lowvram", lowvram);
	argsValue->SetNumberField("guidance_start", guidance_start);
	argsValue->SetNumberField("guidance_end", guidance_end);

	// argsValue를 FJsonValue로 변환하고 args 배열에 추가
	TSharedPtr<FJsonValue> JsonValue = MakeShareable(new FJsonValueObject(argsValue));
	args.Add(JsonValue);

	// args 배열을 controlnetValue JsonObject에 설정
	controlnetValue->SetArrayField("args", args);

	alwayson_scriptsValue->SetObjectField("controlnet", controlnetValue);
}

void FSDAPI_Parameters::AddQueryField(TSharedPtr<FJsonObject> JsonObject)
{
	JsonObject->SetNumberField("CLIP_stop_at_last_layers", CLIP_stop_at_last_layers);
	JsonObject->SetNumberField("steps", Steps);
	JsonObject->SetNumberField("denoising_strength", Img2Img_Denoising_Strength);
	JsonObject->SetNumberField("cfg_scale", Cfg_Scale);
	JsonObject->SetNumberField("height", height);
	JsonObject->SetNumberField("width", width);
	JsonObject->SetStringField("sampler_name", sampler_name);
	TArray<TSharedPtr<FJsonValue>> ImageArray;
	for (FString StyleString : Styles)
	{
		TSharedPtr<FJsonValueString> EncodedStringJson = MakeShared<FJsonValueString>(StyleString);
		ImageArray.Add(EncodedStringJson);
	}
	JsonObject->SetArrayField("Styles", ImageArray);
}
