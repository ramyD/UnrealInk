// Copyright 2018 David Colson. All Rights Reserved.

#include "InkCompiler.h"

#include "Ink.h"
#include "MonoBaseClassImpl.h"

#include <mono/jit/jit.h>


////////////////////////////////////////////////////////
UInkCompiler::UInkCompiler()
{

}

////////////////////////////////////////////////////////
UInkCompiler* UInkCompiler::NewInkCompiler(FString inkFileContents, FString inkFileName)
{
	UInkCompiler* NewCompiler = NewObject<UInkCompiler>();

	MonoString* monoInkFileContents = mono_string_new(mono_domain_get(), TCHAR_TO_ANSI(*(inkFileContents)));
	MonoString* monoInkFileName = mono_string_new(mono_domain_get(), TCHAR_TO_ANSI(*(inkFileName)));
	void* args[2];
	args[0] = monoInkFileContents;
	args[1] = monoInkFileName;

	NewCompiler->MonoNew(args, 2);

	return NewCompiler;
}

////////////////////////////////////////////////////////
FString UInkCompiler::CompileToJson()
{
	return MonoInvoke<FString>("CompileToJson", NULL);
}

TArray<FString> UInkCompiler::GetErrors()
{
	TArray<FString> errors;

	MonoArray* result = MonoInvoke<MonoArray*>("GetErrors", nullptr);
	for (int i = 0; i < mono_array_length(result); i++)
	{
		MonoString* variableName = mono_array_get(result, MonoString*, i);
		errors.Add(FString(mono_string_to_utf8(variableName)));
	}

	return errors;
}

TArray<FString> UInkCompiler::GetPaths()
{
	TArray<FString> paths;

	MonoArray* result = MonoInvoke<MonoArray*>("GetPaths", nullptr);
	for (int i = 0; i < mono_array_length(result); i++)
	{
		MonoString* pathName = mono_array_get(result, MonoString*, i);
		paths.Add(FString(mono_string_to_utf8(pathName)));
	}

	return paths;
}

