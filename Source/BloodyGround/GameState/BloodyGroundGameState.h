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

    // �������Ʈ���� ���� �� ���� �����ϵ��� ����
    UPROPERTY(ReplicatedUsing = OnRep_DayNightChanged, BlueprintReadWrite, EditDefaultsOnly, Category = "DayNight")
        EDayNightCycle DayNightCycle;

    // ��/�� ���� ����� ȣ��Ǵ� �Լ�
    UFUNCTION()
        void OnRep_DayNightChanged();

    // ��/�� ���¸� �����ϴ� �Լ�
    void SetDayNightCycle(EDayNightCycle NewCycle);

protected:
    virtual void BeginPlay() override;

private:
    // �޺��� �����ϴ� �Լ�
    void AdjustSunlight(bool bIsDay);

    // ���� ������ ���� Ÿ�̸� �ڵ�
    FTimerHandle SunlightAdjustmentTimerHandle;

    // ��/�� ���� �ֱ� Ÿ�̸� �ڵ�
    FTimerHandle DayNightCycleTimerHandle;

    // ������ ���� ����
    float CurrentSunlightIntensity;

    // ��ǥ ���� ����
    float TargetSunlightIntensity;

    // ���� ���⸦ ������ �����ϴ� �Լ�
    void UpdateSunlight();

    // ���� ����
    UPROPERTY()
     class ADirectionalLight* Sunlight;

    // ��/�� �ֱ⸦ �����ϴ� �Լ�
    void ToggleDayNightCycle();
};