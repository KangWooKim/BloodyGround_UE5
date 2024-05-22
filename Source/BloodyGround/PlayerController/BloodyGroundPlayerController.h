// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BloodyGroundPlayerController.generated.h"

/**
 * ABloodyGroundPlayerController 클래스는 게임 중 플레이어 컨트롤러를 관리합니다.
 */
UCLASS()
class BLOODYGROUND_API ABloodyGroundPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    // 생성자: RTT와 마지막 RTT 갱신 시간을 초기화합니다.
    ABloodyGroundPlayerController();

    // RTT 값을 반환하는 인라인 함수
    // @return 현재 RTT 값
    FORCEINLINE float GetRoundTripTime() { return RoundTripTime; }

protected:
    // 플레이어의 틱 함수: 매 프레임마다 호출되어 RTT를 주기적으로 갱신합니다.
    // @param DeltaTime 마지막 틱 이후 경과된 시간
    virtual void PlayerTick(float DeltaTime) override;

private:
    // RTT 관련 변수
    UPROPERTY(VisibleAnywhere, Category = "Network")
        float RoundTripTime;  // 왕복 시간 (RTT)을 저장하는 변수

    // 마지막으로 RTT를 갱신한 시점
    float LastRTTUpdateTime;  // 마지막 RTT 갱신 시간
};
