// Copyright Epic Games, Inc. All Rights Reserved.

#include "BloodyGroundGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "BloodyGround/Character/BaseCharacter.h"
#include "BloodyGround/HUD/InGameHUD.h"
#include "BloodyGround/HUD/InGameWidget.h"

// ������: ���� ����� �⺻ ������ �ʱ�ȭ�մϴ�.
ABloodyGroundGameModeBase::ABloodyGroundGameModeBase()
{
    // ABaseCharacter Ŭ������ DefaultPawnClass�� ����
    DefaultPawnClass = ABaseCharacter::StaticClass();
}

// �÷��̾ �������ϴ� �Լ�
// @param PC �������� �÷��̾��� ��Ʈ�ѷ�
void ABloodyGroundGameModeBase::RespawnPlayer(APlayerController* PC)
{
    // �÷��̾� ��Ʈ�ѷ��� ��ȿ������ ���� ������ �ִ��� Ȯ��
    if (!PC || !HasAuthority())
    {
        return;
    }

    // �ʿ��� ��� PlayerStart ���͸� ã��
    TArray<AActor*> PlayerStarts;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);

    // PlayerStart ���Ͱ� ������ ��� �޽����� ����ϰ� ����
    if (PlayerStarts.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No PlayerStarts found on map."));
        return;
    }

    // ������ PlayerStart ���͸� ����
    int32 Index = FMath::RandRange(0, PlayerStarts.Num() - 1);
    APlayerStart* ChosenStart = Cast<APlayerStart>(PlayerStarts[Index]);

    // ���õ� PlayerStart�� ��ȿ���� Ȯ��
    if (!ChosenStart)
    {
        UE_LOG(LogTemp, Warning, TEXT("Selected PlayerStart is not valid."));
        return;
    }

    // �� ĳ���� ���� ��ġ �� ȸ���� PlayerStart ��ġ �� ȸ������ ����
    FVector SpawnLocation = ChosenStart->GetActorLocation();
    FRotator SpawnRotation = ChosenStart->GetActorRotation();

    // DefaultPawnClass�� ��ȿ���� Ȯ��
    if (!DefaultPawnClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("DefaultPawnClass is not set."));
        return;
    }

    // �� ĳ���� ����
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    ABaseCharacter* NewCharacter = GetWorld()->SpawnActor<ABaseCharacter>(DefaultPawnClass, SpawnLocation, SpawnRotation, SpawnParams);

    // �� ĳ���Ͱ� ��ȿ���� Ȯ�� �� �÷��̾� ��Ʈ�ѷ��� �Ҵ�
    if (NewCharacter)
    {
        PC->Possess(NewCharacter);
        NewCharacter->PlayerController = PC;
       
        AInGameHUD* PlayerHUD = Cast<AInGameHUD>(PC->GetHUD());
        if (PlayerHUD)
        {
            PlayerHUD->UpdateHealth(1.f); // HUD�� �ʱ� ü�� ������Ʈ
            PlayerHUD->DeleteRespawnText(); // Respawn �ؽ�Ʈ ����
            PlayerHUD->UpdateAmmo(10, 50);
        }
    }
}
