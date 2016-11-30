// © 2016 - 2017 Daniel Bortfeld

#include "Etos.h"
#include "Residence.h"

void AResidence::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Data.bIsBuilt)
	{

	}
}

void AResidence::BindDelayAction()
{
	if (Action.IsBound())
	{
		Action.Unbind();
	}

	Action.BindDynamic(this, &AResidence::ProduceMoney);
}

void AResidence::ProduceMoney()
{

}
