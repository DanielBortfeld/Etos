// © 2016 - 2017 Daniel Bortfeld

#pragma once

#include "Buildings/Base/Building.h"
#include "TownCenter.generated.h"

/**
 *
 */
UCLASS()
class ETOS_API ATownCenter : public ABuilding
{
	GENERATED_BODY()

private:

	UPROPERTY(VisibleAnywhere)
		TMap<EResource, float> consumedResources;

	UPROPERTY(VisibleAnywhere)
		float payedTaxes;

	UPROPERTY(VisibleAnywhere)
		int32 registeredResidents;

	UPROPERTY(VisibleAnywhere)
		float taxPerResident = 1.17f;

	UPROPERTY(VisibleAnywhere)
		FResidentNeeds peasantNeeds;

public:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	bool HasResource(EResource resource);

	void PayTaxes(float DeltaTime);

	void ConsumeResources(float DeltaTime);

	void UpdatePopulation(int32 delta);

protected:

	virtual void BindDelayAction() override;
};
