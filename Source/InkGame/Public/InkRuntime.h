// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InkVar.h"
#include "GameFramework/Actor.h"
#include "InkRuntime.generated.h"

class UInkThread;

UCLASS()
class INKGAME_API AInkRuntime : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInkRuntime();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	UInkThread* Start(TSubclassOf<UInkThread> type, FString path, bool runImmediately = true);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Story asset used in this level
	UPROPERTY(EditAnywhere)
	class UStoryAsset* StoryAsset;

	// Called by threads when they want to register an external function
	void ExternalFunctionRegistered(FString functionName);

private:
	UFUNCTION()
	FInkVar ExternalFunctionHandler(FString functionName, TArray<FInkVar> arguments);

private:
	UPROPERTY()
	class UStory* mpRuntime;

	UPROPERTY()
	TArray<UInkThread*> mThreads;

	UPROPERTY()
	UInkThread* mpCurrentThread;

	UPROPERTY()
	TSet<FString> mRegisteredFunctions;
};
