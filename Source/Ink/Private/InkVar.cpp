#include "InkVar.h"

#include "Misc/AssertionMacros.h"

FInkVar::FInkVar(const FInkVarInterop& other)
	: type(other.type),
	floatVar(other.floatVal),
	intVar(other.intVal),
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

FString UInkVarLibrary::Conv_InkVarString(const FInkVar& InkVar)
{
	if (ensureMsgf(InkVar.type == EInkVarType::String, TEXT("InkVar is not a String Type!")))
		return InkVar.stringVar;
	return FString(TEXT(""));
}

int UInkVarLibrary::Conv_InkVarInt(const FInkVar& InkVar)
{
	if (ensureMsgf(InkVar.type == EInkVarType::Int, TEXT("InkVar is not an Int Type!")))
		return InkVar.intVar;
	return 0;
}

float UInkVarLibrary::Conv_InkVarFloat(const FInkVar& InkVar)
{
	if (ensureMsgf(InkVar.type == EInkVarType::Float, TEXT("InkVar is not a Float Type!")))
		return InkVar.floatVar;
	return 0.f;
}

FName UInkVarLibrary::Conv_InkVarName(const FInkVar& InkVar)
{
	if (ensureMsgf(InkVar.type == EInkVarType::String, TEXT("InkVar is not a String Type!")))
		return FName(*InkVar.stringVar);
	return NAME_None;
}

FText UInkVarLibrary::Conv_InkVarText(const FInkVar& InkVar)
{
	if (ensureMsgf(InkVar.type == EInkVarType::String, TEXT("InkVar is not a String Type!")))
		return FText::FromString(InkVar.stringVar);
	return FText::GetEmpty();
}

bool UInkVarLibrary::Conv_InkVarBool(const FInkVar& InkVar)
{
	if (ensureMsgf(InkVar.type == EInkVarType::Int, TEXT("InkVar is not an Int Type!")))
		return InkVar.intVar > 0;
	return false;
}

FInkVar UInkVarLibrary::Conv_StringInkVar(const FString& String)
{
	return FInkVar(String);
}

FInkVar UInkVarLibrary::Conv_IntInkVar(int Number)
{
	return FInkVar(Number);
}

FInkVar UInkVarLibrary::Conv_FloatInkVar(float Number)
{
	return FInkVar(Number);
}

FInkVar UInkVarLibrary::Conv_TextInkVar(const FText& Text)
{
	return FInkVar(Text.ToString());
}

FInkVar UInkVarLibrary::Conv_BoolInkVar(bool Boolean)
{
	return FInkVar(Boolean ? 1 : 0);
}