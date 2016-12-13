// © 2016 - 2017 Daniel Bortfeld

#include "Etos.h"
#include "SimpleCollisionManager.h"
#include "BoxCollider.h"

// Sets default values
ASimpleCollisionManager::ASimpleCollisionManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called every frame
void ASimpleCollisionManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CheckCollisions();
}

void ASimpleCollisionManager::RegisterCollider(UBoxCollider * Collider)
{
	registeredColliders.AddUnique(Collider);
}

void ASimpleCollisionManager::UnregisterCollider(UBoxCollider * Collider)
{
	if (registeredColliders.Contains(Collider))
	{
		for (UBoxCollider* A : registeredColliders)
		{
			A->CheckCollision(Collider);
		}
		registeredColliders.Remove(Collider);
	}
}

void ASimpleCollisionManager::CheckCollisions()
{
	for (UBoxCollider* A : registeredColliders)
	{
		if (A && A->IsValidLowLevel())
		{
			for (UBoxCollider* B : registeredColliders)
			{
				if (A != B)
				{
					if (B && B->IsValidLowLevel())
					{
						A->CheckCollision(B);
					}
				}
			}
		}
	}
}

