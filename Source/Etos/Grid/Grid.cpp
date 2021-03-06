// © 2016 - 2017 Daniel Bortfeld

#include "Etos.h"
#include "Grid.h"


// Sets default values
AGrid::AGrid()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	GridMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>("Grid Mesh");
	RootComponent = GridMesh;
	GridMesh->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel2); // Floor
	GridMesh->SetMobility(EComponentMobility::Static);
	GridMesh->SetCanEverAffectNavigation(false);
}

// Called when the game starts or when spawned
void AGrid::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AGrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGrid::OnConstruction(const FTransform & Transform)
{
	if (bConstruct)
	{
		GridMesh->ClearInstances();
		for (int32 i = 0; i < Width * Height; i++)
		{
			float X = i / Height * TileSize;
			float Y = i % Height * TileSize;
			GridMesh->AddInstance(FTransform(FVector(X, Y, 0)));
		}
	}
}

//#if WITH_EDITOR
//void AGrid::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
//{
//	//if (bConstruct)
//	//{
//	//	FName PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
//
//	//	if ((PropertyName == GET_MEMBER_NAME_CHECKED(AGrid, Width))
//	//	||  (PropertyName == GET_MEMBER_NAME_CHECKED(AGrid, Height))
//	//	||  (PropertyName == GET_MEMBER_NAME_CHECKED(AGrid, TileSize))
//	//	||  (PropertyName == GET_MEMBER_NAME_CHECKED(AGrid, bConstruct)))
//	//	{
//	//		GridMesh->ClearInstances();
//	//		for (int32 i = 0; i < Width * Height; i++)
//	//		{
//	//			float X = i / Height * TileSize;
//	//			float Y = i % Height * TileSize;
//	//			GridMesh->AddInstance(FTransform(FVector(X, Y, 0)));
//	//		}
//	//	}
//	//}
//}
//#endif

