// Fill out your copyright notice in the Description page of Project Settings.

#include "BloodyGroundPlayerController.h"
#include "GameFramework/PlayerState.h"

// 생성자: RTT와 마지막 RTT 갱신 시간 초기화
ABloodyGroundPlayerController::ABloodyGroundPlayerController()
{
    // RTT 초기화
    RoundTripTime = 0.0f;
    LastRTTUpdateTime = 0.0f;
}

// 플레이어의 틱 함수: 매 프레임마다 호출되며, RTT를 주기적으로 갱신
void ABloodyGroundPlayerController::PlayerTick(float DeltaTime)
{
    // 부모 클래스의 PlayerTick 함수 호출
    Super::PlayerTick(DeltaTime);

    // RTT 갱신 주기 체크: 마지막 갱신 이후 1초가 지났는지 확인
    if (GetWorld()->TimeSince(LastRTTUpdateTime) > 1.0f)  // 예: 매 1초마다 갱신
    {
        // 플레이어 상태가 유효하고, 로컬 컨트롤러인 경우
        if (PlayerState && IsLocalController())
        {
            // 플레이어 상태에서 핑 값을 가져와서 RTT를 갱신 (밀리초를 초로 변환)
            RoundTripTime = PlayerState->GetPing() * 0.001f;
            // 마지막 RTT 갱신 시간을 현재 시간으로 업데이트
            LastRTTUpdateTime = GetWorld()->GetTimeSeconds();
        }
    }
}
