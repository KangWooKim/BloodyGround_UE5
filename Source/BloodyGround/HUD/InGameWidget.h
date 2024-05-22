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
    // 캐릭터의 체력 바를 업데이트하는 함수
    UFUNCTION(BlueprintCallable, Category = "HUD")
        void UpdateHealthBar(float HealthPercentage);

    // 캐릭터의 탄약 정보를 업데이트하는 함수
    UFUNCTION(BlueprintCallable, Category = "HUD")
        void UpdateAmmoCount(int32 AmmoInMagazine, int32 TotalAmmo);

    // 리스폰 텍스트를 화면에 표시하는 함수
    UFUNCTION(BlueprintCallable, Category = "HUD")
        void SetRespawnText();

    // 화면에서 리스폰 텍스트를 제거하는 함수
    UFUNCTION(BlueprintCallable, Category = "HUD")
        void DeleteRespawnText();

    // 승자 텍스트를 설정하는 함수
    UFUNCTION(BlueprintCallable, Category = "HUD")
        void SetWinnerText(const FString& WinnerName);

protected:
    // 체력 바 UI 요소
    UPROPERTY(meta = (BindWidget))
        UProgressBar* HealthBar;

    // 탄약 수 텍스트 UI 요소
    UPROPERTY(meta = (BindWidget))
        UTextBlock* AmmoText;

    // 리스폰 텍스트 UI 요소
    UPROPERTY(meta = (BindWidget))
        UTextBlock* RespawnText;

    // 승자 텍스트 UI 요소
    UPROPERTY(meta = (BindWidget))
        UTextBlock* WinnerText;
};
