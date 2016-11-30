// © 2016 - 2017 Daniel Bortfeld

#include "Etos.h"
#include "ObjectPool.h"

// Sets default values
FObjectPool::FObjectPool()
{
	pooledObjects.Reserve(minPooledObjects);
}

int32 FObjectPool::GetMinPooledObjectsAmount()
{
	return minPooledObjects;
}

void FObjectPool::SetMinPooledObjectsAmount(int32 minPooledObjects)
{
	this->minPooledObjects = FMath::Clamp(minPooledObjects, 0, maxPooledObjects);
}

int32 FObjectPool::GetMaxPooledObjectsAmount()
{
	return maxPooledObjects;
}

void FObjectPool::SetMaxPooledObjectsAmount(int32 maxPooledObjects)
{
	this->maxPooledObjects = FMath::Max(minPooledObjects, maxPooledObjects);
}
