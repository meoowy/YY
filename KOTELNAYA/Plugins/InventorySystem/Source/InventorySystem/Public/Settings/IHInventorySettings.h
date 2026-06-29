// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Engine/DeveloperSettings.h>
#include "IHInventorySettings.generated.h"

/**
 * Global settings for the Inventory System Plugin.
 * Accessible via Project Settings.
 */
UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "IThub Inventory System"))
class INVENTORYSYSTEM_API UIHInventorySettings : public UDeveloperSettings {
	GENERATED_BODY()
	
public:
	// The master Data Table containing all item definitions
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "General")
	TSoftObjectPtr<UDataTable> ItemDataTable;
};