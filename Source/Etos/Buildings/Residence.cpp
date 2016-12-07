// © 2016 - 2017 Daniel Bortfeld

#include "Etos.h"
#include "Residence.h"
#include "TownCenter.h"
#include "Etos/Game/EtosPlayerController.h"
#include "Etos/Game/EtosGameMode.h"

void AResidence::BeginPlay()
{
	Super::BeginPlay();

	if (AEtosGameMode* const GM = Util::GetEtosGameMode(this))
	{
		auto asdf = GM->GetPeasantNeeds();
		for (auto asdfg : asdf.ResourceConsumptions)
		{
			resourceSatisfaction.FindOrAdd(asdfg.Resource);
		}
		for (auto qwert : asdf.ResidentNeeds)
		{
			needsSatisfaction.FindOrAdd(qwert);
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

void AResidence::OnBuild()
{
	RefreshBuildingsInRadius();

	Data.BuildingsInRadius.FindItemByClass<ATownCenter>(&myTownCenter);

	if (myTownCenter)
	{
		myTownCenter->UpdatePopulation(Residents);
	}
	else
	{
		for (auto asd : Data.BuildCost)
		{
			if (MyPlayerController)
			{
				MyPlayerController->AddResource(asd);
			}
		}
		Destroy();
	}
	Super::OnBuild();
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
			myTownCenter->UpdatePopulation(1);
		}
		else if (Residents > 0 && totalSatisfaction < 0.30)
		{
			--Residents;
			myTownCenter->UpdatePopulation(-1);
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
			switch (satisfaction.Key)
			{
			case EResource::None:
			case EResource::EResource_MAX:
				cleanMap = true;
			default:
				break;
			}

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
			switch (satisfaction.Key)
			{
			case EResidentNeed::None:
			case EResidentNeed::EResidentNeed_MAX:
				cleanMap = true;
				break;
			case EResidentNeed::TownCenter:
				satisfaction.Value = Data.BuildingsInRadius.FindItemByClass<ATownCenter>();
				break;
			case EResidentNeed::Chapel:
				//satisfaction.Value = Data.BuildingsInRadius.FindItemByClass<AChapel>();
				break;
			case EResidentNeed::Tavern:
				//satisfaction.Value = Data.BuildingsInRadius.FindItemByClass<ATavern>();
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
	}
}
