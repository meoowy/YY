// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Components/ActorComponent.h>
#include "IHInteractionComponent.generated.h"

class USphereComponent;

/**
 * Manages proximity-based interaction for the Player.
 * Maintains a list of interactable actors in range and handles cycling/selection.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class INTERACTIONSYSTEM_API UIHInteractionComponent : public UActorComponent {
	GENERATED_BODY()

public:    
	UIHInteractionComponent();

	// --- Configuration ---
	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	float InteractionRadius = 200.0f;

	// --- Actions ---

	/**
	 * Attempts to interact with the currently focused object.
	 */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void PerformInteraction();

	/**
	 * Cycles through available interactables in range (useful if multiple items are close).
	 * @param Direction +1 for next, -1 for previous.
	 */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void CycleInteractable(int32 Direction);

protected:
	virtual void BeginPlay() override;
	
	// The sphere trigger for detection.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* InteractionVolume;

	// List of actors currently in range that implement the Interface.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	TArray<AActor*> InteractablePool;

	// Index of the currently focused actor in the pool.
	int32 CurrentFocusIndex = -1;

	// --- Internal Logic ---

	UFUNCTION()
	void OnInteractionVolumeOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnInteractionVolumeOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void UpdateFocus();
	void SetFocusToActor(AActor* NewFocusActor);
};