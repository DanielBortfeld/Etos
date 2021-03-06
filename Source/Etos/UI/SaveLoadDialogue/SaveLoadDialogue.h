// © 2016 - 2017 Daniel Bortfeld

#pragma once

class USaveLoadDialogueEntry;

#include "UMG.h"
#include "UMGStyle.h"
#include "Slate/SObjectWidget.h"
#include "IUMGModule.h"
#include "Blueprint/UserWidget.h"

#include "Blueprint/UserWidget.h"
#include "SaveLoadDialogue.generated.h"

/**
 *
 */
UCLASS()
class ETOS_API USaveLoadDialogue : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, Category = "Entry")
		TSubclassOf<USaveLoadDialogueEntry> SaveLoadDialogueEntryBlueprint;

	UPROPERTY(BlueprintReadOnly)
		USaveLoadDialogueEntry* SelectedEntry;

	UPROPERTY(BlueprintReadOnly)
		 FString NoSavesFound_Text = TEXT("No SaveGames found");

private:

	UPROPERTY()
		UScrollBox* dialogueEntryPanel;

	UPROPERTY()
		UEditableTextBox* editableTextBox;

public:

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintCallable, Category = "Referencing")
		void SetEntryPanel(UScrollBox* panel);

	UFUNCTION(BlueprintCallable, Category = "Referencing")
		void SetEditableTextBox(UEditableTextBox* textBox);

	UFUNCTION(BlueprintCallable, Category = "Entry")
		void PopulatePanel();

	UFUNCTION(BlueprintCallable, Category = "Entry")
		void SetSelectedEntry(USaveLoadDialogueEntry* Entry);

	UFUNCTION(BlueprintCallable, Category = "Entry")
		void ResetSelectedEntry();
};
