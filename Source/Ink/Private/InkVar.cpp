#include "InkVar.h"

FInkVar::FInkVar(const FInkVarInterop& other)
	: type(other.type),
	intVar(other.intVal),
	floatVar(other.floatVal),
	stringVar(other.stringVal)
{
}

FInkVarInterop FInkVar::ToInterop() const
{
	FInkVarInterop result = { type, floatVar, intVar, nullptr };
	if (type == EInkVarType::String)
	{
		UE_LOG(LogInk, Warning, TEXT("Sending strings to C# via InkVar not currently supported!"));
	}
	return result;
}
