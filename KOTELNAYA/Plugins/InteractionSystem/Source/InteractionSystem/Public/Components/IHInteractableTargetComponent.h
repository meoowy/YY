// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Components/SceneComponent.h>
#include "IHInteractableTargetComponent.generated.h"

class UWidgetComponent;

/**
 * Helper component for interactable objects.
 * Manages the visibility of a linked WidgetComponent and Mesh Outline.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class INTERACTIONSYSTEM_API UIHInteractableTargetComponent : public USceneComponent {
	GENERATED_BODY()

public:    
	UIHInteractableTargetComponent();

	// Default Text to display if Interface doesn't override it
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction | UI")
	FText DefaultInteractionText;

	// If true, automatically tries to find a child WidgetComponent to manage.
	UPROPERTY(EditAnywhere, Category = "Interaction | UI")
	bool bAutoFindWidget = true;

	// Reference to the widget component that displays the prompt
	UPROPERTY(BlueprintReadWrite, Category = "Interaction | UI")
	UWidgetComponent* PromptWidget;

	// --- Visuals Settings ---

	// Should we enable Custom Depth (Outline) on the owner's meshes when focused?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction | Visuals")
	bool bEnableOutline = true;

	// The Stencil ID to set (used by Post Process Materials to determine outline color)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction | Visuals")
	int32 OutlineStencilValue = 250;

	// --- Functions ---

	// Shows/Hides the prompt widget and Toggles Outline
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SetIsFocus(bool bInFocus);
    
	// Forces the widget to update its text from the Owner's interface or Default property.
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void RefreshWidgetText();

protected:
	virtual void BeginPlay() override;
};