// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BloodyGroundPlayerController.generated.h"

/**
 * ABloodyGroundPlayerController Ŭ������ ���� �� �÷��̾� ��Ʈ�ѷ��� �����մϴ�.
 */
UCLASS()
class BLOODYGROUND_API ABloodyGroundPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    // ������: RTT�� ������ RTT ���� �ð��� �ʱ�ȭ�մϴ�.
    ABloodyGroundPlayerController();

    // RTT ���� ��ȯ�ϴ� �ζ��� �Լ�
    // @return ���� RTT ��
    FORCEINLINE float GetRoundTripTime() { return RoundTripTime; }

protected:
    // �÷��̾��� ƽ �Լ�: �� �����Ӹ��� ȣ��Ǿ� RTT�� �ֱ������� �����մϴ�.
    // @param DeltaTime ������ ƽ ���� ����� �ð�
    virtual void PlayerTick(float DeltaTime) override;

private:
    // RTT ���� ����
    UPROPERTY(VisibleAnywhere, Category = "Network")
        float RoundTripTime;  // �պ� �ð� (RTT)�� �����ϴ� ����

    // ���������� RTT�� ������ ����
    float LastRTTUpdateTime;  // ������ RTT ���� �ð�
};
