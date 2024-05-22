#include "InGameWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

// 체력 바를 업데이트하는 함수
// @param HealthPercentage 현재 체력을 백분율로 나타낸 값
void UInGameWidget::UpdateHealthBar(float HealthPercentage)
{
    // HealthBar가 유효한 경우 체력 값을 설정
    if (HealthBar)
    {
        HealthBar->SetPercent(HealthPercentage);
    }
}

// 탄약 정보를 업데이트하는 함수
// @param AmmoInMagazine 현재 탄창에 남은 탄약 수
// @param TotalAmmo 전체 남은 탄약 수
void UInGameWidget::UpdateAmmoCount(int32 AmmoInMagazine, int32 TotalAmmo)
{
    // AmmoText가 유효한 경우 텍스트를 업데이트
    if (AmmoText)
    {
        FString AmmoDisplay = FString::Printf(TEXT("%d / %d"), AmmoInMagazine, TotalAmmo);
        AmmoText->SetText(FText::FromString(AmmoDisplay));
    }
}

// 리스폰 텍스트를 설정하는 함수
void UInGameWidget::SetRespawnText()
{
    // RespawnText가 유효한 경우 리스폰 메시지를 설정
    if (RespawnText)
    {
        FString RespawnTextDisplay = FString::Printf(TEXT("You're dead. Wait for the Respawn....."));
        RespawnText->SetText(FText::FromString(RespawnTextDisplay));
    }
}

// 리스폰 텍스트를 제거하는 함수
void UInGameWidget::DeleteRespawnText()
{
    // RespawnText가 유효한 경우 텍스트를 빈 문자열로 설정하여 제거
    if (RespawnText)
    {
        FString RespawnTextDisplay = FString::Printf(TEXT(""));
        RespawnText->SetText(FText::FromString(RespawnTextDisplay));
    }
}
