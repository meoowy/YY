// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/IHInventoryComponent.h"
#include "Data/IHItemTypes.h"
#include "Libraries/IHInventoryFunctionLibrary.h"
#include "Settings/IHInventorySettings.h"

UIHInventoryComponent::UIHInventoryComponent() {
	PrimaryComponentTick.bCanEverTick = false;
}

void UIHInventoryComponent::BeginPlay() {
	Super::BeginPlay();

	// Initialize fixed array with empty slots
	Slots.Empty();
	for (int32 i = 0; i < MaxSlots; i++) {
		Slots.Add(FIHInventorySlot());
	}
}

int32 UIHInventoryComponent::TryAddItem(FName ItemID, int32 Amount) {
	// 1. Basic Validation
	if (ItemID.IsNone() || Amount <= 0) {
		return Amount; // Nothing added, return full amount back
	}

	// 2. Fetch static definition to know MaxStackSize
	FIHItemDefinition ItemDef;
	if (!UIHInventoryFunctionLibrary::GetItemDefinition(ItemID, ItemDef)) {
		// Item ID not found in the global DataTable -> cannot add
		return Amount;
	}

	int32 MaxStack = ItemDef.MaxStackSize;
	int32 AmountLeft = Amount;

	// 3. STACKING PASS: Try to fill existing stacks of the same item first
	if (MaxStack > 1) {
		for (FIHInventorySlot& Slot : Slots) {
			// Check if slot has the same item and is not full
			if (Slot.IsValid() && Slot.GetItemID() == ItemID) {
				if (Slot.Quantity < MaxStack) {
					int32 SpaceInStack = MaxStack - Slot.Quantity;
					int32 AddCount = FMath::Min(SpaceInStack, AmountLeft);

					Slot.Quantity += AddCount;
					AmountLeft -= AddCount;

					if (AmountLeft <= 0) {
						break; // All items added
					}
				}
			}
		}
	}

	// 4. EMPTY SLOT PASS: Put remaining items into new empty slots
	if (AmountLeft > 0) {
		// We need the DataTable asset to set the reference correctly
		const UIHInventorySettings* Settings = GetDefault<UIHInventorySettings>();
		UDataTable* Table = Settings ? Settings->ItemDataTable.LoadSynchronous() : nullptr;

		if (Table) {
			for (FIHInventorySlot& Slot : Slots) {
				if (!Slot.IsValid()) // Found an empty slot
				{
					int32 AddCount = FMath::Min(MaxStack, AmountLeft);

					// Initialize the slot
					Slot.ItemReference.DataTable = Table;
					Slot.ItemReference.RowName = ItemID;
					Slot.Quantity = AddCount;

					AmountLeft -= AddCount;

					if (AmountLeft <= 0) {
						break; // All items added
					}
				}
			}
		}
	}

	// 5. Notifications & Events
	int32 AddedAmount = Amount - AmountLeft;

	if (AddedAmount > 0) {
		// Update UI Grid
		if (OnInventoryUpdated.IsBound()) {
			OnInventoryUpdated.Broadcast();
		}

		// Show Popup "Added 5x Apple"
		if (OnItemAdded.IsBound()) {
			OnItemAdded.Broadcast(ItemID, AddedAmount);
		}
	}
	else {
		// If absolutely nothing was added (Inventory Full)
		if (OnItemAddFailed.IsBound()) {
			OnItemAddFailed.Broadcast();
		}
	}

	// 6. Return what is left (The Remainder)
	// 0 = Success (All added), >0 = Inventory Full (Something remains)
	return AmountLeft;
}

bool UIHInventoryComponent::RemoveItem(FName ItemID, int32 AmountToRemove) {
	if (AmountToRemove <= 0) return false;

	int32 RemainingToRemove = AmountToRemove;

	// Iterate backwards to safely remove from multiple stacks if needed
	for (int32 i = Slots.Num() - 1; i >= 0; i--) {
		if (Slots[i].IsValid() && Slots[i].GetItemID() == ItemID) {
			if (Slots[i].Quantity > RemainingToRemove) {
				// Partial removal from stack
				Slots[i].Quantity -= RemainingToRemove;
				RemainingToRemove = 0;
				break;
			}
			else {
				// Consume entire stack
				RemainingToRemove -= Slots[i].Quantity;
				Slots[i].Clear();
			}
		}
	}

	if (OnInventoryUpdated.IsBound()) {
		OnInventoryUpdated.Broadcast();
	}

	// Return true if we successfully removed the full requested amount
	return RemainingToRemove == 0;
}

bool UIHInventoryComponent::TransferItem(int32 FromIndex, UIHInventoryComponent* TargetComponent, int32 ToIndex) {
	// Validation
	if (!TargetComponent) return false;
	if (!Slots.IsValidIndex(FromIndex) || !TargetComponent->Slots.IsValidIndex(ToIndex)) return false;

	FIHInventorySlot& SourceSlot = Slots[FromIndex];
	FIHInventorySlot& TargetSlot = TargetComponent->Slots[ToIndex];

	if (!SourceSlot.IsValid()) return false; // Nothing to move

	// Logic 1: Try to Merge/Stack if IDs match
	if (TargetSlot.IsValid() && TargetSlot.GetItemID() == SourceSlot.GetItemID()) {
		if (FIHItemDefinition ItemDef; UIHInventoryFunctionLibrary::GetItemDefinition(SourceSlot.GetItemID(), ItemDef)) {
			const int32 MaxStack = ItemDef.MaxStackSize;
			const int32 SpaceInTarget = MaxStack - TargetSlot.Quantity;

			if (SpaceInTarget > 0) {
				const int32 AmountToMove = FMath::Min(SpaceInTarget, SourceSlot.Quantity);

				TargetSlot.Quantity += AmountToMove;
				SourceSlot.Quantity -= AmountToMove;

				if (SourceSlot.Quantity <= 0) {
					SourceSlot.Clear();
				}

				// Notify both components
				if (OnInventoryUpdated.IsBound()) OnInventoryUpdated.Broadcast();
				if (TargetComponent != this && TargetComponent->OnInventoryUpdated.IsBound()) {
					TargetComponent->OnInventoryUpdated.Broadcast();
				}
				return true;
			}
		}
	}

	// Logic 2: Swap (or Move to empty)
	// Simply swap the structs data
	FIHInventorySlot TempSlot = SourceSlot;
	SourceSlot = TargetSlot;
	TargetSlot = TempSlot;

	// Notify both components
	if (OnInventoryUpdated.IsBound()) OnInventoryUpdated.Broadcast();
	if (TargetComponent != this && TargetComponent->OnInventoryUpdated.IsBound()) {
		TargetComponent->OnInventoryUpdated.Broadcast();
	}

	return true;
}

bool UIHInventoryComponent::HasItem(FName ItemID, int32 QuantityNeeded) const {
	int32 Count = 0;
	for (const FIHInventorySlot& Slot : Slots) {
		if (Slot.IsValid() && Slot.GetItemID() == ItemID) {
			Count += Slot.Quantity;
		}
	}
	return Count >= QuantityNeeded;
}
