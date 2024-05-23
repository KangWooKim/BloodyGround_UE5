// Copyright Epic Games, Inc. All Rights Reserved.

#include "BloodyGroundGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "BloodyGround/Character/BaseCharacter.h"
#include "BloodyGround/HUD/InGameHUD.h"
#include "BloodyGround/HUD/InGameWidget.h"

// 생성자: 게임 모드의 기본 설정을 초기화합니다.
ABloodyGroundGameModeBase::ABloodyGroundGameModeBase()
{
    // ABaseCharacter 클래스를 DefaultPawnClass로 설정
    DefaultPawnClass = ABaseCharacter::StaticClass();
}

// 플레이어를 리스폰하는 함수
// @param PC 리스폰할 플레이어의 컨트롤러
void ABloodyGroundGameModeBase::RespawnPlayer(APlayerController* PC)
{
    // 플레이어 컨트롤러가 유효한지와 서버 권한이 있는지 확인
    if (!PC || !HasAuthority())
    {
        return;
    }

    // 맵에서 모든 PlayerStart 액터를 찾음
    TArray<AActor*> PlayerStarts;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);

    // PlayerStart 액터가 없으면 경고 메시지를 출력하고 리턴
    if (PlayerStarts.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No PlayerStarts found on map."));
        return;
    }

    // 랜덤한 PlayerStart 액터를 선택
    int32 Index = FMath::RandRange(0, PlayerStarts.Num() - 1);
    APlayerStart* ChosenStart = Cast<APlayerStart>(PlayerStarts[Index]);

    // 선택된 PlayerStart가 유효한지 확인
    if (!ChosenStart)
    {
        UE_LOG(LogTemp, Warning, TEXT("Selected PlayerStart is not valid."));
        return;
    }

    // 새 캐릭터 스폰 위치 및 회전을 PlayerStart 위치 및 회전으로 설정
    FVector SpawnLocation = ChosenStart->GetActorLocation();
    FRotator SpawnRotation = ChosenStart->GetActorRotation();

    // DefaultPawnClass가 유효한지 확인
    if (!DefaultPawnClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("DefaultPawnClass is not set."));
        return;
    }

    // 새 캐릭터 스폰
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    ABaseCharacter* NewCharacter = GetWorld()->SpawnActor<ABaseCharacter>(DefaultPawnClass, SpawnLocation, SpawnRotation, SpawnParams);

    // 새 캐릭터가 유효한지 확인 후 플레이어 컨트롤러에 할당
    if (NewCharacter)
    {
        PC->Possess(NewCharacter);
        NewCharacter->PlayerController = PC;
       
        AInGameHUD* PlayerHUD = Cast<AInGameHUD>(PC->GetHUD());
        if (PlayerHUD)
        {
            PlayerHUD->UpdateHealth(1.f); // HUD에 초기 체력 업데이트
            PlayerHUD->DeleteRespawnText(); // Respawn 텍스트 삭제
            PlayerHUD->UpdateAmmo(10, 50);
        }
    }
}
