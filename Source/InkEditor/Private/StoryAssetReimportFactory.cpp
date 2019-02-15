// Copyright 2019 Brook Jensen. All Rights Reserved.

#include "StoryAssetReimportFactory.h"
#include "StoryAsset.h"
#include "EditorFramework/AssetImportData.h"
#include "Misc/FileHelper.h"

UStoryAssetReimportFactory::UStoryAssetReimportFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bEditorImport = false;
	bEditAfterNew = false;
}

bool UStoryAssetReimportFactory::CanReimport(UObject * Obj, TArray<FString>& OutFilenames)
{
	// Can reimport story assets
	UStoryAsset* StoryAsset = Cast<UStoryAsset>(Obj);
	if (StoryAsset != nullptr && StoryAsset->AssetImportData)
	{
		StoryAsset->AssetImportData->ExtractFilenames(OutFilenames);
		return true;
	}

	return false;
}

void UStoryAssetReimportFactory::SetReimportPaths(UObject * Obj, const TArray<FString>& NewReimportPaths)
{
	UStoryAsset* StoryAsset = Cast<UStoryAsset>(Obj);
	if (StoryAsset && ensure(NewReimportPaths.Num() == 1))
	{
		StoryAsset->AssetImportData->UpdateFilenameOnly(NewReimportPaths[0]);
	}
}

const UObject * UStoryAssetReimportFactory::GetFactoryObject() const
{
	return this;
}

EReimportResult::Type UStoryAssetReimportFactory::Reimport(UObject * Obj)
{
	UStoryAsset* StoryAsset = Cast<UStoryAsset>(Obj);
	if (!StoryAsset)
		return EReimportResult::Failed;

	const FString Filename = StoryAsset->AssetImportData->GetFirstFilename();
	if (!Filename.Len() || IFileManager::Get().FileSize(*Filename) == INDEX_NONE)
	{
		return EReimportResult::Failed;
	}

	// Run the import again
	EReimportResult::Type Result = EReimportResult::Failed;
	bool OutCanceled = false;

	if (ImportObject(StoryAsset->GetClass(), StoryAsset->GetOuter(), *StoryAsset->GetName(), RF_Public | RF_Standalone, Filename, nullptr, OutCanceled) != nullptr)
	{
		StoryAsset->AssetImportData->Update(Filename);

		// Try to find the outer package so we can dirty it up
		if (StoryAsset->GetOuter())
		{
			StoryAsset->GetOuter()->MarkPackageDirty();
		}
		else
		{
			StoryAsset->MarkPackageDirty();
		}
		Result = EReimportResult::Succeeded;
	}
	else
	{
		Result = EReimportResult::Failed;
	}

	return Result;
}
