// Fill out your copyright notice in the Description page of Project Settings.

#include "InkThread.h"

// Game includes
#include "InkGame.h"
#include "TagList.h"

// UnrealInk includes
#include "Story.h"
#include "StoryState.h"
#include "VariablesState.h"
#include "../Public/InkThread.h"

UInkThread::UInkThread() : mbIsStatePushed(false), mbHasRun(false), mnChoiceToChoose(-1), mbKill(false), mnYieldCounter(0) { }

void UInkThread::Yield()
{
	mnYieldCounter++;
}

bool UInkThread::IsYielding()
{
	return mnYieldCounter > 0;
}

void UInkThread::Resume()
{
	mnYieldCounter--;
}

void UInkThread::RegisterTagFunction(const FString & functionName, const FTagFunctionDelegate & function)
{
	// Register tag function
	mTagFunctions.FindOrAdd(functionName).Add(function);
}

void UInkThread::Initialize(FString path, AInkRuntime* runtime)
{
	mStartPath = path;
	mbIsStatePushed = true;
	mpRuntime = runtime;

	OnStartup();
}

void UInkThread::PushState(UStory * pStory)
{
	if (!ensureMsgf(!mbIsStatePushed, TEXT("Attempting to push ink thread that is already active!")))
		return;

	// Backup variables
	TMap<FString, FInkVar> variableCache;
	UVariablesState* pState = pStory->VariablesState();
	TArray<FString> names = pState->GetVariables();
	for (auto iter = names.CreateConstIterator(); iter; ++iter)
	{
		FInkVar var = pState->GetVariable(*iter);
		variableCache.Add(*iter, var);
	}

	// Restore state from store
	pStory->State()->LoadJson(mState);

	// Restore global variables from cache
	for (auto iter = variableCache.CreateConstIterator(); iter; ++iter)
	{
		pState->SetVariable(iter.Key(), iter.Value());
	}

	// Null out state
	mbIsStatePushed = true;
	mState = nullptr;
}

void UInkThread::PopState(UStory * pStory)
{
	if (!ensureMsgf(mbIsStatePushed, TEXT("Attempting to pop ink thread that is not active!")))
		return;

	// Pull the state out and store it
	mState = pStory->State()->ToJson();
	mbIsStatePushed = false;
}

bool UInkThread::ExecuteInternal(UStory * pStory)
{
	if (!ensureMsgf(mbIsStatePushed, TEXT("Can not execute thread that hasn't been pushed onto the execution stack")))
		return true;

	// Kill thread
	if (mbKill)
		return true;

	// If this is the first time we're running, start us off at our starting path
	if (!mbHasRun)
	{
		if (!ensureMsgf(!mStartPath.IsEmpty(), TEXT("Thread executing without call to ::Initialize")))
			return true;

		pStory->ChoosePathString(mStartPath, true, TArray<FInkVar>());
		mbHasRun = true;
	}

	// Execution loop
	while (true)
	{
		// Handle pending choice
		if (mnChoiceToChoose != -1)
		{
			if (ensure(pStory->CurrentChoices().Num() > 0))
			{
				pStory->ChooseChoiceIndex(mnChoiceToChoose);
			}
			mnChoiceToChoose = -1;
		}

		// Execute until story yields or finishes
		while (mnYieldCounter == 0 && pStory->CanContinue())
		{
			// Handle text
			FString line = pStory->Continue();

			// Handle tags
			TArray<FString> tags = pStory->CurrentTags();
			for (auto it = tags.CreateConstIterator(); it; ++it)
			{
				// Generic tag handler
				OnTag(*it);

				// Tag methods
				TArray<FString> params;
				it->ParseIntoArray(params, TEXT("_"));

				// Look for method and execute with parameters
				FTagFunctionMulticastDelegate* function = mTagFunctions.Find(params[0]);
				if (function != nullptr)
				{
#define GET_PARAM(n) params.Num() > n ? params[n] : FString()
					function->Broadcast(GET_PARAM(1), GET_PARAM(2), GET_PARAM(3));
#undef GET_PARAM
				}
			}

			// Forward to handler
			UTagList* pTags = NewObject<UTagList>(this, UTagList::StaticClass());
			pTags->Initialize(tags);
			OnLineWritten(line, pTags);
		}

		// Handle choice block
		if (mnYieldCounter == 0 && pStory->CurrentChoices().Num() > 0)
		{
			mbInChoice = true;

			// TODO: Record choices somewhere?

			// Forward to handler
			OnChoice(pStory->CurrentChoices());

			// If we've chosen a choice already, go back up to handle it.
			if (mnChoiceToChoose != -1)
				continue;
		}

		break;
	}

	// Have we reached the end? If so, destroy the thread
	if (!pStory->CanContinue() && mnYieldCounter == 0 && pStory->CurrentChoices().Num() == 0)
	{
		UE_LOG(InkRuntime, Display, TEXT("Destroying thread"));

		// TODO: Event for ending?

		// TODO: Destroy ourselves?
		return true;
	}

	// There's more to go. Return false to put us on the waiting list.
	return false;
}

bool UInkThread::Execute(UStory * pStory)
{
	// Execute thread
	bool finished = ExecuteInternal(pStory);

	// If we've finished, run callback
	if (finished)
		OnShutdown();

	// Return result
	return finished;
}

void UInkThread::PickChoice(int index)
{
	mnChoiceToChoose = index;
	mbInChoice = false;
}

bool UInkThread::CanExecute() const
{
	return mnYieldCounter == 0 && !mbInChoice;
}

void UInkThread::Stop()
{
	mbKill = true;
}