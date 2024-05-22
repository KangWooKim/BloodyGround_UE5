// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "InGameHUD.generated.h"

class UInGameWidget;

/**
 * AInGameHUD 클래스는 게임 중 UI를 관리하는 HUD 클래스입니다.
 */
UCLASS()
class BLOODYGROUND_API AInGameHUD : public AHUD
{
    GENERATED_BODY()

public:

    // 게임 위젯 클래스 참조를 위한 변수
    UPROPERTY(EditAnywhere)
        TSubclassOf<class UUserWidget> InGameWidgetClass;

    // 캐릭터의 체력을 업데이트하는 함수
    // @param HealthPercentage 캐릭터의 현재 체력을 백분율로 나타낸 값
    UFUNCTION()
        void UpdateHealth(float HealthPercentage);

    // 캐릭터의 탄약 정보를 업데이트하는 함수
    // @param AmmoInMagazine 현재 탄창에 남은 탄약 수
    // @param TotalAmmo 전체 남은 탄약 수
    UFUNCTION()
        void UpdateAmmo(int32 AmmoInMagazine, int32 TotalAmmo);

    // 리스폰 텍스트를 화면에 표시하는 함수
    UFUNCTION()
        void SetRespawnText();

    // 화면에서 리스폰 텍스트를 제거하는 함수
    UFUNCTION()
        void DeleteRespawnText();

protected:

    // HUD가 처음 생성될 때 호출되는 함수
    virtual void BeginPlay() override;

private:

    // InGameWidget 인스턴스에 대한 포인터
    UPROPERTY()
        class UInGameWidget* InGameWidget;
};
