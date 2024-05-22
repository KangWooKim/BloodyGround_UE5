#include "InGameWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

// 체력 바를 업데이트하는 함수
void UInGameWidget::UpdateHealthBar(float HealthPercentage)
{
    if (HealthBar)
    {
        HealthBar->SetPercent(HealthPercentage);
    }
}

// 탄약 정보를 업데이트하는 함수
void UInGameWidget::UpdateAmmoCount(int32 AmmoInMagazine, int32 TotalAmmo)
{
    if (AmmoText)
    {
        FString AmmoDisplay = FString::Printf(TEXT("%d / %d"), AmmoInMagazine, TotalAmmo);
        AmmoText->SetText(FText::FromString(AmmoDisplay));
    }
}

// 리스폰 텍스트를 설정하는 함수
void UInGameWidget::SetRespawnText()
{
    if (RespawnText)
    {
        FString RespawnTextDisplay = FString::Printf(TEXT("You're dead. Wait for the Respawn....."));
        RespawnText->SetText(FText::FromString(RespawnTextDisplay));
    }
}

// 리스폰 텍스트를 제거하는 함수
void UInGameWidget::DeleteRespawnText()
{
    if (RespawnText)
    {
        RespawnText->SetText(FText::FromString(TEXT("")));
    }
}

// 승자 텍스트를 설정하는 함수
void UInGameWidget::SetWinnerText(const FString& WinnerName)
{
    if (WinnerText)
    {
        FString WinnerTextDisplay = FString::Printf(TEXT("Stage Cleared by %s!"), *WinnerName);
        WinnerText->SetText(FText::FromString(WinnerTextDisplay));
    }
}
