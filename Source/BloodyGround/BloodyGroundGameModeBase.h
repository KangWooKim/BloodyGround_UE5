// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BloodyGroundGameModeBase.generated.h"

/**
 * ABloodyGroundGameModeBase 클래스는 게임 모드의 기본 동작을 정의합니다.
 */
UCLASS()
class BLOODYGROUND_API ABloodyGroundGameModeBase : public AGameMode
{
    GENERATED_BODY()

public:

    // 생성자: 게임 모드의 기본 설정을 초기화합니다.
    ABloodyGroundGameModeBase();

    // 플레이어를 리스폰하는 함수 선언
    // @param PC 리스폰할 플레이어의 컨트롤러
    void RespawnPlayer(APlayerController* PC);
};
