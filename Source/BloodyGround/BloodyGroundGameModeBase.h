// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BloodyGroundGameModeBase.generated.h"

/**
 * ABloodyGroundGameModeBase Ŭ������ ���� ����� �⺻ ������ �����մϴ�.
 */
UCLASS()
class BLOODYGROUND_API ABloodyGroundGameModeBase : public AGameMode
{
    GENERATED_BODY()

public:

    // ������: ���� ����� �⺻ ������ �ʱ�ȭ�մϴ�.
    ABloodyGroundGameModeBase();

    // �÷��̾ �������ϴ� �Լ� ����
    // @param PC �������� �÷��̾��� ��Ʈ�ѷ�
    void RespawnPlayer(APlayerController* PC);
};
