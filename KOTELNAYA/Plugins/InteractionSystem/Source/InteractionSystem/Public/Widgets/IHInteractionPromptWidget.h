// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Blueprint/UserWidget.h>
#include "IHInteractionPromptWidget.generated.h"

class UTextBlock;

/**
 * Base logic for the Interaction Widget (e.g. "Press E to Pick Up").
 */
UCLASS(Abstract)
class INTERACTIONSYSTEM_API UIHInteractionPromptWidget : public UUserWidget {
	GENERATED_BODY()
	
public:
	// The main action description (e.g. "Pick Up")
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* ActionText;

	// The input key to press (e.g. "E" or Gamepad "A")
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, OptionalWidget = true))
	UTextBlock* KeyText;

	// Updates the displayed text
	UFUNCTION(BlueprintCallable, Category = "Interaction UI")
	void SetPromptText(const FText& InText);

protected:
	virtual void NativeConstruct() override;
};
