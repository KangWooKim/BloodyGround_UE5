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
    UFUNCTION(BlueprintCallable, Category = "HUD")
        void UpdateHealthBar(float HealthPercentage);

    UFUNCTION(BlueprintCallable, Category = "HUD")
        void UpdateAmmoCount(int32 AmmoInMagazine, int32 TotalAmmo);

    UFUNCTION(BlueprintCallable, Category = "HUD")
     void SetRespawnText();

    UFUNCTION(BlueprintCallable, Category = "HUD")
     void DeleteRespawnText();

protected:
    // 진행률 표시 막대 변수
    UPROPERTY(meta = (BindWidget))
        UProgressBar* HealthBar;

    // 탄약 수 표시 텍스트 변수
    UPROPERTY(meta = (BindWidget))
        UTextBlock* AmmoText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* RespawnText;
};
