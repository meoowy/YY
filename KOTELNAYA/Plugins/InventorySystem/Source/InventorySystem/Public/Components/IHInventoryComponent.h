// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Components/ActorComponent.h>
#include "IHInventoryComponent.generated.h"

struct FIHInventorySlot;
struct FIHInventoryItem;

// Delegate to notify UI about inventory changes (refresh slots)
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);
// Delegate to notify when an item is successfully added (for popup widget)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemAdded, FName, ItemID, int32, Amount);
// Delegate to notify when add failed due to full inventory (for "No Space" hint)
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemAddFailed);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class INVENTORYSYSTEM_API UIHInventoryComponent : public UActorComponent {
	GENERATED_BODY()

public:
	UIHInventoryComponent();
	
	UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
	FOnInventoryUpdated OnInventoryUpdated;

	UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
	FOnItemAdded OnItemAdded;

	UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
	FOnItemAddFailed OnItemAddFailed;

	/**
	 * Tries to add an item by RowName (ID).
	 * @param ItemID - The RowName of the item to add.
	 * @param Amount - The quantity to add.
	 * @return The number of items that COULD NOT be added (Remainder). 
	 * Returns 0 if all items were successfully added.
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 TryAddItem(FName ItemID, int32 Amount = 1);

	/**
	 * Removes item(s) by ID.
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItem(FName ItemID, int32 AmountToRemove = 1);

	/**
	 * Moves items from one slot index to another (possibly between components).
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool TransferItem(int32 FromIndex, UIHInventoryComponent* TargetComponent, int32 ToIndex);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool HasItem(FName ItemID, int32 QuantityNeeded = 1) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	const TArray<FIHInventorySlot>& GetSlots() { return Slots; };
	
protected:
	virtual void BeginPlay() override;

	// STATE: Fixed array of slots
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory | State")
	TArray<FIHInventorySlot> Slots;

	// Config: How many slots this component has (e.g., 5 for Belt, 20 for Chest)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory | Config")
	int32 MaxSlots = 5;
};
