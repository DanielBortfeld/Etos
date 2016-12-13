// © 2016 - 2017 Daniel Bortfeld

#include "Etos.h"
#include "Residence.h"
#include "TownCenter.h"
#include "Etos/Game/EtosGameMode.h"
#include "Etos/Game/EtosPlayerController.h"
#include "Etos/Utility/FunctionLibraries/BuildingFunctionLibrary.h"

void AResidence::BeginPlay()
{
	Super::BeginPlay();

	if (AEtosGameMode* const GM = Util::GetEtosGameMode(this))
	{
		auto peasantNeeds = GM->GetPeasantNeeds();
		for (auto con : peasantNeeds.ResourceConsumptions)
		{
			resourceSatisfaction.FindOrAdd(con.Resource) = 1;
		}
		for (auto need : peasantNeeds.ResidentNeeds)
		{
			needsSatisfaction.FindOrAdd(need) = 1;
		}
	}
}

void AResidence::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Data.bIsBuilt)
	{
		UpdateSatisfaction(DeltaTime);
		MoveResidents(DeltaTime);
	}
}

void AResidence::UpgradeToCitizen()
{
	Upgrade_Internal("CitizenUpgrade");

	if (AEtosGameMode* const GM = Util::GetEtosGameMode(this))
	{
		auto needs = GM->GetCitizenNeeds();
		for (auto con : needs.ResourceConsumptions)
		{
			if (!resourceSatisfaction.Contains(con.Resource))
			{
				resourceSatisfaction.Add(con.Resource) = 0;
			}
		}
		for (auto need : needs.ResidentNeeds)
		{
			if (!needsSatisfaction.Contains(need))
			{
				needsSatisfaction.Add(need) = 0;
			}
		}
	}
}

void AResidence::Upgrade_Internal(const FName & UpgradeName)
{
	if (auto* const GM = Util::GetEtosGameMode(this))
	{
		if (MyPlayerController)
		{
			auto upgrade = *GM->GetUpgradeData(UpgradeName);

			if (!MyPlayerController->HasEnoughResources(upgrade.UpgradeCost))
				return;

			for (auto cost : upgrade.UpgradeCost)
			{
				MyPlayerController->RemoveResource(cost);
			}

			Data.BuildingIcon = upgrade.BuildingIcon;
			BuildingMesh = upgrade.Mesh;
			Data.Name = upgrade.Name;
			MaxResidents = upgrade.NewMaximum;

			if (upgrade.ProductionTime >= 0)
			{
				Data.ProductionTime = upgrade.ProductionTime;
			}

			MyPlayerController->UpdatePopulation(MyLevel, upgrade.ResidentLevel, Residents);
			MyLevel = upgrade.ResidentLevel;
		}
	}
}

float AResidence::GetSatisfaction(const EResource & resource)
{
	float satisfaction = 0;

	if (Enum::IsValid(resource))
	{
		satisfaction = resourceSatisfaction.FindOrAdd(resource);
	}

	return satisfaction;
}

void AResidence::OnBuild()
{
	RefreshBuildingsInRadius();

	Data.BuildingsInRadius.FindItemByClass<ATownCenter>(&myTownCenter);

	if (myTownCenter)
	{
		myTownCenter->UpdatePopulation(MyLevel, Residents);
		Super::OnBuild();
	}
	else
	{
		for (auto cost : Data.BuildCost)
		{
			if (MyPlayerController)
			{
				MyPlayerController->AddResource(cost);
			}
		}
		Destroy();
	}
}

void AResidence::BindDelayAction()
{
	if (Action.IsBound())
	{
		Action.Unbind();
	}
}

void AResidence::MoveResidents(float DeltaTime)
{
	residentTimerPassed += DeltaTime;

	if (residentTimerPassed >= residentTimerTotal)
	{
		residentTimerPassed = 0;

		if (Residents < MaxResidents && totalSatisfaction >= 0.60)
		{
			++Residents;
			myTownCenter->UpdatePopulation(MyLevel, 1);
		}
		else if (Residents > 0 && totalSatisfaction < 0.30)
		{
			--Residents;
			myTownCenter->UpdatePopulation(MyLevel, -1);
		}
	}
}

void AResidence::UpdateSatisfaction(float DeltaTime)
{
	needsTimerPassed += DeltaTime;

	if (needsTimerPassed >= needsTimerTotal)
	{
		needsTimerPassed = 0;

		totalSatisfaction = 0;
		float deltaSatisfaction = 0.03;

		bool cleanMap = false;

		for (auto& satisfaction : resourceSatisfaction)
		{
			cleanMap = !Enum::IsValid(satisfaction.Key);

			if (cleanMap)
				continue;

			if (myTownCenter&& myTownCenter->HasResource(satisfaction.Key))
			{
				if (satisfaction.Value < 1)
				{
					satisfaction.Value += deltaSatisfaction;
				}
			}
			else
			{
				if (satisfaction.Value > 0)
				{
					satisfaction.Value -= deltaSatisfaction;
				}
			}

			totalSatisfaction += satisfaction.Value;
		}

		if (cleanMap)
		{
			resourceSatisfaction.Remove(EResource::None);
			resourceSatisfaction.Remove(EResource::EResource_MAX);
			resourceSatisfaction.Shrink();
			cleanMap = false;
		}

		RefreshBuildingsInRadius();
		for (auto& satisfaction : needsSatisfaction)
		{
			cleanMap = !Enum::IsValid(satisfaction.Key);

			if (cleanMap)
				continue;

			switch (satisfaction.Key)
			{
			case EResidentNeed::TownCenter:
				satisfaction.Value = IsConnectedTo<ATownCenter>();
				break;
			case EResidentNeed::Chapel:
				//satisfaction.Value = IsConnectedTo<AChapel>();
				break;
			case EResidentNeed::Tavern:
				//satisfaction.Value = IsConnectedTo<ATavern>();
				break;
			default:
				break;
			}

			totalSatisfaction += satisfaction.Value;
		}

		if (cleanMap)
		{
			needsSatisfaction.Remove(EResidentNeed::None);
			needsSatisfaction.Remove(EResidentNeed::EResidentNeed_MAX);
			needsSatisfaction.Shrink();
		}

		totalSatisfaction /= (resourceSatisfaction.Num() + needsSatisfaction.Num());

		if (totalSatisfaction >= 0.95f)
		{
			switch (MyLevel)
			{
			case EResidentLevel::Citizen:
				break;
			case EResidentLevel::Peasant:
				UpgradeToCitizen();
				break;
			default:
				break;
			}
		}
	}
}

template<class TBuilding>
inline bool AResidence::IsConnectedTo()
{
	TBuilding* building;
	return Data.BuildingsInRadius.FindItemByClass<TBuilding>(&building) && BFuncs::FindPath(this, dynamic_cast<ABuilding*, TBuilding>(building));
}
