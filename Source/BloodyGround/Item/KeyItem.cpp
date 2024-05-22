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

// ������: AKeyItem Ŭ������ �ν��Ͻ��� �ʱ�ȭ�մϴ�.
AKeyItem::AKeyItem()
{
    // �� �����Ӹ��� Tick �Լ��� ȣ���ϵ��� ����
    PrimaryActorTick.bCanEverTick = true;

    // �޽� ������Ʈ�� �����ϰ� ��Ʈ ������Ʈ�� ����
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    RootComponent = Mesh;

    // ��Ʈ��ũ ���� Ȱ��ȭ
    SetReplicates(true);
    SetReplicateMovement(true);

    // �浹 ���� ���(SphereComponent)�� �����ϰ� ����
    SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
    SphereComponent->SetSphereRadius(100.0f);
    SphereComponent->SetupAttachment(RootComponent);
    SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AKeyItem::OnOverlapBegin);
}

// BeginPlay: ������ ���۵ǰų� ������ �� ȣ��Ǵ� �Լ�
void AKeyItem::BeginPlay()
{
    Super::BeginPlay();
    StartLocation = GetActorLocation(); // �������� ���� ��ġ ����
}

// Tick: �� �����Ӹ��� ȣ��Ǵ� �Լ�
void AKeyItem::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // ���Ʒ��� �����̴� �ִϸ��̼� ȿ���� ����
    FVector NewLocation = StartLocation;
    RunningTime += DeltaTime;
    float DeltaHeight = (FMath::Sin(RunningTime + DeltaTime) - FMath::Sin(RunningTime));
    NewLocation.Z += DeltaHeight * 20.0f; // ���� ������ �Ű�����
    SetActorLocation(NewLocation);
}

// OnOverlapBegin: �ٸ� ���Ϳ��� �浹�� �����ϴ� �Լ�
void AKeyItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // ���������� ����
    if (HasAuthority())
    {
        if (!OtherActor || !OtherComp)
        {
            UE_LOG(LogTemp, Warning, TEXT("Overlap ignored due to null actor or component."));
            return;
        }

        // �浹�� ���Ͱ� ABaseCharacter�� ��� ó��
        ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(OtherActor);
        if (BaseCharacter && OtherActor != this && OtherComp)
        {
            APlayerState* PlayerState = BaseCharacter->GetPlayerState();
            if (PlayerState)
            {
                // �������� ȿ���� ��� �� ������ ���� ó��
                Multicast_PlaySoundAndDestroy();

                // Ŭ���̾�Ʈ���� ���� ǥ�� �� ���� �޴��� �̵� ��û
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

// Multicast_PlaySoundAndDestroy: ��� Ŭ���̾�Ʈ���� ȿ������ ����ϰ� �������� �����ϴ� �Լ�
void AKeyItem::Multicast_PlaySoundAndDestroy_Implementation()
{
    // ��� Ŭ���̾�Ʈ���� ȿ���� ���
    if (PickupSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());
    }
}

// Multicast_ShowWinnerAndReturnToMainMenu: ��� Ŭ���̾�Ʈ�� ���� �޽����� ǥ���ϰ� ���� �޴��� �̵��ϴ� �Լ�
void AKeyItem::Multicast_ShowWinnerAndReturnToMainMenu_Implementation(const FString& WinnerName)
{
    // ��� Ŭ���̾�Ʈ�� ���� �޽��� ǥ��
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

    // 5�� �Ŀ� ���� �޴��� �̵�
    GetWorld()->GetTimerManager().SetTimer(TimerHandle_ReturnToMainMenu, this, &AKeyItem::ReturnToMainMenu, 5.0f, false);
}

// ReturnToMainMenu: ���� �޴��� �̵���Ű�� ������ �����ϴ� �Լ�
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
                // Ŭ���̾�Ʈ Ʈ������ ���� ���� �޴��� �̵�
                PC->ClientTravel(TEXT("/Game/Maps/Lobby"), ETravelType::TRAVEL_Absolute);
            }
        }

        // 5�� �Ŀ� ���� ����
        FTimerHandle ServerShutdownHandle;
        World->GetTimerManager().SetTimer(ServerShutdownHandle, []()
            {
                FGenericPlatformMisc::RequestExit(false);
            }, 5.0f, false);
    }
}
