// © 2016 - 2017 Daniel Bortfeld

#pragma once

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
		int32 Residents;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Residents")
		int32 MaxResidents;

private:

	UPROPERTY()
		TMap<EResource, int32> storedResources;

public:

	virtual void Tick(float DeltaTime) override;

protected:

	virtual void BindDelayAction() override;

private:

	UFUNCTION()
		void ProduceMoney();
};
