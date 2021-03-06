// © 2016 - 2017 Daniel Bortfeld

#pragma once

#include "GameFramework/Actor.h"
#include "ResourcePopup.generated.h"

UCLASS()
class ETOS_API AResourcePopup : public AActor
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	class UWidgetComponent* Widget;

public:

	// Speed per Second in Z Direction
	UPROPERTY(EditAnywhere)
		float UpSpeed = 10.f;

private:

	UPROPERTY()
		APlayerCameraManager* PlayerCameraManager;

public:

	// Sets default values for this actor's properties
	AResourcePopup();

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Popup", meta = (DisplayName = "Update Widget"))
		void BPEvent_UpdateWidget(UTexture2D* Icon, FText const& Text = FText::FromName(TEXT("+1")), FSlateColor const& TextColor = FSlateColor(FLinearColor::Green));
};
