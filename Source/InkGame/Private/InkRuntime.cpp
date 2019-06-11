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

UInkThread* AInkRuntime::Start(TSubclassOf<class UInkThread> type, FString path, bool startImmediately)
{
	// Instantiate the thread
	UInkThread* pThread = NewObject<UInkThread>(this, type);
	pThread->Initialize(path, this);

	// If we're not starting immediately, just queue
	if (!startImmediately)
	{
		mThreads.Add(pThread);
		return pThread;
	}

	// Otherwise, pop off the current thread
	if (mpCurrentThread != nullptr)
		mpCurrentThread->PopState(mpRuntime);

	// Execute the newly created thread
	if (!pThread->Execute(mpRuntime))
	{
		// If it hasn't finished immediately, add it to the threads list
		mThreads.Add(pThread);
		mpCurrentThread = pThread;
	}

	return pThread;
}