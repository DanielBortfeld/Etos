// � 2016 - 2017 Daniel Bortfeld

#include "Etos.h"
#include "EtosGameMode.h"
#include "Runtime/Engine/Public/GameDelegates.h"
#include "EtosPlayerController.h"

UTexture2D* AEtosGameMode::defaultTexture = nullptr;

AEtosGameMode::AEtosGameMode()
{
	PrimaryActorTick.bCanEverTick = true;

	typedef ConstructorHelpers::FObjectFinder<UDataTable> FDataTableFinder;
	static FDataTableFinder DataTableFinder(TEXT("DataTable'/Game/Blueprints/DataTables/PredefinedBuildingData.PredefinedBuildingData'"));

	if (!PredefinedBuildingData)
	{
		DataTableFinder = FDataTableFinder(TEXT("DataTable'/Game/Blueprints/DataTables/PredefinedBuildingData.PredefinedBuildingData'"));
		if (DataTableFinder.Succeeded())
		{
			PredefinedBuildingData = DataTableFinder.Object;
		}
	}

	if (!UpgradeData)
	{
		DataTableFinder = FDataTableFinder(TEXT("DataTable'/Game/Blueprints/DataTables/UpgradeData.UpgradeData'"));
		if (DataTableFinder.Succeeded())
		{
			UpgradeData = DataTableFinder.Object;
		}
	}

	if (!TaxData)
	{
		DataTableFinder = FDataTableFinder(TEXT("DataTable'/Game/Blueprints/DataTables/TaxData.TaxData'"));
		if (DataTableFinder.Succeeded())
		{
			TaxData = DataTableFinder.Object;
		}
	}

	DataTableFinder = FDataTableFinder(TEXT("DataTable'/Game/Blueprints/DataTables/ResourceIconPairings.ResourceIconPairings'"));
	if (DataTableFinder.Succeeded())
	{
		UDataTable* Icons = DataTableFinder.Object;
		const int32 pairCount = Icons->GetRowNames().Num();

		Util::ResourceIcons.Empty(pairCount);

		for (int32 i = 0; i < pairCount; ++i)
		{
			FResourceIconPair* pair = Icons->FindRow<FResourceIconPair>(*FString::FromInt(i), FString(TEXT("GameMode")));
			Util::ResourceIcons.Add(pair->Resource, FIcon(pair->Icon_Smol, pair->Icon_Big));
		}
	}

	if (!defaultTexture)
	{
		ConstructorHelpers::FObjectFinder<UTexture2D> TextureFinder(TEXT("Texture2D'/Game/NewTexture2D.NewTexture2D'"));
		if (TextureFinder.Succeeded())
		{
			defaultTexture = TextureFinder.Object;
		}
	}

	checkf(defaultTexture, TEXT("Default Texture could not be found"));

	if (UWorld* const World = GetWorld())
	{
		CollisionManager = nullptr;

		for (TActorIterator<ASimpleCollisionManager> ActorItr(World); ActorItr; ++ActorItr)
		{
			if (*ActorItr != nullptr)
			{
				CollisionManager = *ActorItr;
				break;
			}
		}

		if (!CollisionManager)
		{
			UE_LOG(LogTemp, Error, TEXT("There is no Collision Manager in the level."));
		}
	}

	taxPerResidentPerMinute.Add(EResidentLevel::Peasant, 1.17f);

	FGameDelegates::Get().GetEndPlayMapDelegate().AddLambda([&]
	{
		//PredefinedBuildingData = nullptr;
		CollisionManager = nullptr;
	});
}

void AEtosGameMode::Tick(float DeltaTime)
{
	checkWinTimerPassed += DeltaTime;

	if (checkWinTimerPassed >= checkWinTimerTotal)
	{
		checkWinTimerPassed = 0;

		if (UWorld* const World = GetWorld())
		{
			AEtosPlayerController* winner = nullptr;

			for (TActorIterator<AEtosPlayerController> ActorItr(World); ActorItr; ++ActorItr)
			{
				int32 population = ActorItr->GetTotalPopulation();

				if (population >= populationWinAmount)
				{
					winner = *ActorItr;
					winner->Win();
					break;
				}
			}

			if (winner)
			{
				for (TActorIterator<AEtosPlayerController> ActorItr(World); ActorItr; ++ActorItr)
				{
					if (winner != *ActorItr)
					{
						ActorItr->Lose();
					}
				}
			}
		}
	}
}

FPredefinedBuildingData* AEtosGameMode::GetPredefinedBuildingData(const int32& buildingID)
{
	check(PredefinedBuildingData);

	// DataTable.h:97 variable swap error -> fixed in 4.15 or earlier
	// UObjectGlobals.h typo in NewObject assert

	FPredefinedBuildingData* preDefData = PredefinedBuildingData->FindRow<FPredefinedBuildingData>(*FString::FromInt(buildingID), FString(TEXT("GameMode")));

	return preDefData;
}

FUpgradeData * AEtosGameMode::GetUpgradeData(const FName & upgrade)
{
	check(UpgradeData);

	FUpgradeData* data = UpgradeData->FindRow<FUpgradeData>(upgrade, FString(TEXT("GameMode")));

	return data;
}

FTaxData * AEtosGameMode::GetTaxData(const EResidentLevel & level)
{
	check(TaxData);

	FTaxData* data = nullptr;

	switch (level)
	{
	case EResidentLevel::Peasant:
		data = TaxData->FindRow<FTaxData>("Peasant", FString(TEXT("GameMode")));
		break;
	case EResidentLevel::Citizen:
		data = TaxData->FindRow<FTaxData>("Citizen", FString(TEXT("GameMode")));
		break;
	default:
		break;
	}

	return data;
}

int32 AEtosGameMode::GetBuildingAmount()
{
	return PredefinedBuildingData ? PredefinedBuildingData->GetRowNames().Num() : 0;
}

FResidentNeeds AEtosGameMode::GetPeasantNeeds()
{
	FResidentNeeds needs = FResidentNeeds(EResidentLevel::Peasant);

	needs.ResidentNeeds.Add(EResidentNeed::MarketPlace);
	//needs.ResidentNeeds.Add(EResidentNeed::Chapel);

	needs.ResourceConsumptions.Add(FResourceConsumption(EResource::Fish, 200, 2));
	//needs.ResourceConsumptions.Add(FResourceConsumption(EResource::Most, 340, 1.5));

	return needs;
}

FResidentNeeds AEtosGameMode::GetCitizenNeeds()
{
	FResidentNeeds needs = FResidentNeeds(EResidentLevel::Citizen);

	needs.ResidentNeeds.Add(EResidentNeed::MarketPlace);
	needs.ResidentNeeds.Add(EResidentNeed::Chapel);
	//needs.ResidentNeeds.Add(EResidentNeed::Tavern);

	needs.ResourceConsumptions.Add(FResourceConsumption(EResource::Fish, 500, 2));
	needs.ResourceConsumptions.Add(FResourceConsumption(EResource::Most, 340, 1.5));

	return needs;
}

float AEtosGameMode::GetBaseTaxForResident(const EResidentLevel& level)
{
	float tax = 0;

	if (auto const taxData = GetTaxData(level))
	{
		tax = taxData->BaseTax;
	}

	return tax;
}

int32 AEtosGameMode::GetPopulationWinAmount()
{
	return populationWinAmount;
}

UTexture2D * AEtosGameMode::GetDefaultTexture()
{
	return defaultTexture;
}
