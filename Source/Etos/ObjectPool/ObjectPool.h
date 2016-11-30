// © 2016 - 2017 Daniel Bortfeld

#pragma once

#include "GameFramework/Actor.h"
#include "ObjectPool.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FResetObjectDelegate, AActor*, ResetObject);

USTRUCT()
struct FObjectPool
{
	GENERATED_BODY()

public:

	UPROPERTY()
		FResetObjectDelegate ResetObjectEvent;

private:

	UPROPERTY()
		int32 minPooledObjects = 1;
	UPROPERTY()
		int32 maxPooledObjects = 10;

	UPROPERTY()
		TArray<UObject*> pooledObjects;

public:

	// Sets default values for this actor's properties
	FObjectPool();

	template<class T>
	bool AddObjectToPool(T* Object);

	template<class T>
	T* GetPooledObject(bool& bIsValid);

	int32 GetMinPooledObjectsAmount();

	void SetMinPooledObjectsAmount(int32 minPooledObjects);

	int32 GetMaxPooledObjectsAmount();

	void SetMaxPooledObjectsAmount(int32 maxPooledObjects);
};

template<class T>
inline bool FObjectPool::AddObjectToPool(T * Object)
{
	bool success = false;
	if (pooledObjects.Num() + 1 <= maxPooledObjects)
	{
		pooledObjects.Push(Object);
		success = true;
	}

	return success;
}

template<class T>
inline T * FObjectPool::GetPooledObject(bool& bIsValid)
{
	bIsValid = false;

	const int32 pooledObjectsNum = pooledObjects.Num();
	if (pooledObjectsNum > 0)
	{
		bool allowShrinking = pooledObjectsNum - 1 > minPooledObjects;
		T* pooledObject = dynamic_cast<T*, UObject>(pooledObjects.Pop(allowShrinking));

		if (pooledObject && pooledObject->IsValidLowLevel())
		{
			bIsValid = true;
			if (ResetObjectEvent.IsBound())
			{
				ResetObjectEvent.Broadcast(pooledObject);
			}
		}

		return pooledObject;
	}

	return nullptr;
}