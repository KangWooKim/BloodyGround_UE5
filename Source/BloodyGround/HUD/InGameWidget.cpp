#include "InGameWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

// ü�� �ٸ� ������Ʈ�ϴ� �Լ�
// @param HealthPercentage ���� ü���� ������� ��Ÿ�� ��
void UInGameWidget::UpdateHealthBar(float HealthPercentage)
{
    // HealthBar�� ��ȿ�� ��� ü�� ���� ����
    if (HealthBar)
    {
        HealthBar->SetPercent(HealthPercentage);
    }
}

// ź�� ������ ������Ʈ�ϴ� �Լ�
// @param AmmoInMagazine ���� źâ�� ���� ź�� ��
// @param TotalAmmo ��ü ���� ź�� ��
void UInGameWidget::UpdateAmmoCount(int32 AmmoInMagazine, int32 TotalAmmo)
{
    // AmmoText�� ��ȿ�� ��� �ؽ�Ʈ�� ������Ʈ
    if (AmmoText)
    {
        FString AmmoDisplay = FString::Printf(TEXT("%d / %d"), AmmoInMagazine, TotalAmmo);
        AmmoText->SetText(FText::FromString(AmmoDisplay));
    }
}

// ������ �ؽ�Ʈ�� �����ϴ� �Լ�
void UInGameWidget::SetRespawnText()
{
    // RespawnText�� ��ȿ�� ��� ������ �޽����� ����
    if (RespawnText)
    {
        FString RespawnTextDisplay = FString::Printf(TEXT("You're dead. Wait for the Respawn....."));
        RespawnText->SetText(FText::FromString(RespawnTextDisplay));
    }
}

// ������ �ؽ�Ʈ�� �����ϴ� �Լ�
void UInGameWidget::DeleteRespawnText()
{
    // RespawnText�� ��ȿ�� ��� �ؽ�Ʈ�� �� ���ڿ��� �����Ͽ� ����
    if (RespawnText)
    {
        FString RespawnTextDisplay = FString::Printf(TEXT(""));
        RespawnText->SetText(FText::FromString(RespawnTextDisplay));
    }
}
