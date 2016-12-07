// © 2016 - 2017 Daniel Bortfeld

#pragma once

class ATownCenter;
#include "Buildings/Base/Building.h"
#include "Residence.generated.h"

/**
 *
 */
UCLASS()
class ETOS_API AResidence : public ABuilding
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Residents")
		int32 Residents = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Residents")
		int32 MaxResidents = 8;

private:

	UPROPERTY(VisibleAnywhere)
		TMap<EResource, float> resourceSatisfaction;

	UPROPERTY(VisibleAnywhere)
		TMap<EResidentNeed, bool> needsSatisfaction;

	UPROPERTY(VisibleAnywhere)
		float totalSatisfaction = 0.5;

	UPROPERTY(VisibleAnywhere)
		ATownCenter* myTownCenter;

	UPROPERTY()
		float residentTimerTotal = 28;

	UPROPERTY()
		float residentTimerPassed = 0;

	UPROPERTY()
		float needsTimerTotal = 4;

	UPROPERTY()
		float needsTimerPassed = 0;

public:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

protected:

	virtual void OnBuild() override;

	virtual void BindDelayAction() override;

private:

	void MoveResidents(float DeltaTime);

	void UpdateSatisfaction(float DeltaTime);
};
