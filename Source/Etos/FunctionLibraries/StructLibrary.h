// © 2016 - 2017 Daniel Bortfeld

#pragma once

#include "EnumLibrary.h"
#include "StructLibrary.generated.h"

/**
 *
 */
USTRUCT(BlueprintType)
struct FResource
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Resource)
		EResource Type = EResource::None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Resource)
		int32 Amount = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Resource)
		UTexture2D* Icon;

public:

	FResource(EResource Type = EResource::None, int32 Amount = 0, UTexture2D* Icon = nullptr)
	{
		if (Type != EResource::None)
		{
			this->Type = Type;
			this->Amount = Amount;
			this->Icon = Icon;
		}
	}
};

/**
*
*/
USTRUCT(BlueprintType)
struct FBuildingData
{
	GENERATED_BODY()

public:

	// The name of the building
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName Name = FName(TEXT("New Building"));
	// The icon of the building
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTexture2D* BuildingIcon;


	// Whether this building is held by the mouse of the player
	// true means it's not bulit and it follows the mouse
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool bIsHeld = false;
	// Whether something blocks the buildings position in buildmode (when it's held and not built)
	// true means you can't build here
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool bPositionIsBlocked = false;
	// Whether this building was built
	// Its built when the resources are spend for building and when 
	// it was placed in the world
	// It calls its delay action when built
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool bIsBuilt = false;
	// The resources required to build this building
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TArray<FResource> BuildCost;
	// The paths this building could be connected with when it was built right here
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TArray<class APath*> PossibleConnections;
	// The bulidings that could be in the radius of this building if it was built right here
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TArray<class ABuilding*> PossibleBuildingsInRadius;


	// The Money this buildings needs per minute
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Upkeep;
	// The first resource needed for production
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FResource NeededResource1;
	// The second resource needed for production
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FResource NeededResource2;
	// The resource this building produces (or already has produced)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FResource ProducedResource;
	// The time it takes for a building to produce a resource 
	// (or call its respective delay action)
	// <= 0 means dont call a delay action
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float ProductionTime;
	// The maximum amount of each needed and produced resources this 
	// building can store
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 MaxStoredResources;


	// The buildings inside the radius of this buildings
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TArray<ABuilding*> BuildingsInRadius;
	// Whether there is already a market barrow on the way to this building
	// to get the resources
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool bBarrowIsOnTheWay;
	// The Path/Road tiles connected to this buildling
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TArray<APath*> PathConnections;
	// The radius of the building in cm
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Radius;
};

/**
*
*/
USTRUCT(BlueprintType)
struct FPredefinedBuildingData : public FTableRowBase
{
	GENERATED_BODY()

public:

	// The name of the building
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PredefinedBuildingData)
		FName Name = FName(TEXT("New Building"));
	// The blueprint of this building containing its mesh and its size
	// (should derive from ABuilding)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PredefinedBuildingData)
		TSubclassOf<ABuilding> BuildingBlueprint = nullptr;
	// The icon of the building
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PredefinedBuildingData)
		UTexture2D* BuildingIcon = nullptr;

	// The resources required to build this building
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PredefinedBuildingData)
		TArray<FResource> BuildCost = TArray<FResource>();

	// The Money this buildings needs per minute
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Upkeep;
	// The first resource needed for production
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PredefinedBuildingData)
		EResource NeededResource1 = EResource::None;
	// The second resource needed for production
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PredefinedBuildingData)
		EResource NeededResource2 = EResource::None;
	// The resource this building sould produce
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PredefinedBuildingData)
		EResource ProducedResource = EResource::None;
	// The time it takes for a building to produce a resource 
	// (or call its respective delay action)
	// <= 0 means dont call a delay action
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PredefinedBuildingData)
		float ProductionTime = 0.f;
	// The maximum amount of each needed and produced resources this 
	// building can store
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PredefinedBuildingData)
		int32 MaxStoredResources = 5;

	// The radius of the building in cm
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PredefinedBuildingData)
		float Radius = 500;
};

/**
*
*/
USTRUCT(BlueprintType)
struct FResourceConsumption : public FTableRowBase
{
	GENERATED_BODY()

public:

	EResource Resource;
	int32 Residents;
	float ConsumptionPerMinute;

public:

	FResourceConsumption(){}

	FResourceConsumption(EResource Resource, int32 Residents, float ConsumptionPerMinute)
	{
		this->Resource = Resource;
		this->Residents = Residents;
		this->ConsumptionPerMinute = ConsumptionPerMinute;
	}
};

/**
*
*/
USTRUCT(BlueprintType)
struct FResidentNeeds : public FTableRowBase
{
	GENERATED_BODY()

public:

	UPROPERTY()
		EResidentLevel ResidentLevel;

	UPROPERTY()
		TArray<FResourceConsumption> ResourceConsumptions;

	UPROPERTY()
		TArray<EResidentNeed> ResidentNeeds;

public:

	FResidentNeeds(EResidentLevel ResidentLevel = EResidentLevel::Peasant)
	{
		this->ResidentLevel = ResidentLevel;
	}
};
