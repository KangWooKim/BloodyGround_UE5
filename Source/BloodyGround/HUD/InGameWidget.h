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
    UFUNCTION(BlueprintCallable, Category = "HUD")
        void UpdateHealthBar(float HealthPercentage);

    // ĳ������ ź�� ������ ������Ʈ�ϴ� �Լ�
    UFUNCTION(BlueprintCallable, Category = "HUD")
        void UpdateAmmoCount(int32 AmmoInMagazine, int32 TotalAmmo);

    // ������ �ؽ�Ʈ�� ȭ�鿡 ǥ���ϴ� �Լ�
    UFUNCTION(BlueprintCallable, Category = "HUD")
        void SetRespawnText();

    // ȭ�鿡�� ������ �ؽ�Ʈ�� �����ϴ� �Լ�
    UFUNCTION(BlueprintCallable, Category = "HUD")
        void DeleteRespawnText();

    // ���� �ؽ�Ʈ�� �����ϴ� �Լ�
    UFUNCTION(BlueprintCallable, Category = "HUD")
        void SetWinnerText(const FString& WinnerName);

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

    // ���� �ؽ�Ʈ UI ���
    UPROPERTY(meta = (BindWidget))
        UTextBlock* WinnerText;
};
