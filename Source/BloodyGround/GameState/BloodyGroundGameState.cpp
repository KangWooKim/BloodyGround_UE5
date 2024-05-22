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
        // 낮/밤 주기 타이머 설정 (예: 60초마다 낮/밤 변경)
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

    // 낮/밤 상태 변수 복제
    DOREPLIFETIME(ABloodyGroundGameState, DayNightCycle);
}

void ABloodyGroundGameState::AdjustSunlight(bool bIsDay)
{
    // 목표 광원 세기 설정 (낮이면 밝고 밤이면 어둡게)
    TargetSunlightIntensity = bIsDay ? 3.0f : 0.1f;

    // 현재 광원 세기 가져오기
    if (!Sunlight)
    {
        Sunlight = Cast<ADirectionalLight>(UGameplayStatics::GetActorOfClass(GetWorld(), ADirectionalLight::StaticClass()));
    }

    if (Sunlight)
    {
        CurrentSunlightIntensity = Sunlight->GetLightComponent()->Intensity;

        // 10초 동안 서서히 변경
        GetWorldTimerManager().SetTimer(SunlightAdjustmentTimerHandle, this, &ABloodyGroundGameState::UpdateSunlight, 0.1f, true);
    }
}

void ABloodyGroundGameState::UpdateSunlight()
{
    if (Sunlight)
    {
        // 10초 동안 서서히 목표 광원 세기로 변경
        float DeltaIntensity = (TargetSunlightIntensity - CurrentSunlightIntensity) / 100.0f;
        CurrentSunlightIntensity += DeltaIntensity;
        Sunlight->GetLightComponent()->SetIntensity(CurrentSunlightIntensity);

        // 목표 광원 세기에 도달하면 타이머 중지
        if (FMath::IsNearlyEqual(CurrentSunlightIntensity, TargetSunlightIntensity, 0.01f))
        {
            GetWorldTimerManager().ClearTimer(SunlightAdjustmentTimerHandle);
        }
    }
}

void ABloodyGroundGameState::ToggleDayNightCycle()
{
    // 현재 상태를 토글하여 낮/밤 변경
    EDayNightCycle NewCycle = (DayNightCycle == EDayNightCycle::Day) ? EDayNightCycle::Night : EDayNightCycle::Day;
    SetDayNightCycle(NewCycle);
}