#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "BloodyGroundGameState.generated.h"

UENUM(BlueprintType)
enum class EDayNightCycle : uint8
{
    Day UMETA(DisplayName = "Day"),
    Night UMETA(DisplayName = "Night")
};

UCLASS()
class BLOODYGROUND_API ABloodyGroundGameState : public AGameState
{
    GENERATED_BODY()

public:
    ABloodyGroundGameState();

    virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

    // 블루프린트에서 접근 및 수정 가능하도록 설정
    UPROPERTY(ReplicatedUsing = OnRep_DayNightChanged, BlueprintReadWrite, EditDefaultsOnly, Category = "DayNight")
        EDayNightCycle DayNightCycle;

    // 낮/밤 상태 변경시 호출되는 함수
    UFUNCTION()
        void OnRep_DayNightChanged();

    // 낮/밤 상태를 설정하는 함수
    void SetDayNightCycle(EDayNightCycle NewCycle);

protected:
    virtual void BeginPlay() override;

private:
    // 햇빛을 조정하는 함수
    void AdjustSunlight(bool bIsDay);

    // 조명 변경을 위한 타이머 핸들
    FTimerHandle SunlightAdjustmentTimerHandle;

    // 낮/밤 변경 주기 타이머 핸들
    FTimerHandle DayNightCycleTimerHandle;

    // 현재의 광원 세기
    float CurrentSunlightIntensity;

    // 목표 광원 세기
    float TargetSunlightIntensity;

    // 광원 세기를 서서히 변경하는 함수
    void UpdateSunlight();

    // 조명 참조
    UPROPERTY()
     class ADirectionalLight* Sunlight;

    // 낮/밤 주기를 변경하는 함수
    void ToggleDayNightCycle();
};