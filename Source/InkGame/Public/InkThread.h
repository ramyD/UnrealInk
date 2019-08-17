// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "InkVar.h"
#include "InkThread.generated.h"

class UStory;
class UChoice;
class UTagList;
class AInkRuntime;

DECLARE_DYNAMIC_DELEGATE_ThreeParams(FTagFunctionDelegate, FString, FirstParameter, FString, SecondParameter, FString, ThirdParameter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FTagFunctionMulticastDelegate, FString, FirstParameter, FString, SecondParameter, FString, ThirdParameter);

DECLARE_DYNAMIC_DELEGATE_TwoParams(FExternalFunctionDelegate, const TArray<FInkVar>&, Arguments, FInkVar&, Result);

/**
 * Base class for all ink threads
 */
UCLASS(Blueprintable)
class INKGAME_API UInkThread : public UObject
{
	GENERATED_BODY()

public:
	UInkThread();

	// Yields the thread immediately. Will wait until Resume().
	UFUNCTION(BlueprintCallable)
	void Yield();

	UFUNCTION(BlueprintPure)
	bool IsYielding();

	// Causes the thread to resume if yielded.
	UFUNCTION(BlueprintCallable)
	void Resume();

	// Kills the thread, regardless of state
	UFUNCTION(BlueprintCallable)
	void Stop();

	// Returns the runtime which owns this thread.
	UFUNCTION(BlueprintPure)
	AInkRuntime* GetRuntime() const { return mpRuntime; }

	// Called before the thread begins executing
	UFUNCTION(BlueprintImplementableEvent)
	void OnStartup();

	// Called when the thread has printed a new line
	UFUNCTION(BlueprintImplementableEvent)
	void OnLineWritten(const FString& line, UTagList* tags);

	// Called when a tag has been processed on the current line
	UFUNCTION(BlueprintImplementableEvent)
	void OnTag(const FString& line);

	// Called when the thread has requested a branch
	UFUNCTION(BlueprintImplementableEvent)
	void OnChoice(const TArray<UChoice*>& choices);

	// Called before the thread is destroyed
	UFUNCTION(BlueprintImplementableEvent)
	void OnShutdown();

	// Picks a choice by index at the current branch
	UFUNCTION(BlueprintCallable)
	void PickChoice(int index);

	// Registers a callback for a named "tag function"
	UFUNCTION(BlueprintCallable)
	void RegisterTagFunction(const FString& functionName, const FTagFunctionDelegate& function);

	UFUNCTION(BlueprintCallable)
	void RegisterExternalFunction(const FString& functionName, const FExternalFunctionDelegate& function);
	
private:
	friend class AInkRuntime;

	void Initialize(FString path, AInkRuntime* runtime);
	void PushState(UStory* pStory);
	void PopState(UStory* pStory);
	bool Execute(UStory* pStory);
	bool CanExecute() const;

	bool ExecuteInternal(UStory* pStory);

	bool HandleExternalFunction(const FString& functionName, TArray<FInkVar> arguments, FInkVar& result);

private:
	FString mState;
	bool mbIsStatePushed;

	FString mStartPath;
	bool mbHasRun;

	int mnChoiceToChoose;
	int mnYieldCounter;
	bool mbInChoice;
	bool mbKill;
	bool mbInitialized;

	UPROPERTY()
	AInkRuntime* mpRuntime;

	UPROPERTY()
	TMap<FString, FTagFunctionMulticastDelegate> mTagFunctions;

	UPROPERTY()
	TMap<FString, FExternalFunctionDelegate> mExternalFunctions;
};
