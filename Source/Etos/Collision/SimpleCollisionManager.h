// © 2016 - 2017 Daniel Bortfeld

#pragma once

#include "GameFramework/Actor.h"
#include "SimpleCollisionManager.generated.h"

UCLASS()
class ETOS_API ASimpleCollisionManager : public AActor
{
	GENERATED_BODY()
	
private:

	TArray<class UBoxCollider*> registeredColliders = TArray<UBoxCollider*>();

public:	
	// Sets default values for this actor's properties
	ASimpleCollisionManager();

	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	void RegisterCollider(UBoxCollider* Collider);

	void UnregisterCollider(UBoxCollider* Collider);

private:
	
	void CheckCollisions();
};
