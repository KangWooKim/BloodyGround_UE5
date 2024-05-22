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
    // @param HealthPercentage 현재 체력을 백분율로 나타낸 값
    UFUNCTION(BlueprintCallable, Category = "HUD")
        void UpdateHealthBar(float HealthPercentage);

    // 캐릭터의 탄약 정보를 업데이트하는 함수
    // @param AmmoInMagazine 현재 탄창에 남은 탄약 수
    // @param TotalAmmo 전체 남은 탄약 수
    UFUNCTION(BlueprintCallable, Category = "HUD")
        void UpdateAmmoCount(int32 AmmoInMagazine, int32 TotalAmmo);

    // 리스폰 텍스트를 화면에 표시하는 함수
    UFUNCTION(BlueprintCallable, Category = "HUD")
        void SetRespawnText();

    // 화면에서 리스폰 텍스트를 제거하는 함수
    UFUNCTION(BlueprintCallable, Category = "HUD")
        void DeleteRespawnText();

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
};
