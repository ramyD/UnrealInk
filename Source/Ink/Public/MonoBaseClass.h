// Copyright 2018 David Colson. All Rights Reserved.

#pragma once


#include "UObject/Object.h"

#include "Ink.h"

#include "MonoBaseClass.generated.h"


typedef struct _MonoArray MonoArray;
typedef struct _MonoObject MonoObject;
typedef struct _MonoClass MonoClass;
typedef struct _MonoMethod MonoMethod;

class UObject;

UCLASS()
class INK_API UMonoBaseClass : public UObject
{
	GENERATED_BODY()

public:
	UMonoBaseClass();

protected:
	// Creates a new instance of this class in the mono domain ready for use
	void MonoNew(void** params, int nParams);

	// Creates the C++ representation of an object that's already been created in the mono domain
	void NewFromInstance(MonoObject* MonoInstance);

	// TODO Convert to use Variadics
	// Function invocation methods
	template<typename type>
	inline type MonoInvoke(FString MethodName, void** params);


protected:
	void FindMethods();
	void ManualMethodBind(FString MethodName, int numParams);

	TMap<FString, MonoMethod*> Methods;

	MonoClass* Class{ nullptr };
	MonoObject* Instance{ nullptr };
};
