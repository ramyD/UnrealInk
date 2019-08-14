#pragma once

#include "InkVar.generated.h"

// A wrapper for passing around ink vars to and fro ink itself
// Not templated so it can be used in blueprints
UENUM(BlueprintType)
enum class EInkVarType : uint8
{
	Float,
	Int,
	String,
	None
};

// Used to pass ink variable data from C# to C++ and back
//  DO NOT CHANGE THE DATA LAYOUT OF THIS STRUCT WITHOUT CHANGING
//  GlueStory.cs::InkVar
struct FInkVarInterop
{
	EInkVarType type;
	float floatVal;
	int32 intVal;
	const char* stringVal;
};

USTRUCT(BlueprintType)
struct FInkVar
{
	GENERATED_BODY()

	FInkVar() { type = EInkVarType::None; }

	FInkVar(float val) { type = EInkVarType::Float; floatVar = val; }
	FInkVar(int val) { type = EInkVarType::Int; intVar = val; }
	FInkVar(FString val) { type = EInkVarType::String; stringVar = val; }
	FInkVar(const FInkVarInterop& other);

	FInkVarInterop ToInterop() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ink")
	EInkVarType type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ink")
	float floatVar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ink")
	int intVar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ink")
	FString stringVar;
};