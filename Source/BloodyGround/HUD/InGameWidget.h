#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameWidget.generated.h"

class UProgressBar;
class UTextBlock;

UCLASS()
class BLOODYGROUND_API UInGameWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // ĳ������ ü�� �ٸ� ������Ʈ�ϴ� �Լ�
    // @param HealthPercentage ���� ü���� ������� ��Ÿ�� ��
    UFUNCTION(BlueprintCallable, Category = "HUD")
        void UpdateHealthBar(float HealthPercentage);

    // ĳ������ ź�� ������ ������Ʈ�ϴ� �Լ�
    // @param AmmoInMagazine ���� źâ�� ���� ź�� ��
    // @param TotalAmmo ��ü ���� ź�� ��
    UFUNCTION(BlueprintCallable, Category = "HUD")
        void UpdateAmmoCount(int32 AmmoInMagazine, int32 TotalAmmo);

    // ������ �ؽ�Ʈ�� ȭ�鿡 ǥ���ϴ� �Լ�
    UFUNCTION(BlueprintCallable, Category = "HUD")
        void SetRespawnText();

    // ȭ�鿡�� ������ �ؽ�Ʈ�� �����ϴ� �Լ�
    UFUNCTION(BlueprintCallable, Category = "HUD")
        void DeleteRespawnText();

protected:
    // ü�� �� UI ���
    UPROPERTY(meta = (BindWidget))
        UProgressBar* HealthBar;

    // ź�� �� �ؽ�Ʈ UI ���
    UPROPERTY(meta = (BindWidget))
        UTextBlock* AmmoText;

    // ������ �ؽ�Ʈ UI ���
    UPROPERTY(meta = (BindWidget))
        UTextBlock* RespawnText;
};
