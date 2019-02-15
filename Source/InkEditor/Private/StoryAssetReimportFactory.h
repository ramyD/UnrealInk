// Copyright 2019 Brook Jensen. All Rights Reserved.

#pragma once

#include "StoryAssetFactory.h"
#include "EditorReimportHandler.h"
#include "UObject/UObjectGlobals.h"

#include "StoryAssetReimportFactory.generated.h"

UCLASS()
class UStoryAssetReimportFactory : public UStoryAssetFactory, public FReimportHandler
{
	GENERATED_UCLASS_BODY()

public:

	bool CanReimport(UObject* Obj, TArray<FString>& OutFilenames) override;
	const UObject* GetFactoryObject() const override;
	EReimportResult::Type Reimport(UObject* Obj) override;
	void SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths) override;
};