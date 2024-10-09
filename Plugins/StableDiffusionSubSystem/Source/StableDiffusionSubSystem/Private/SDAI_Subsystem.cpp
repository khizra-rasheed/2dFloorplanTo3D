// Copyright by CodeParrots 2023. All Rights Reserved.

#include "SDAI_Subsystem.h"




#if WITH_EDITOR

#include "DesktopPlatformModule.h"
#include "Framework/Application/SlateApplication.h"

#endif

#include "SDAPI_DeveloperSettings.h"

void USDAI_Subsystem::InitializedDefaultSettingsToAPI()
{
}


FSDAPI_Response_LoraData USDAI_Subsystem::FindLoraFromAvailableLorasByName(FString ContainWord)
{
	FSDAPI_Response_LoraData* ModelNamePtr = AvailableLoraList.FindByPredicate([ContainWord](FSDAPI_Response_LoraData& Element) { return Element.name.Contains(ContainWord); });
	if (ModelNamePtr != nullptr)
	{
		return *ModelNamePtr;
	}
	else
		return FSDAPI_Response_LoraData();
}

void USDAI_Subsystem::SetLoraInfoData(FString LoraName, FSDAPI_Response_LoraInfoData LoraInfo)
{
	FSDAPI_Response_LoraData* ModelNamePtr = AvailableLoraList.FindByPredicate([LoraName](FSDAPI_Response_LoraData& Element) { return Element.name.Contains(LoraName); });
	if (ModelNamePtr != nullptr)
	{
		ModelNamePtr->LoraInfo = LoraInfo;
	}
}

FString USDAI_Subsystem::GetLoraUsageFormat(FString Name)
{
	return  IsValid(USDAPI_DeveloperSettings::Get()) ? USDAPI_DeveloperSettings::Get()->GetLoraFormat(Name, USDAPI_DeveloperSettings::Get()->LoraDefaultUsage) : FString();
}

FString USDAI_Subsystem::GetLoraInfoFromName(FString Name)
{


	return FString();
}

bool USDAI_Subsystem::CanUseLCMLora()
{
	 if (IsValid(USDAPI_DeveloperSettings::Get()))
		 return FindLoraFromAvailableLorasByName(USDAPI_DeveloperSettings::Get()->LCMLoraName) != FSDAPI_Response_LoraData(); 
	 else
		 return false; 
}

void USDAI_Subsystem::SetLCMDefaultParam(FSDAPI_Parameters_LCM InLCMDefaultParam)
{
	if (IsValid(USDAPI_DeveloperSettings::Get())) USDAPI_DeveloperSettings::Get()->DefaultLCMParams = InLCMDefaultParam; 
}

FSDAPI_Parameters_LCM USDAI_Subsystem::GetLCMDefaultParam()
{
	return IsValid(USDAPI_DeveloperSettings::Get()) ? USDAPI_DeveloperSettings::Get()->DefaultLCMParams : FSDAPI_Parameters_LCM();
}

FString USDAI_Subsystem::GetLoraFormatForLCM()
{
	return GetLoraUsageFormat(IsValid(USDAPI_DeveloperSettings::Get()) ? USDAPI_DeveloperSettings::Get()->LCMLoraName : FString()); 
}

FString USDAI_Subsystem::FindCheckPointModelFromAvailableModels(FString ContainWord)
{
	FSDAPI_Response_ModelData* ModelNamePtr = AvailableModelList.FindByPredicate([ContainWord](FSDAPI_Response_ModelData& Element) { return Element.model_name.Contains(ContainWord); });
	if (ModelNamePtr != nullptr)
	{
		return ModelNamePtr->model_name;
	}
	else
		return FString();
}

TArray<FString> USDAI_Subsystem::GetURLKeys()
{
	TArray<FString> Result;

	USDAPI_DeveloperSettings* DevSettings = USDAPI_DeveloperSettings::Get();

	for (auto URLItem : DevSettings->API_URLMap)
	{
		Result.AddUnique(URLItem.Key);
	}
	return Result;
}

FString USDAI_Subsystem::GetURLByKey(FString Key)
{
	USDAPI_DeveloperSettings* DevSettings = USDAPI_DeveloperSettings::Get();

	if (IsValid(DevSettings) && DevSettings->API_URLMap.Contains(Key))
	{
		return DevSettings->API_URLMap[Key];
	}

	return FString();
}

TArray<FString> USDAI_Subsystem::GetAvailableSamplers()
{
    if (IsValid(USDAPI_DeveloperSettings::Get()))
    {
        TArray<FString> Samplers = USDAPI_DeveloperSettings::Get()->AvailableSamplers;

        return Samplers;
    }
    
    return TArray<FString>();
}

FString USDAI_Subsystem::FindSamplerFromAvailableSamplers(FString ContainWord)
{
	if (USDAPI_DeveloperSettings::Get() != nullptr)
	{

		FString* ModelNamePtr = USDAPI_DeveloperSettings::Get()->AvailableSamplers.FindByPredicate([ContainWord](FString& Element) { return Element.Contains(ContainWord); });


		if (ModelNamePtr != nullptr)
		{
			return *ModelNamePtr;
		}
		else
			return FString();
	}
	return
		FString();
}

USDAPI_DeveloperSettings* USDAI_Subsystem::GetSubsystemDeveloperSettings()
{
	return USDAPI_DeveloperSettings::Get();
}

FString USDAI_Subsystem::GetStatupCommandLineArg()
{
	return IsValid(USDAPI_DeveloperSettings::Get()) ? USDAPI_DeveloperSettings::Get()->GetStatupCommandLineArg() : FString();
}

FFilePath USDAI_Subsystem::GetLocalAIPath()
{
	return IsValid(USDAPI_DeveloperSettings::Get()) ? USDAPI_DeveloperSettings::Get()->GetLocalAIPath() : FFilePath();
}

void USDAI_Subsystem::SetLocalAIPath()
{
#if WITH_EDITOR
#if PLATFORM_WINDOWS
	const void* ParentWindowPtr = FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr);

	TArray<FString> OutFiles;
	FString FileTypes = TEXT("Text files (*.txt)|*.txt|All files (*.*)|*.*");

	// Open the file picker
	if (FDesktopPlatformModule::Get()->OpenFileDialog(
		ParentWindowPtr,
		TEXT("Choose a file"),
		TEXT(""),
		TEXT(""),
		FileTypes,
		EFileDialogFlags::None,
		OutFiles
	))
	{
		if (OutFiles.Num() > 0 && IsValid(USDAPI_DeveloperSettings::Get()))
		{
			USDAPI_DeveloperSettings::Get()->SetLocalAIPath(OutFiles[0]);
		}
	}
#endif
#endif
}

void USDAI_Subsystem::StartLocalAIProgram()
{
	FFilePath Path = GetLocalAIPath();
	FString CommandArg = GetStatupCommandLineArg();
	Path.FilePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()) + "stable-diffusion-webui-master/webui.bat";
	if (Path.FilePath.IsEmpty() == false)
	{
		void* ReadPipe = nullptr;
		void* WritePipe = nullptr;
		FString Location = FPaths::GetPath(Path.FilePath);

		UE_LOG(LogTemp, Warning, TEXT("PATH %s"), *Location);

		FPlatformProcess::CreatePipe(ReadPipe, WritePipe);
		FPlatformProcess::CreateProc(*Path.FilePath, *CommandArg, true, false, false, nullptr, 0, *Location, WritePipe);
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("EMPTYYY PATH"));

#if WITH_EDITOR

	
#endif
}

void USDAI_Subsystem::StopLocalAIProgram() {
	//FPlatformProcess::ClosePipe(ReadPipe, WritePipe);
}

bool USDAI_Subsystem::GetControlNet_LowVRamMode()
{
	if (CanUseControlnet())
	{

		USDAPI_DeveloperSettings* Settings = GetSubsystemDeveloperSettings();

		if (IsValid(Settings))
		{
			return Settings->GetControlNet_LowVRamMode();
		}
	}

	return false;
}

void USDAI_Subsystem::SetControlNet_LowVRamMode(bool bControlNet_LowVRamMode)
{
	USDAPI_DeveloperSettings* Settings = GetSubsystemDeveloperSettings();

	if (IsValid(Settings))
	{
		Settings->SetControlNet_LowVRamMode(bControlNet_LowVRamMode);
	}
}

FString USDAI_Subsystem::GetDefaultURLKey()
{
	USDAPI_DeveloperSettings* Settings = GetSubsystemDeveloperSettings();

	if (Settings != nullptr)
	{
		return Settings->GetDefaultURLKey();
	}

	return FString();
}

FString USDAI_Subsystem::FindFirstControlnetModelContainsName(ESDAPI_Controlnet_Module Module)
{
	if (CanUseControlnet())
	{

		USDAPI_DeveloperSettings* Settings = GetSubsystemDeveloperSettings();

		if (Settings != nullptr)
		{
			FString ModelKeyword = Settings->GetControlnetModelKeywordToSearch(Module);

			FString* ModelNamePtr = AvailableControlnetModelList.FindByPredicate([ModelKeyword](FString& Element) { return Element.Contains(ModelKeyword); });

			if (ModelNamePtr != nullptr)
			{
				return *ModelNamePtr;
			}
			else
				return  "None";
		}
	}

	return "None";
}

TArray<FString> USDAI_Subsystem::FindAllControlnetModelContainsName(ESDAPI_Controlnet_Module Module)
{
	TArray<FString> Array;

	if (CanUseControlnet())
	{
		USDAPI_DeveloperSettings* Settings = GetSubsystemDeveloperSettings();

		if (Settings != nullptr)
		{
			FString ModelKeyword = Settings->GetControlnetModelKeywordToSearch(Module);

			FString* ModelNamePtr = AvailableControlnetModelList.FindByPredicate([ModelKeyword](FString& Element) { return Element.Contains(ModelKeyword); });

			if (ModelNamePtr != nullptr)
			{
				Array.Add(*ModelNamePtr);
			}
		}
	}

	return Array;
}

void USDAI_Subsystem::ResetConnectionState()
{
	CurrentState = ESDAPI_SubsystemState::API_NOT_CONNECTED;
}
