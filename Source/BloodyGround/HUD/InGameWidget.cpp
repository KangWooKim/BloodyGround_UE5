#include "InGameWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

// ü�� �ٸ� ������Ʈ�ϴ� �Լ�
void UInGameWidget::UpdateHealthBar(float HealthPercentage)
{
    if (HealthBar)
    {
        HealthBar->SetPercent(HealthPercentage);
    }
}

// ź�� ������ ������Ʈ�ϴ� �Լ�
void UInGameWidget::UpdateAmmoCount(int32 AmmoInMagazine, int32 TotalAmmo)
{
    if (AmmoText)
    {
        FString AmmoDisplay = FString::Printf(TEXT("%d / %d"), AmmoInMagazine, TotalAmmo);
        AmmoText->SetText(FText::FromString(AmmoDisplay));
    }
}

// ������ �ؽ�Ʈ�� �����ϴ� �Լ�
void UInGameWidget::SetRespawnText()
{
    if (RespawnText)
    {
        FString RespawnTextDisplay = FString::Printf(TEXT("You're dead. Wait for the Respawn....."));
        RespawnText->SetText(FText::FromString(RespawnTextDisplay));
    }
}

// ������ �ؽ�Ʈ�� �����ϴ� �Լ�
void UInGameWidget::DeleteRespawnText()
{
    if (RespawnText)
    {
        RespawnText->SetText(FText::FromString(TEXT("")));
    }
}

// ���� �ؽ�Ʈ�� �����ϴ� �Լ�
void UInGameWidget::SetWinnerText(const FString& WinnerName)
{
    if (WinnerText)
    {
        FString WinnerTextDisplay = FString::Printf(TEXT("Stage Cleared by %s!"), *WinnerName);
        WinnerText->SetText(FText::FromString(WinnerTextDisplay));
    }
}
