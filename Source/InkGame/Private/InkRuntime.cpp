// Fill out your copyright notice in the Description page of Project Settings.

#include "InkRuntime.h"

// Game includes
#include "InkGame.h"
#include "InkThread.h"

// UnrealInk includes
#include "Story.h"

// Sets default values
AInkRuntime::AInkRuntime() : mpRuntime(nullptr)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AInkRuntime::BeginPlay()
{
	Super::BeginPlay();

	// Create the CPU for the story
	if (StoryAsset != nullptr)
	{
		mpRuntime = UStory::NewStory(StoryAsset);
		UE_LOG(InkRuntime, Display, TEXT("Loaded Ink asset"));
	}
	else
	{
		UE_LOG(InkRuntime, Warning, TEXT("No story asset assigned."));
	}
}

// Called every frame
void AInkRuntime::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Early out: no threads.
	if (mThreads.Num() == 0)
		return;

	// Optimization: Try to execute current thread first (avoids context switches)
	if (mpCurrentThread != nullptr && mpCurrentThread->CanExecute() && mpCurrentThread->Execute(mpRuntime))
	{
		// Thread is out of content. Finish.
		mThreads.Remove(mpCurrentThread);
		mpCurrentThread = nullptr;
	}

	UInkThread* pOriginalThread = mpCurrentThread;

	// Execute other available threads
	for (auto iter = mThreads.CreateIterator(); iter; iter++)
	{
		UInkThread* pNextThread = *iter;

		// We've already handled the original thread.
		if (pOriginalThread == pNextThread)
			continue;

		// Ignore threads that aren't eligable for execution
		if (!pNextThread->CanExecute())
			continue;

		// Pop old state
		if(mpCurrentThread != nullptr)
			mpCurrentThread->PopState(mpRuntime);

		// Push new thread's state into the runtime
		mpCurrentThread = pNextThread;
		mpCurrentThread->PushState(mpRuntime);

		// Execute
		if (mpCurrentThread->Execute(mpRuntime))
		{
			// If the thread has finished, destroy it
			iter.RemoveCurrent();
			mpCurrentThread = nullptr;
		}
	}
}

void AInkRuntime::ExternalFunctionRegistered(FString functionName)
{
	// Add to registered function set
	bool alreadyRegistered = false;
	mRegisteredFunctions.Add(functionName, &alreadyRegistered);

	// If we've never head of this function before, register it with the Story runtime
	if (!alreadyRegistered) 
	{
		FExternalFunctionHandler handler;
		handler.BindDynamic(this, &AInkRuntime::ExternalFunctionHandler);
		mpRuntime->RegisterExternalFunction(functionName, handler);
	}
}

FInkVar AInkRuntime::ExternalFunctionHandler(FString functionName, TArray<FInkVar> arguments)
{
	checkf(mpCurrentThread != nullptr, TEXT("Got callback for external function '%s' but no thread is currently active!"), *functionName);

	FInkVar result;
	bool handled = mpCurrentThread->HandleExternalFunction(functionName, arguments, result);
	if (!handled)
	{
		UE_LOG(InkRuntime, Warning, TEXT("Currently running thread has no implementation of external function %s"), *functionName);
		return FInkVar();
	}

	return result;
}

UInkThread* AInkRuntime::Start(TSubclassOf<class UInkThread> type, FString path, bool startImmediately)
{
	if (mpRuntime == nullptr)
	{
		return nullptr;
	}

	// Instantiate the thread
	UInkThread* pThread = NewObject<UInkThread>(this, type);

	// Startup
	return StartExisting(pThread, path, startImmediately);
}

UInkThread* AInkRuntime::StartExisting(UInkThread* thread, FString path, bool startImmediately /*= true*/)
{
	if (mpRuntime == nullptr)
	{
		return nullptr;
	}

	// Initialize thread
	thread->Initialize(path, this);

	// If we're not starting immediately, just queue
	if (!startImmediately)
	{
		mThreads.Add(thread);
		return thread;
	}

	// Otherwise, pop off the current thread
	if (mpCurrentThread != nullptr)
	{
		mpCurrentThread->PopState(mpRuntime);
		mpCurrentThread = nullptr;
	}

	// Execute the newly created thread
	mpCurrentThread = thread;
	if (!thread->Execute(mpRuntime))
	{
		// If it hasn't finished immediately, add it to the threads list
		mThreads.Add(thread);
	}
	else
	{
		// Clear current thread pointer if we ended immediately
		mpCurrentThread = nullptr;
	}

	return thread;
}
