#include "KeyItem.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "BloodyGround/Character/BaseCharacter.h"
#include "BloodyGround/HUD/InGameHUD.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Engine/Engine.h"

// 생성자: AKeyItem 클래스의 인스턴스를 초기화합니다.
AKeyItem::AKeyItem()
{
    // 매 프레임마다 Tick 함수를 호출하도록 설정
    PrimaryActorTick.bCanEverTick = true;

    // 메시 컴포넌트를 생성하고 루트 컴포넌트로 설정
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    RootComponent = Mesh;

    // 네트워크 복제 활성화
    SetReplicates(true);
    SetReplicateMovement(true);

    // 충돌 구성 요소(SphereComponent)를 생성하고 설정
    SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
    SphereComponent->SetSphereRadius(100.0f);
    SphereComponent->SetupAttachment(RootComponent);
    SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AKeyItem::OnOverlapBegin);
}

// BeginPlay: 게임이 시작되거나 스폰될 때 호출되는 함수
void AKeyItem::BeginPlay()
{
    Super::BeginPlay();
    StartLocation = GetActorLocation(); // 아이템의 시작 위치 저장
}

// Tick: 매 프레임마다 호출되는 함수
void AKeyItem::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 위아래로 움직이는 애니메이션 효과를 구현
    FVector NewLocation = StartLocation;
    RunningTime += DeltaTime;
    float DeltaHeight = (FMath::Sin(RunningTime + DeltaTime) - FMath::Sin(RunningTime));
    NewLocation.Z += DeltaHeight * 20.0f; // 조정 가능한 매개변수
    SetActorLocation(NewLocation);
}

// OnOverlapBegin: 다른 액터와의 충돌을 감지하는 함수
void AKeyItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // 서버에서만 실행
    if (HasAuthority())
    {
        if (!OtherActor || !OtherComp)
        {
            UE_LOG(LogTemp, Warning, TEXT("Overlap ignored due to null actor or component."));
            return;
        }

        // 충돌한 액터가 ABaseCharacter인 경우 처리
        ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(OtherActor);
        if (BaseCharacter && OtherActor != this && OtherComp)
        {
            APlayerState* PlayerState = BaseCharacter->GetPlayerState();
            if (PlayerState)
            {
                // 서버에서 효과음 재생 및 아이템 제거 처리
                Multicast_PlaySoundAndDestroy();

                // 클라이언트에게 승자 표시 및 메인 메뉴로 이동 요청
                FString WinnerName = PlayerState->GetPlayerName();
                Multicast_ShowWinnerAndReturnToMainMenu(WinnerName);
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("OnOverlapBegin called on client."));
    }
}

// Multicast_PlaySoundAndDestroy: 모든 클라이언트에서 효과음을 재생하고 아이템을 제거하는 함수
void AKeyItem::Multicast_PlaySoundAndDestroy_Implementation()
{
    // 모든 클라이언트에서 효과음 재생
    if (PickupSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());
    }
}

// Multicast_ShowWinnerAndReturnToMainMenu: 모든 클라이언트에 승자 메시지를 표시하고 메인 메뉴로 이동하는 함수
void AKeyItem::Multicast_ShowWinnerAndReturnToMainMenu_Implementation(const FString& WinnerName)
{
    // 모든 클라이언트에 승자 메시지 표시
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        APlayerController* PC = It->Get();
        if (PC)
        {
            AInGameHUD* HUD = Cast<AInGameHUD>(PC->GetHUD());
            if (HUD)
            {
                HUD->SetWinnerText(WinnerName);
            }
        }
    }

    // 5초 후에 메인 메뉴로 이동
    GetWorld()->GetTimerManager().SetTimer(TimerHandle_ReturnToMainMenu, this, &AKeyItem::ReturnToMainMenu, 5.0f, false);
}

// ReturnToMainMenu: 메인 메뉴로 이동시키고 서버를 종료하는 함수
void AKeyItem::ReturnToMainMenu()
{
    UWorld* World = GetWorld();
    if (World)
    {
        for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
        {
            APlayerController* PC = It->Get();
            if (PC)
            {
                // 클라이언트 트래블을 통해 메인 메뉴로 이동
                PC->ClientTravel(TEXT("/Game/Maps/Lobby"), ETravelType::TRAVEL_Absolute);
            }
        }

        // 5초 후에 서버 종료
        FTimerHandle ServerShutdownHandle;
        World->GetTimerManager().SetTimer(ServerShutdownHandle, []()
            {
                FGenericPlatformMisc::RequestExit(false);
            }, 5.0f, false);
    }
}
