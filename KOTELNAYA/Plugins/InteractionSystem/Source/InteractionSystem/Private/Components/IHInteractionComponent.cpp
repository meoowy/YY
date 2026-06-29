// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/IHInteractionComponent.h"

#include <Components/SphereComponent.h>

#include "Interfaces/IHInteractable.h"

UIHInteractionComponent::UIHInteractionComponent() {
	PrimaryComponentTick.bCanEverTick = false;
}

void UIHInteractionComponent::BeginPlay() {
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	if (!Owner) return;

	// Create detection sphere dynamically to make setup easier for students
	InteractionVolume = NewObject<USphereComponent>(Owner, TEXT("InteractionVolume"));
	if (InteractionVolume) {
		InteractionVolume->RegisterComponent();
		InteractionVolume->AttachToComponent(Owner->GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
		InteractionVolume->SetSphereRadius(InteractionRadius);

		// Setup collision profiles
		InteractionVolume->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

		// Bind events
		InteractionVolume->OnComponentBeginOverlap.AddDynamic(this, &UIHInteractionComponent::OnInteractionVolumeOverlapBegin);
		InteractionVolume->OnComponentEndOverlap.AddDynamic(this, &UIHInteractionComponent::OnInteractionVolumeOverlapEnd);
	}
}

void UIHInteractionComponent::PerformInteraction() {
	if (InteractablePool.IsValidIndex(CurrentFocusIndex)) {
		AActor* Target = InteractablePool[CurrentFocusIndex];
		if (IsValid(Target) && Target->Implements<UIHInteractable>()) {
			IIHInteractable::Execute_Interact(Target, Cast<APawn>(GetOwner()));
		}
	}
}

void UIHInteractionComponent::CycleInteractable(int32 Direction) {
	if (InteractablePool.Num() <= 1) return;

	int32 NewIndex = CurrentFocusIndex + Direction;

	// Wrap around logic
	if (NewIndex >= InteractablePool.Num()) {
		NewIndex = 0;
	}
	else if (NewIndex < 0) {
		NewIndex = InteractablePool.Num() - 1;
	}

	if (NewIndex != CurrentFocusIndex) {
		// Unfocus old
		if (InteractablePool.IsValidIndex(CurrentFocusIndex)) {
			AActor* OldActor = InteractablePool[CurrentFocusIndex];
			if (IsValid(OldActor) && OldActor->Implements<UIHInteractable>()) {
				IIHInteractable::Execute_OnFocusLost(OldActor);
			}
		}

		CurrentFocusIndex = NewIndex;

		// Focus new
		SetFocusToActor(InteractablePool[CurrentFocusIndex]);
	}
}

void UIHInteractionComponent::OnInteractionVolumeOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (!OtherActor || OtherActor == GetOwner()) return;

	if (OtherActor->Implements<UIHInteractable>()) {
		InteractablePool.AddUnique(OtherActor);

		// If this is the only item (or first found), focus it immediately
		if (InteractablePool.Num() == 1) {
			CurrentFocusIndex = 0;
			SetFocusToActor(OtherActor);
		}
	}
}

void UIHInteractionComponent::OnInteractionVolumeOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	if (!OtherActor) return;

	if (InteractablePool.Contains(OtherActor)) {
		const bool bWasFocused = (InteractablePool.IsValidIndex(CurrentFocusIndex) && InteractablePool[CurrentFocusIndex] == OtherActor);

		// Remove focus from the leaving actor if it was active
		if (bWasFocused && IsValid(OtherActor) && OtherActor->Implements<UIHInteractable>()) {
			IIHInteractable::Execute_OnFocusLost(OtherActor);
		}

		InteractablePool.Remove(OtherActor);

		UpdateFocus();
	}
}

void UIHInteractionComponent::UpdateFocus() {
	if (InteractablePool.Num() == 0) {
		CurrentFocusIndex = -1;
		return;
	}

	// Safety check: if index is out of bounds, reset to 0
	if (!InteractablePool.IsValidIndex(CurrentFocusIndex)) {
		CurrentFocusIndex = 0;
	}

	SetFocusToActor(InteractablePool[CurrentFocusIndex]);
}

void UIHInteractionComponent::SetFocusToActor(AActor* NewFocusActor) {
	if (IsValid(NewFocusActor) && NewFocusActor->Implements<UIHInteractable>()) {
		IIHInteractable::Execute_OnFocusReceived(NewFocusActor);
	}
}
