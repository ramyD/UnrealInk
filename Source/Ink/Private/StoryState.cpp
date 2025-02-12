// Copyright 2018 David Colson. All Rights Reserved.

#include "StoryState.h"

#include "Ink.h"
#include "MonoBaseClassImpl.h"

#include <mono/jit/jit.h>


////////////////////////////////////////////////////////
UStoryState::UStoryState()
{

}

////////////////////////////////////////////////////////
UStoryState* UStoryState::NewStoryState(MonoObject* MonoStoryState)
{
	UStoryState* NewStoryState = NewObject<UStoryState>();
	NewStoryState->NewFromInstance(MonoStoryState);
	return NewStoryState;
}

////////////////////////////////////////////////////////
int UStoryState::InkSaveStateVersion()
{
	return MonoInvoke<int>("InkSaveStateVersion", NULL);
}

////////////////////////////////////////////////////////
FString UStoryState::ToJson()
{
	return MonoInvoke<FString>("ToJson", NULL);
}

////////////////////////////////////////////////////////
void UStoryState::LoadJson(FString Json)
{
	MonoString* MonoJson = mono_string_new(mono_domain_get(), TCHAR_TO_ANSI(*Json));
	void* args[1];
	args[0] = MonoJson;

	MonoInvoke<void>("LoadJson", args);
}

////////////////////////////////////////////////////////
void UStoryState::LoadJsonButMaintainGlobalState(FString Json)
{
	MonoString* MonoJson = mono_string_new(mono_domain_get(), TCHAR_TO_ANSI(*Json));
	void* args[1];
	args[0] = MonoJson;

	MonoInvoke<void>("LoadJsonButMaintainGlobalState", args);
}

////////////////////////////////////////////////////////
int UStoryState::VisitCountAtPathString(FString PathString)
{
	MonoString* MonoPath = mono_string_new(mono_domain_get(), TCHAR_TO_ANSI(*PathString));
	void* args[1];
	args[0] = MonoPath;

	return MonoInvoke<int>("VisitCountAtPathString", args);
}

////////////////////////////////////////////////////////
void UStoryState::ForceEnd()
{
	MonoInvoke<void>("ForceEnd", NULL);
}
