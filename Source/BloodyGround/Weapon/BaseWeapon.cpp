#include "BaseWeapon.h"
#include "Animation/AnimationAsset.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "BloodyGround/Character/BaseCharacter.h"
#include "Components/PawnNoiseEmitterComponent.h"
#include "BloodyGround/Component/InventoryComponent.h"
#include "BloodyGround/PlayerController/BloodyGroundPlayerController.h"
#include "BloodyGround/Enemy/EliteZombie.h"
#include "BloodyGround/Component/ServerLocationComponent.h"
#include "BloodyGround/HUD/InGameHUD.h"

// ������
ABaseWeapon::ABaseWeapon()
{
    // �� �����Ӹ��� ƽ �Լ��� ȣ������ ����
    PrimaryActorTick.bCanEverTick = false;

    // ���� ���� ���� ����. �� ���ʹ� ���� �����մϴ�.
    bReplicates = true;

    // ������ ���� ���� ����. �� ������ �������� �����˴ϴ�.
    SetReplicateMovement(true);
    SetReplicates(true);

    // ���̷�Ż �޽� ������Ʈ �ʱ�ȭ
    SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
    SetRootComponent(SkeletalMeshComponent);

    // ���� �޽��� �浹 ���� ����
    SkeletalMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
    SkeletalMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
    SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // ���� ���� ������Ʈ �ʱ�ȭ
    NoiseEmitter = CreateDefaultSubobject<UPawnNoiseEmitterComponent>(TEXT("Noise Emitter"));

    // ��Ʈ �Ӱ谪 �ʱ�ȭ
    HitThreshold = 100.f;
}

// ���Ͱ� ���۵� �� ȣ��Ǵ� �Լ�
void ABaseWeapon::BeginPlay()
{
    Super::BeginPlay();

    // �ʱ� ���� ���� ����
    WeaponState = EWeaponState::None;

    // ������ ĳ���� ĳ����
    Character = Cast<ABaseCharacter>(GetOwner());
}

// ��Ʈ��ũ ���� �Ӽ��� �����ϴ� �Լ�
void ABaseWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // ������ �Ӽ� �߰�
    DOREPLIFETIME(ABaseWeapon, WeaponState);
    DOREPLIFETIME(ABaseWeapon, CurrentAmmo);
}

// ���� ��ü �Լ� (�������� ����)
void ABaseWeapon::ChangeWeapon()
{
}

// �� �����Ӹ��� ȣ��Ǵ� ƽ �Լ�
void ABaseWeapon::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// �߻� �Լ�
void ABaseWeapon::Fire()
{
    // �������� ���� ��
    if (Character->HasAuthority())
    {
        MulticastPlayFireAnimation();
        WeaponNoise(1.f);
    }
    // Ŭ���̾�Ʈ���� ���� ��
    else if (Character->IsLocallyControlled())
    {
        ServerPlayFireAnimation();
    }

    WeaponState = EWeaponState::Fire;

    if (Character)
    {
        // �߻� ���� ��ġ�� ���� ���
        FVector StartLocation = SkeletalMeshComponent->GetSocketLocation(TEXT("MuzzleFlash"));
        FVector ForwardVector = Character->GetControlRotation().Vector();
        FVector EndLocation = StartLocation + ForwardVector * 10000.0f;

        FHitResult HitResult;
        FCollisionQueryParams QueryParams;
        QueryParams.AddIgnoredActor(this);
        QueryParams.AddIgnoredActor(Character);

        // ����Ʈ���̽� ���� ��Ʈ üũ
        if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, QueryParams))
        {
            ABaseZombie* HitZombie = Cast<ABaseZombie>(HitResult.GetActor());
            if (HitZombie)
            {
                if (Character->HasAuthority())
                {
                    // �������� ���� ���� Ȯ��
                    FLocationTimeData TempLocationData = HitZombie->GetServerLocationComponent()->GetLocationData();
                    FHitResultData HitResultData = HitZombie->GetServerLocationComponent()->ServerTrace(StartLocation, EndLocation, TempLocationData);
                    if (HitResultData.bHitBody)
                    {
                        HitZombie->TakeShot(HitResultData, Damage);
                        UGameplayStatics::ApplyDamage(HitZombie, Damage, Character->GetController(), this, UDamageType::StaticClass());
                    }
                }
                else if (Character->IsLocallyControlled())
                {
                    // Ŭ���̾�Ʈ���� ���� ���θ� ������ ����
                    ServerCheckHit(HitResult, GetWorld()->GetTimeSeconds(), StartLocation, ForwardVector);
                }
            }
        }
    }
}

// Ŭ���̾�Ʈ�� ��Ʈ Ȯ�� ��û�� ó���ϴ� ���� �Լ�
void ABaseWeapon::ServerCheckHit_Implementation(FHitResult ClientHitResult, float HitTime, FVector StartLocation, FVector EndDirection)
{
    ABaseZombie* HitZombie = Cast<ABaseZombie>(ClientHitResult.GetActor());
    if (!HitZombie) return;

    ABloodyGroundPlayerController* PlayerController = Cast<ABloodyGroundPlayerController>(Character->GetController());
    if (!PlayerController) return;

    float RTT = PlayerController->GetRoundTripTime();
    UServerLocationComponent* ServerLocationComp = HitZombie->FindComponentByClass<UServerLocationComponent>();
    if (!ServerLocationComp) return;

    float CorrectedTime = HitTime - (RTT / 2.0f);
    FLocationTimeData LocationData = ServerLocationComp->GetInterpolatedLocationData(CorrectedTime);
    FVector EndLocation = StartLocation + EndDirection * 10000.0f;

    FHitResultData HitResults = ServerLocationComp->CheckHitWithTrace(StartLocation, EndLocation, LocationData);

    // ���뿡 ������ ���
    if (HitResults.bHitBody)
    {
        HitZombie->TakeShot(HitResults, Damage);
        UGameplayStatics::ApplyDamage(HitZombie, Damage, Character->GetController(), this, UDamageType::StaticClass());
    }
}

// �������� Ŭ���̾�Ʈ�� ��Ʈ Ȯ�� ��û�� ��ȿȭ�ϴ� �Լ�
bool ABaseWeapon::ServerCheckHit_Validate(FHitResult HitResult, float HitTime, FVector StartLocation, FVector EndDirection)
{
    return true; // �߰����� ��ȿ�� �˻� ������ �ʿ��� �� �ֽ��ϴ�
}

// ������ �߻���Ű�� �Լ�
void ABaseWeapon::WeaponNoise_Implementation(float Loudness)
{
    if (Character)
    {
        Character->MakeNoise(Loudness, Character, GetActorLocation());
    }
}

// �߻� �������� ���θ� Ȯ���ϴ� �Լ�
bool ABaseWeapon::CanFire()
{
    return true;
}

// �߻� ���� �Լ�
void ABaseWeapon::FireEnd()
{
    WeaponState = EWeaponState::None;
}

// ������ ���� �Լ�
void ABaseWeapon::ReloadEnd()
{
    WeaponState = EWeaponState::None;
}

// ������ �Լ�
void ABaseWeapon::Reload()
{
    if (Character)
    {
        // Ŭ���̾�Ʈ���� ������ ������ ��û ����
        if (Character->IsLocallyControlled() && !Character->HasAuthority())
        {
            ServerReload();
        }

        // �������� ���� ������ ó��
        else if (Character->HasAuthority())
        {
            PerformReload();
        }
    }
}

// ���� �������� �����ϴ� �Լ�
void ABaseWeapon::PerformReload()
{
    if (WeaponState == EWeaponState::None)
    {
        if (Character == nullptr || Character->InventoryComp == nullptr)
        {
            return; // ĳ���� �Ǵ� �κ��丮 ������Ʈ�� ���� ��� ����
        }

        WeaponState = EWeaponState::Reload;

        // ��� Ŭ���̾�Ʈ���� ������ �ִϸ��̼� ���
        MulticastPlayReloadAnimation();

        // ���� �߻�
        WeaponNoise(0.3f);  // ������ ���� ũ�� ����

        // ź�� ���� ���� �߰�
        if (IsA<APistol>())
        {
            int32 AmmoToReload = FMath::Min(Capacity - CurrentAmmo, Character->InventoryComp->GetPistolAmmo());
            CurrentAmmo += AmmoToReload;
            Character->InventoryComp->SetPistolAmmo(Character->InventoryComp->GetPistolAmmo() - AmmoToReload);
        }
        else if (IsA<AMachineGun>())
        {
            int32 AmmoToReload = FMath::Min(Capacity - CurrentAmmo, Character->InventoryComp->GetMachineGunAmmo());
            CurrentAmmo += AmmoToReload;
            Character->InventoryComp->SetMachineGunAmmo(Character->InventoryComp->GetMachineGunAmmo() - AmmoToReload);
        }

        // HUD ������Ʈ
        if (Character->GetInGameHUD())
        {
            if (IsA<APistol>())
            {
                Character->GetInGameHUD()->UpdateAmmo(CurrentAmmo, Character->InventoryComp->GetPistolAmmo());
            }
            else if (IsA<AMachineGun>())
            {
                Character->GetInGameHUD()->UpdateAmmo(CurrentAmmo, Character->InventoryComp->GetMachineGunAmmo());
            }
        }

        WeaponState = EWeaponState::None;
    }
}

// �������� �������� ó���ϴ� �Լ�
void ABaseWeapon::ServerReload_Implementation()
{
    PerformReload();
}

// ������ �ִϸ��̼��� ��� Ŭ���̾�Ʈ���� ����ϴ� �Լ�
void ABaseWeapon::MulticastPlayReloadAnimation_Implementation()
{
    if (ReloadAnimation)
    {
        SkeletalMeshComponent->PlayAnimation(ReloadAnimation, false);
    }
}

// ���� ���� Ÿ���� ��ȯ�ϴ� �Լ�
EWeaponType ABaseWeapon::GetCurrentWeaponType()
{
    return EWeaponType::None;
}

// �߻� �ִϸ��̼��� �������� ����ϴ� �Լ�
void ABaseWeapon::ServerPlayFireAnimation_Implementation()
{
    MulticastPlayFireAnimation();
    WeaponNoise(1.f);
}

// �߻� �ִϸ��̼��� ��� Ŭ���̾�Ʈ���� ����ϴ� �Լ�
void ABaseWeapon::MulticastPlayFireAnimation_Implementation()
{
    if (FireAnimation)
    {
        SkeletalMeshComponent->PlayAnimation(FireAnimation, false);
    }
}
