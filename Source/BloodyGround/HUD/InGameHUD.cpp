// Fill out your copyright notice in the Description page of Project Settings.

#include "InGameHUD.h"
#include "BloodyGround/HUD/InGameWidget.h"

// ĳ������ ü�� ������Ʈ �Լ�
void AInGameHUD::UpdateHealth(float HealthPercentage)
{
    // InGameWidget�� ü�� �� ������Ʈ �Լ� ȣ��
    InGameWidget->UpdateHealthBar(HealthPercentage);
}

// ĳ������ ź�� ���� ������Ʈ �Լ�
void AInGameHUD::UpdateAmmo(int32 AmmoInMagazine, int32 TotalAmmo)
{
    // InGameWidget�� ź�� ī��Ʈ ������Ʈ �Լ� ȣ��
    InGameWidget->UpdateAmmoCount(AmmoInMagazine, TotalAmmo);
}

// ������ �ؽ�Ʈ ���� �Լ�
void AInGameHUD::SetRespawnText()
{
    // InGameWidget�� ������ �ؽ�Ʈ ���� �Լ� ȣ��
    InGameWidget->SetRespawnText();
}

// ������ �ؽ�Ʈ ���� �Լ�
void AInGameHUD::DeleteRespawnText()
{
    // InGameWidget�� ������ �ؽ�Ʈ ���� �Լ� ȣ��
    InGameWidget->DeleteRespawnText();
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
