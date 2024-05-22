// Fill out your copyright notice in the Description page of Project Settings.

#include "BloodyGroundPlayerController.h"
#include "GameFramework/PlayerState.h"

// ������: RTT�� ������ RTT ���� �ð� �ʱ�ȭ
ABloodyGroundPlayerController::ABloodyGroundPlayerController()
{
    // RTT �ʱ�ȭ
    RoundTripTime = 0.0f;
    LastRTTUpdateTime = 0.0f;
}

// �÷��̾��� ƽ �Լ�: �� �����Ӹ��� ȣ��Ǹ�, RTT�� �ֱ������� ����
void ABloodyGroundPlayerController::PlayerTick(float DeltaTime)
{
    // �θ� Ŭ������ PlayerTick �Լ� ȣ��
    Super::PlayerTick(DeltaTime);

    // RTT ���� �ֱ� üũ: ������ ���� ���� 1�ʰ� �������� Ȯ��
    if (GetWorld()->TimeSince(LastRTTUpdateTime) > 1.0f)  // ��: �� 1�ʸ��� ����
    {
        // �÷��̾� ���°� ��ȿ�ϰ�, ���� ��Ʈ�ѷ��� ���
        if (PlayerState && IsLocalController())
        {
            // �÷��̾� ���¿��� �� ���� �����ͼ� RTT�� ���� (�и��ʸ� �ʷ� ��ȯ)
            RoundTripTime = PlayerState->GetPing() * 0.001f;
            // ������ RTT ���� �ð��� ���� �ð����� ������Ʈ
            LastRTTUpdateTime = GetWorld()->GetTimeSeconds();
        }
    }
}
