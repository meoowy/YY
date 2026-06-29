// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Kismet/BlueprintFunctionLibrary.h>
#include "IHInventoryFunctionLibrary.generated.h"

struct FIHItemDefinition;
/**
 * Global static helper functions for the Inventory System.
 */
UCLASS()
class INVENTORYSYSTEM_API UIHInventoryFunctionLibrary : public UBlueprintFunctionLibrary {
	GENERATED_BODY()

public:
	/**
	 * Retrieves the static Item Definition from the Global Settings Data Table.
	 * @param ItemID - The RowName of the item.
	 * @param OutDef - The found definition.
	 * @return True if the item was found.
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory | Database")
	static bool GetItemDefinition(FName ItemID, FIHItemDefinition& OutDef);
};