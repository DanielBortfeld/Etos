// © 2016 - 2017 Daniel Bortfeld 

#include "Etos.h"
#include "Etos/Buildings/Base/Building.h"
#include "Etos/Game/EtosPlayerController.h"
#include "BuildMenuButton.h"

bool UBuildMenuButton::Initialize()
{
	bool result = Super::Initialize();

	if (result)
	{
		BPEvent_OnInitialization();

		if (BuildButton)
		{
			BuildButton->OnReleased.AddDynamic(this, &UBuildMenuButton::SpawnBuilding);
		}
	}

	return result;
}

void UBuildMenuButton::SetButtonAndIcon(UButton * button, UImage * icon)
{
	this->BuildButton = button;
	this->BuildingIcon = icon;
}

void UBuildMenuButton::SpawnBuilding()
{
	UUtilityFunctionLibrary::GetFirstEtosPlayerController(this)->SpawnBuilding(Building, Data);
}
