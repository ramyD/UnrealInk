#pragma once 

#include "MonoBaseClass.h"

#include <mono/jit/jit.h>

template<typename type>
inline type UMonoBaseClass::MonoInvoke(FString MethodName, void** params)
{
	FInkModule& Ink = FModuleManager::GetModuleChecked<FInkModule>("Ink");

	MonoObject* Exception;
	MonoObject* ret = mono_runtime_invoke(Methods[MethodName], Instance, params, &Exception);
	Ink.MaybeThrowMonoException(Exception);

	return *(type*)mono_object_unbox(ret);
}

template<>
inline void UMonoBaseClass::MonoInvoke<void>(FString MethodName, void** params)
{
	FInkModule& Ink = FModuleManager::GetModuleChecked<FInkModule>("Ink");

	MonoObject* Exception;
	mono_runtime_invoke(Methods[MethodName], Instance, params, &Exception);
	Ink.MaybeThrowMonoException(Exception);
}

template <>
inline MonoObject* UMonoBaseClass::MonoInvoke<MonoObject*>(FString MethodName, void** params)
{
	FInkModule& Ink = FModuleManager::GetModuleChecked<FInkModule>("Ink");

	MonoObject* Exception;
	MonoObject* ret = mono_runtime_invoke(Methods[MethodName], Instance, params, &Exception);
	Ink.MaybeThrowMonoException(Exception);
	return ret;
}

template <>
inline MonoArray* UMonoBaseClass::MonoInvoke<MonoArray*>(FString MethodName, void** params)
{
	FInkModule& Ink = FModuleManager::GetModuleChecked<FInkModule>("Ink");

	MonoObject* Exception;
	MonoArray* ret = (MonoArray*)mono_runtime_invoke(Methods[MethodName], Instance, params, &Exception);
	Ink.MaybeThrowMonoException(Exception);
	return ret;
}

template <>
inline FString UMonoBaseClass::MonoInvoke<FString>(FString MethodName, void** params)
{
	FInkModule& Ink = FModuleManager::GetModuleChecked<FInkModule>("Ink");

	MonoObject* Exception;
	MonoString* ret = (MonoString*)mono_runtime_invoke(Methods[MethodName], Instance, params, &Exception);
	Ink.MaybeThrowMonoException(Exception);

	return FString(mono_string_to_utf8(ret));
}