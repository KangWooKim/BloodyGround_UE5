// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameHUD.h"
#include "BloodyGround/HUD/InGameWidget.h"

void AInGameHUD::UpdateHealth(float HealthPercentage)
{
	InGameWidget->UpdateHealthBar(HealthPercentage);
}

void AInGameHUD::UpdateAmmo(int32 AmmoInMagazine, int32 TotalAmmo)
{
	InGameWidget->UpdateAmmoCount(AmmoInMagazine, TotalAmmo);
}

void AInGameHUD::SetRespawnText()
{
    InGameWidget->SetRespawnText();
}

void AInGameHUD::DeleteRespawnText()
{
    InGameWidget->DeleteRespawnText();
}

void AInGameHUD::BeginPlay()
{
	Super::BeginPlay();

    if (GetWorld()) {
        APlayerController* Controller = GetWorld()->GetFirstPlayerController();

        if (Controller && InGameWidgetClass) {
            InGameWidget = CreateWidget<UInGameWidget>(Controller, InGameWidgetClass);
            InGameWidget->AddToViewport();
            UpdateAmmo(10, 50);
        }
    }
}
