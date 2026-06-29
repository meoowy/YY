#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "IHItemTypes.generated.h"

/**
 * Structure representing an item in the inventory.
 * Contains visual (2D and 3D) and logical data.
 */
USTRUCT(BlueprintType)
struct FIHItemDefinition : public FTableRowBase {
	GENERATED_BODY()

	// Unique identifier for logic (e.g., "RedKey", "Hammer")
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	FName ItemID;

	// Display name for UI
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	FText Name;

	// Short description for the popup widget
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	FText Description;

	// Icon to display in the UI slot (2D)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	UTexture2D* Icon;

	// Mesh representation for the world pickup or 3D inspection
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	UStaticMesh* Mesh;

	// Maximum amount of items allowed in one stack
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 MaxStackSize = 1;
};

/**
 * Represents the state of a specific slot.
 * Stored in the Inventory Component.
 */
USTRUCT(BlueprintType)
struct FIHInventorySlot {
	GENERATED_BODY()

	// Reference to the Item in the DataTable
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	FDataTableRowHandle ItemReference;

	// Current amount
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 Quantity = 0;

	// Helper to check if slot is logically valid
	bool IsValid() const {
		return !ItemReference.IsNull() && Quantity > 0;
	}

	// Helper to get the Row Name (ID) conveniently
	FName GetItemID() const {
		return ItemReference.RowName;
	}

	void Clear() {
		ItemReference.RowName = NAME_None;
		ItemReference.DataTable = nullptr;
		Quantity = 0;
	}
};
