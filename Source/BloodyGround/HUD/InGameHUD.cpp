#include "InGameHUD.h"
#include "BloodyGround/HUD/InGameWidget.h"

// ĳ������ ü�� ������Ʈ �Լ�
void AInGameHUD::UpdateHealth(float HealthPercentage)
{
    if (InGameWidget)
    {
        // InGameWidget�� ü�� �� ������Ʈ �Լ� ȣ��
        InGameWidget->UpdateHealthBar(HealthPercentage);
    }
}

// ĳ������ ź�� ���� ������Ʈ �Լ�
void AInGameHUD::UpdateAmmo(int32 AmmoInMagazine, int32 TotalAmmo)
{
    // InGameWidget�� ź�� ī��Ʈ ������Ʈ �Լ� ȣ��
    if (InGameWidget)
    {
        InGameWidget->UpdateAmmoCount(AmmoInMagazine, TotalAmmo);
    }
}

// ������ �ؽ�Ʈ ���� �Լ�
void AInGameHUD::SetRespawnText()
{
    if (InGameWidget)
    {
        InGameWidget->SetRespawnText();
    }
    // InGameWidget�� ������ �ؽ�Ʈ ���� �Լ� ȣ��
}

// ������ �ؽ�Ʈ ���� �Լ�
void AInGameHUD::DeleteRespawnText()
{
    if (InGameWidget)
    {
        InGameWidget->DeleteRespawnText();
    }
    // InGameWidget�� ������ �ؽ�Ʈ ���� �Լ� ȣ��
}

// ���� �ؽ�Ʈ ���� �Լ�
void AInGameHUD::SetWinnerText(const FString& WinnerName)
{
    if (InGameWidget)
    {
        InGameWidget->SetWinnerText(WinnerName);
    }
}

// HUD �ʱ�ȭ �Լ�
void AInGameHUD::BeginPlay()
{
    // �θ� Ŭ������ BeginPlay ȣ��
    Super::BeginPlay();

    if (GetWorld()) {
        // ù ��° �÷��̾� ��Ʈ�ѷ� ��������
        APlayerController* Controller = GetWorld()->GetFirstPlayerController();

        // �÷��̾� ��Ʈ�ѷ��� InGameWidgetClass�� ��ȿ�� ���
        if (Controller && InGameWidgetClass) {
            // InGameWidget �ν��Ͻ� ����
            InGameWidget = CreateWidget<UInGameWidget>(Controller, InGameWidgetClass);
            // InGameWidget�� ����Ʈ�� �߰�
            InGameWidget->AddToViewport();
            // �ʱ� ź�� ���� ������Ʈ
            UpdateAmmo(10, 50);
        }
    }
}
