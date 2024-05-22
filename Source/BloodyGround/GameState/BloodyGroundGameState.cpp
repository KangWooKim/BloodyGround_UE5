#include "BloodyGroundGameState.h"
#include "Net/UnrealNetwork.h"
#include "Engine/DirectionalLight.h"
#include "Components/LightComponent.h"
#include "Kismet/GameplayStatics.h"

ABloodyGroundGameState::ABloodyGroundGameState()
{
    Sunlight = nullptr;
}

void ABloodyGroundGameState::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority())
    {
        // ��/�� �ֱ� Ÿ�̸� ���� (��: 60�ʸ��� ��/�� ����)
        GetWorldTimerManager().SetTimer(DayNightCycleTimerHandle, this, &ABloodyGroundGameState::ToggleDayNightCycle, 60.0f, true);
    }
}

void ABloodyGroundGameState::SetDayNightCycle(EDayNightCycle NewCycle)
{
    if (HasAuthority())
    {
        DayNightCycle = NewCycle;
        OnRep_DayNightChanged();
    }
}

void ABloodyGroundGameState::OnRep_DayNightChanged()
{
    bool bIsDay = DayNightCycle == EDayNightCycle::Day;
    AdjustSunlight(bIsDay);
}

void ABloodyGroundGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // ��/�� ���� ���� ����
    DOREPLIFETIME(ABloodyGroundGameState, DayNightCycle);
}

void ABloodyGroundGameState::AdjustSunlight(bool bIsDay)
{
    // ��ǥ ���� ���� ���� (���̸� ��� ���̸� ��Ӱ�)
    TargetSunlightIntensity = bIsDay ? 3.0f : 0.1f;

    // ���� ���� ���� ��������
    if (!Sunlight)
    {
        Sunlight = Cast<ADirectionalLight>(UGameplayStatics::GetActorOfClass(GetWorld(), ADirectionalLight::StaticClass()));
    }

    if (Sunlight)
    {
        CurrentSunlightIntensity = Sunlight->GetLightComponent()->Intensity;

        // 10�� ���� ������ ����
        GetWorldTimerManager().SetTimer(SunlightAdjustmentTimerHandle, this, &ABloodyGroundGameState::UpdateSunlight, 0.1f, true);
    }
}

void ABloodyGroundGameState::UpdateSunlight()
{
    if (Sunlight)
    {
        // 10�� ���� ������ ��ǥ ���� ����� ����
        float DeltaIntensity = (TargetSunlightIntensity - CurrentSunlightIntensity) / 100.0f;
        CurrentSunlightIntensity += DeltaIntensity;
        Sunlight->GetLightComponent()->SetIntensity(CurrentSunlightIntensity);

        // ��ǥ ���� ���⿡ �����ϸ� Ÿ�̸� ����
        if (FMath::IsNearlyEqual(CurrentSunlightIntensity, TargetSunlightIntensity, 0.01f))
        {
            GetWorldTimerManager().ClearTimer(SunlightAdjustmentTimerHandle);
        }
    }
}

void ABloodyGroundGameState::ToggleDayNightCycle()
{
    // ���� ���¸� ����Ͽ� ��/�� ����
    EDayNightCycle NewCycle = (DayNightCycle == EDayNightCycle::Day) ? EDayNightCycle::Night : EDayNightCycle::Day;
    SetDayNightCycle(NewCycle);
}