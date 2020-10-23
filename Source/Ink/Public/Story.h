// Copyright 2018 David Colson. All Rights Reserved.

#pragma once

#include "Delegates/DelegateCombinations.h"
#include "MonoBaseClass.h"
#include "InkVar.h"

#include "Story.generated.h"

class UStoryAsset;
class UChoice;
class UStoryState;

DECLARE_DYNAMIC_DELEGATE_TwoParams(FVariableObserver, FString, variableName, FInkVar, newValue);
DECLARE_DYNAMIC_DELEGATE_RetVal_TwoParams(FInkVar, FExternalFunctionHandler, FString, functionName, TArray<FInkVar>, arguments);

#if PLATFORM_LINUX
extern "C" __attribute__((visibility("default"))) void ObserverCallbackInt(int instanceId, const char* variableName, int newValue);
extern "C" __attribute__((visibility("default"))) void ObserverCallbackFloat(int instanceId, const char* variableName, float newValue);
extern "C" __attribute__((visibility("default"))) void ObserverCallbackString(int instanceId, const char* variableName, const char* newValue);
extern "C" __attribute__((visibility("default"))) FInkVarInterop ExternalFunctionCallback(int32 instanceId, const char* functionName, uint32 numArgs, FInkVarInterop* pArgs);
#else
extern "C" __declspec(dllexport) void ObserverCallbackInt(int instanceId, const char* variableName, int newValue);
extern "C" __declspec(dllexport) void ObserverCallbackFloat(int instanceId, const char* variableName, float newValue);
extern "C" __declspec(dllexport) void ObserverCallbackString(int instanceId, const char* variableName, const char* newValue);
extern "C" __declspec(dllexport) FInkVarInterop ExternalFunctionCallback(int32 instanceId, const char* functionName, uint32 numArgs, FInkVarInterop* pArgs);
#endif //PLATFORM_LINUX

UCLASS(BlueprintType)
class INK_API UStory : public UMonoBaseClass
{
	GENERATED_BODY()

public:
	UStory();
	~UStory();

	UFUNCTION(BlueprintCallable, Category = Ink)
	static UStory* NewStory(UStoryAsset* StoryAsset);

	UFUNCTION(BlueprintCallable, Category = Ink)
	FString Continue();

	UFUNCTION(BlueprintCallable, Category = Ink)
	bool CanContinue();

	UFUNCTION(BlueprintCallable, Category = Ink)
	TArray<UChoice*> CurrentChoices();

	UFUNCTION(BlueprintCallable, Category = Ink)
	void ChooseChoiceIndex(int index);

	UFUNCTION(BlueprintCallable, Category = Ink)
	FString CurrentText();

	UFUNCTION(BlueprintCallable, Category = Ink)
	UStoryState* State();

	UFUNCTION(BlueprintCallable, Category = Ink)
	bool HasError();

	UFUNCTION(BlueprintCallable, Category = Ink)
	TArray<FString> CurrentTags();

	UFUNCTION(BlueprintCallable, Category = Ink)
	TArray<FString> CurrentErrors();

	UFUNCTION(BlueprintCallable, Category = Ink)
	void ResetState();

	UFUNCTION(BlueprintCallable, Category = Ink)
	void ResetErrors();

	UFUNCTION(BlueprintCallable, Category = Ink)
	void ResetCallstack();

	UFUNCTION(BlueprintCallable, Category = Ink)
	void ContinueAsync(float millisecondLimitAsync);

	UFUNCTION(BlueprintCallable, Category = Ink)
	FString ContinueMaximally();

	UFUNCTION(BlueprintCallable, Category = Ink)
	bool AsyncContinueComplete();

	UFUNCTION(BlueprintCallable, Category = Ink)
	TArray<FString> TagsForContentAtPath(FString Path);

	UFUNCTION(BlueprintCallable, Category = Ink)
	void ChoosePathString(FString Path, bool ResetCallstack, TArray<FInkVar> vars);

	UFUNCTION(BlueprintCallable, Category = Ink)
	void ObserveVariable(FString variableName, const FVariableObserver& observer);

	UFUNCTION(BlueprintCallable, Category = Ink)
	void ObserveVariables(TArray<FString> variableNames, const FVariableObserver& observer);

	UFUNCTION(BlueprintCallable, Category = Ink)
	void RemoveVariableObserver(const FVariableObserver& observer, FString specificVariableName = "");

	UFUNCTION(BlueprintPure, Category = Ink)
	class UVariablesState* VariablesState();

	UFUNCTION(BlueprintCallable, Category = Ink)
	void SetVariableState(const TMap<FString, FInkVar>& state);

	UFUNCTION(BlueprintCallable, Category = Ink)
	void RegisterExternalFunction(FString functionName, const FExternalFunctionHandler& function);

private:
	typedef TPair<int, FString> FDelegateMapKey;
	static TMap<FDelegateMapKey, TArray<FVariableObserver>> delegateMap;
	static TMap<FDelegateMapKey, FExternalFunctionHandler> funcMap;
	static int instanceCounter;
	int instanceId{ -1 };

#if PLATFORM_LINUX
	friend __attribute__((visibility("default"))) void ObserverCallbackInt(int instanceId, const char* variableName, int newValue);
	friend __attribute__((visibility("default"))) void ObserverCallbackFloat(int instanceId, const char* variableName, float newValue);
	friend __attribute__((visibility("default"))) void ObserverCallbackString(int instanceId, const char* variableName, const char* newValue);
	friend __attribute__((visibility("default"))) FInkVarInterop ExternalFunctionCallback(int32 instanceId, const char* functionName, uint32 numArgs, FInkVarInterop* pArgs);
#else
	friend __declspec(dllexport) void ObserverCallbackInt(int instanceId, const char* variableName, int newValue);
	friend __declspec(dllexport) void ObserverCallbackFloat(int instanceId, const char* variableName, float newValue);
	friend __declspec(dllexport) void ObserverCallbackString(int instanceId, const char* variableName, const char* newValue);
	friend __declspec(dllexport) FInkVarInterop ExternalFunctionCallback(int32 instanceId, const char* functionName, uint32 numArgs, FInkVarInterop* pArgs);
#endif // PLATFORM_LINUX
};