// © 2016 - 2017 Daniel Bortfeld

#pragma once

#include "Etos/Utility/EnumLibrary.h"
#include "Blueprint/UserWidget.h"
#include "ResourceCostLayout.generated.h"

/**
 *
 */
UCLASS()
class ETOS_API UResourceCostLayout : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, Category = "LayoutData")
		struct FResource Cost;

public:

	UFUNCTION(BlueprintPure, Category = "LayoutData")
		int32 GetAvailableResources();

	UFUNCTION(BlueprintPure, Category = "LayoutData")
		FSlateColor GetTextColor();
};
