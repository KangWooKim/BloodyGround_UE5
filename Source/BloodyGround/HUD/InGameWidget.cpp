#include "InGameWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UInGameWidget::UpdateHealthBar(float HealthPercentage)
{
    if (HealthBar)
    {
        HealthBar->SetPercent(HealthPercentage);
    }
}

void UInGameWidget::UpdateAmmoCount(int32 AmmoInMagazine, int32 TotalAmmo)
{
    if (AmmoText)
    {
        FString AmmoDisplay = FString::Printf(TEXT("%d / %d"), AmmoInMagazine, TotalAmmo);
        AmmoText->SetText(FText::FromString(AmmoDisplay));
    }
}

void UInGameWidget::SetRespawnText()
{
    if (RespawnText)
    {
        FString RespawnTextDisplay = FString::Printf(TEXT("You're dead. Wait for the Respawn....."));
        RespawnText->SetText(FText::FromString(RespawnTextDisplay));
    }
}

void UInGameWidget::DeleteRespawnText()
{
    if (RespawnText)
    {
        FString RespawnTextDisplay = FString::Printf(TEXT(""));
        RespawnText->SetText(FText::FromString(RespawnTextDisplay));
    }
}
