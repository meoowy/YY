// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <UObject/Interface.h>
#include "IHInteractable.generated.h"


UINTERFACE(MinimalAPI)
class UIHInteractable : public UInterface {
	GENERATED_BODY()
};

/**
 * Interface implemented by any Actor that can be interacted with (Chests, Doors, NPCs).
 */
class INTERACTIONSYSTEM_API IIHInteractable {
	GENERATED_BODY()

public:
	/** * Main interaction trigger.
	 * @param InstigatorPawn The pawn initiating the interaction.
	 * @return True if interaction was successful.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	bool Interact(APawn* InstigatorPawn);

	/**
	 * Called when the player focuses on this object (e.g. looks at it or enters range).
	 * Used to show widgets or highlights.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
	void OnFocusReceived();

	/**
	 * Called when the player looks away or leaves range.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
	void OnFocusLost();

	/**
	 * Returns the text to display on the UI (e.g. "Open", "Talk").
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
	FText GetInteractionActionText() const;
};
