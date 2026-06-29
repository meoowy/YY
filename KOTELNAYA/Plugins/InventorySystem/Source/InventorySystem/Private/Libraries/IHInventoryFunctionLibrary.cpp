// Fill out your copyright notice in the Description page of Project Settings.


#include "Libraries/IHInventoryFunctionLibrary.h"
#include "Settings/IHInventorySettings.h"
#include "Data/IHItemTypes.h"

bool UIHInventoryFunctionLibrary::GetItemDefinition(const FName ItemID, FIHItemDefinition& OutDef) {
	if (ItemID.IsNone()) {
		return false;
	}

	// Access the global project settings
	const UIHInventorySettings* Settings = GetDefault<UIHInventorySettings>();
	if (!Settings) {
		return false;
	}

	// Ensure the pointer is valid. Note: LoadSynchronous is safe here for asset data.
	const UDataTable* Table = Settings->ItemDataTable.LoadSynchronous();
	if (!Table) {
		return false;
	}

	// Attempt to find the row
	const FString ContextString;

	if (const FIHItemDefinition* Row = Table->FindRow<FIHItemDefinition>(ItemID, ContextString)) {
		OutDef = *Row;
		return true;
	}

	return false;
}
