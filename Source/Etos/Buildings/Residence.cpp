// © 2016 - 2017 Daniel Bortfeld

#include "Etos.h"
#include "Residence.h"
#include "TownCenter.h"
#include "Chapel.h"
#include "Etos/Game/EtosGameMode.h"
#include "Etos/Game/EtosPlayerController.h"
#include "Etos/Utility/FunctionLibraries/BuildingFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"

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

void AResidence::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* const World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(NeedsUpdateTimerHandle);
		World->GetTimerManager().ClearTimer(ResidentUpdateTimerHandle);
	}
	Super::EndPlay(EndPlayReason);
}

void AResidence::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Data.bIsBuilt && myTownCenter->IsValidLowLevel())
	{
		UpdateSatisfaction(DeltaTime);
		MoveResidents(DeltaTime);
	}
}

void AResidence::Demolish()
{
	if (myTownCenter && myTownCenter->IsValidLowLevel())
	{
		myTownCenter->UpdatePopulation(MyLevel, -Residents);
	}
	else if (auto* PC = Util::GetFirstEtosPlayerController(this))
	{
		PC->UpdatePopulation(MyLevel, -Residents);
	}

	Super::Demolish();
}

void AResidence::UpgradeToCitizen()
{
	// internal code is always the same for all upgrades
	Upgrade_Internal(TEXT("CitizenUpgrade"));

	// special to citizens
	if (MyLevel == EResidentLevel::Citizen) // if upgrade was successful
	{
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
}

void AResidence::Upgrade_Internal(const FName & UpgradeName)
{
	if (auto* const GM = Util::GetEtosGameMode(this))
	{
		if (MyPlayerController)
		{
			auto upgradeData = *GM->GetUpgradeData(UpgradeName);

			if (!MyPlayerController->HasEnoughResources(upgradeData.UpgradeCost))
				return;

			for (auto cost : upgradeData.UpgradeCost)
			{
				MyPlayerController->RemoveResource(cost);
			}

			Data.BuildingIcon = upgradeData.BuildingIcon;
			BuildingMesh->SetStaticMesh(Util::LoadObjFromPath<UStaticMesh>(upgradeData.MeshPath));
			Data.Name = upgradeData.Name;
			MaxResidents = upgradeData.NewMaximum;
			
			OnBuild_ParticleSystem->Activate(true);

			if (upgradeData.ProductionTime >= 0)
			{
				Data.ProductionTime = upgradeData.ProductionTime;
			}

			MyPlayerController->ReportUpgrade(this, MyLevel, upgradeData.ResidentLevel);
			MyPlayerController->UpdatePopulation(MyLevel, upgradeData.ResidentLevel, Residents);
			MyLevel = upgradeData.ResidentLevel;
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

float AResidence::GetSatisfaction(const EResidentNeed & need)
{
	float satisfaction = 0;

	if (Enum::IsValid(need))
	{
		satisfaction = needsSatisfaction.FindOrAdd(need);
	}

	return satisfaction;
}

void AResidence::GetAllSatisfactions(TMap<EResource, float>& ResourceSatisfaction, TMap<EResidentNeed, bool>& NeedsSatisfaction, float& TotalSatisfaction)
{
	ResourceSatisfaction = resourceSatisfaction;
	NeedsSatisfaction = needsSatisfaction;
	TotalSatisfaction = totalSatisfaction;
}

void AResidence::SetAllSatisfactions(const TMap<EResource, float>& ResourceSatisfaction, const TMap<EResidentNeed, bool>& NeedsSatisfaction, const float & TotalSatisfaction)
{
	resourceSatisfaction = ResourceSatisfaction;
	needsSatisfaction = NeedsSatisfaction;
	totalSatisfaction = TotalSatisfaction;
}

void AResidence::Build()
{
	RefreshBuildingsInRadius();

	Data.BuildingsInRadius.FindItemByClass<ATownCenter>(&myTownCenter);

	if (myTownCenter)
	{
		myTownCenter->UpdatePopulation(MyLevel, Residents);

		if (UWorld* const World = GetWorld())
		{
			World->GetTimerManager().SetTimer(ResidentUpdateTimerHandle, this, &AResidence::MoveResidents, residentTimerTotal, true, residentTimerTotal);
			World->GetTimerManager().SetTimer(NeedsUpdateTimerHandle, this, &AResidence::UpdateSatisfaction, needsTimerTotal, true, needsTimerTotal);
		}

		Super::Build();
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
	//residentTimerPassed += DeltaTime;

	//if (residentTimerPassed >= residentTimerTotal)
	//{
	//	residentTimerPassed = 0;

	if (Residents < MaxResidents && totalSatisfaction > 0.65)
	{
		++Residents;
		if (myTownCenter && myTownCenter->IsValidLowLevel())
		{
		myTownCenter->UpdatePopulation(MyLevel, 1);
		}
		else if (MyPlayerController && MyPlayerController->IsValidLowLevel())
		{
			MyPlayerController->UpdatePopulation(MyLevel, 1);
		}
	}
	else if (Residents > 0 && totalSatisfaction < 0.50)
	{
		--Residents;
		if (myTownCenter && myTownCenter->IsValidLowLevel())
		{
			myTownCenter->UpdatePopulation(MyLevel, -1);
		}
		else if (MyPlayerController && MyPlayerController->IsValidLowLevel())
		{
			MyPlayerController->UpdatePopulation(MyLevel, -1);
		}
	}
	//}
}

void AResidence::UpdateSatisfaction(float DeltaTime)
{
	//needsTimerPassed += DeltaTime;

	//if (needsTimerPassed >= needsTimerTotal)
	//{
	//	needsTimerPassed = 0;

	totalSatisfaction = 0;
	float deltaSatisfaction = 0.03;

	bool cleanMap = false;

	for (auto& satisfaction : resourceSatisfaction)
	{
		cleanMap = !Enum::IsValid(satisfaction.Key);

		if (cleanMap)
			continue;

		if (needsSatisfaction.FindOrAdd(EResidentNeed::MarketPlace) &&
			myTownCenter->IsValidLowLevel() && myTownCenter->HasResource(satisfaction.Key))
		{
			if (satisfaction.Value < 1)
			{
				satisfaction.Value = UKismetMathLibrary::FClamp(satisfaction.Value + deltaSatisfaction, 0, 1);;
			}
		}
		else
		{
			if (satisfaction.Value > 0)
			{
				satisfaction.Value = UKismetMathLibrary::FClamp(satisfaction.Value - deltaSatisfaction, 0, 1);;
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
		case EResidentNeed::MarketPlace:
			satisfaction.Value = IsConnectedTo<ATownCenter>();
			break;
		case EResidentNeed::Chapel:
			satisfaction.Value = IsConnectedTo<AChapel>();
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

	if (totalSatisfaction >= 0.95f && Residents == MaxResidents)
	{
		if (MyPlayerController)
		{
			switch (MyLevel)
			{
			case EResidentLevel::Citizen:
				break;
			case EResidentLevel::Peasant:
				if (MyPlayerController->GetPopulationAmount(EResidentLevel::Peasant) >= 90)
				{
					if (MyPlayerController->GetAvailablePromotions(EResidentLevel::Citizen) > 0)
					{
						UpgradeToCitizen();
					}
				}
				break;
			default:
				break;
			}
		}
	}
	//}
}

void AResidence::MoveResidents()
{
	MoveResidents(0);
}

void AResidence::UpdateSatisfaction()
{
	UpdateSatisfaction(0);
}

template<class TBuilding>
inline bool AResidence::IsConnectedTo()
{
	TBuilding* building;
	return Data.BuildingsInRadius.FindItemByClass<TBuilding>(&building) && BFuncs::FindPath(this, dynamic_cast<ABuilding*, TBuilding>(building));
}
