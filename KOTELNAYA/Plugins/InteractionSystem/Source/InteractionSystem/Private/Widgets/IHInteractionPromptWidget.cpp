// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/IHInteractionPromptWidget.h"
#include <Components/TextBlock.h>

void UIHInteractionPromptWidget::SetPromptText(const FText& InText) {
	if (ActionText) {
		ActionText->SetText(InText);
	}
}

void UIHInteractionPromptWidget::NativeConstruct() {
	Super::NativeConstruct();
	// Logic for Key Icon (Keyboard/Gamepad) can be expanded here later
}
