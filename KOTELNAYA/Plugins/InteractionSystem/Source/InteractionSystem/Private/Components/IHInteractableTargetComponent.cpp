// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/IHInteractableTargetComponent.h"
#include <Components/WidgetComponent.h>
#include "Interfaces/IHInteractable.h"
#include "Widgets/IHInteractionPromptWidget.h"

UIHInteractableTargetComponent::UIHInteractableTargetComponent() {
	PrimaryComponentTick.bCanEverTick = false;
	DefaultInteractionText = NSLOCTEXT("Interaction", "DefaultAction", "Interact");
}

void UIHInteractableTargetComponent::BeginPlay() {
	Super::BeginPlay();

	// 1. Auto-Find Widget Component if not set
	if (!PromptWidget && bAutoFindWidget) {
		// Check children of this component
		TArray<USceneComponent*> Children;
		GetChildrenComponents(true, Children);

		for (USceneComponent* Child : Children) {
			if (UWidgetComponent* WidgetComp = Cast<UWidgetComponent>(Child)) {
				PromptWidget = WidgetComp;
				break;
			}
		}

		// Check Actor's components if not found in children
		if (!PromptWidget && GetOwner()) {
			PromptWidget = GetOwner()->FindComponentByClass<UWidgetComponent>();
		}
	}

	// 2. Initial Setup
	if (PromptWidget) {
		PromptWidget->SetVisibility(false);

		// Ensure widget is initialized
		if (!PromptWidget->GetWidget()) {
			PromptWidget->InitWidget();
		}

		RefreshWidgetText();
	}
}

void UIHInteractableTargetComponent::SetIsFocus(bool bInFocus) {
	// 1. Handle UI Widget
	if (PromptWidget) {
		PromptWidget->SetVisibility(bInFocus);
	}

	// 2. Handle Mesh Outline (Post Process)
	if (bEnableOutline) {
		if (AActor* Owner = GetOwner()) {
			// Get all mesh components (Static Mesh, Skeletal Mesh)
			TArray<UMeshComponent*> Meshes;
			Owner->GetComponents<UMeshComponent>(Meshes);

			for (UMeshComponent* Mesh : Meshes) {
				if (Mesh) {
					Mesh->SetRenderCustomDepth(bInFocus);
					if (bInFocus) {
						Mesh->SetCustomDepthStencilValue(OutlineStencilValue);
					}
				}
			}
		}
	}
}

void UIHInteractableTargetComponent::RefreshWidgetText() {
	if (!PromptWidget) return;

	if (UIHInteractionPromptWidget* InteractionWidget = Cast<UIHInteractionPromptWidget>(PromptWidget->GetWidget())) {
		FText DisplayText = DefaultInteractionText;

		// Try to pull dynamic text from the Owner via Interface
		if (const AActor* Owner = GetOwner()) {
			if (Owner->Implements<UIHInteractable>()) {
				DisplayText = IIHInteractable::Execute_GetInteractionActionText(Owner);
			}
		}

		InteractionWidget->SetPromptText(DisplayText);
	}
}