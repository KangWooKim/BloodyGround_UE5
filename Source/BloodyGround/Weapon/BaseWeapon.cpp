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

// 생성자
ABaseWeapon::ABaseWeapon()
{
    // 매 프레임마다 틱 함수를 호출하지 않음
    PrimaryActorTick.bCanEverTick = false;

    // 복제 가능 여부 설정. 이 액터는 복제 가능합니다.
    bReplicates = true;

    // 움직임 복제 여부 설정. 이 액터의 움직임은 복제됩니다.
    SetReplicateMovement(true);
    SetReplicates(true);

    // 스켈레탈 메시 컴포넌트 초기화
    SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
    SetRootComponent(SkeletalMeshComponent);

    // 무기 메시의 충돌 응답 설정
    SkeletalMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
    SkeletalMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
    SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // 소음 방출 컴포넌트 초기화
    NoiseEmitter = CreateDefaultSubobject<UPawnNoiseEmitterComponent>(TEXT("Noise Emitter"));

    // 히트 임계값 초기화
    HitThreshold = 100.f;
}

// 액터가 시작될 때 호출되는 함수
void ABaseWeapon::BeginPlay()
{
    Super::BeginPlay();

    // 초기 무기 상태 설정
    WeaponState = EWeaponState::None;

    // 소유한 캐릭터 캐스팅
    Character = Cast<ABaseCharacter>(GetOwner());
}

// 네트워크 복제 속성을 설정하는 함수
void ABaseWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // 복제할 속성 추가
    DOREPLIFETIME(ABaseWeapon, WeaponState);
    DOREPLIFETIME(ABaseWeapon, CurrentAmmo);
}

// 무기 교체 함수 (구현되지 않음)
void ABaseWeapon::ChangeWeapon()
{
}

// 매 프레임마다 호출되는 틱 함수
void ABaseWeapon::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// 발사 함수
void ABaseWeapon::Fire()
{
    // 서버에서 실행 시
    if (Character->HasAuthority())
    {
        MulticastPlayFireAnimation();
        WeaponNoise(1.f);
    }
    // 클라이언트에서 실행 시
    else if (Character->IsLocallyControlled())
    {
        ServerPlayFireAnimation();
    }

    WeaponState = EWeaponState::Fire;

    if (Character)
    {
        // 발사 시작 위치와 방향 계산
        FVector StartLocation = SkeletalMeshComponent->GetSocketLocation(TEXT("MuzzleFlash"));
        FVector ForwardVector = Character->GetControlRotation().Vector();
        FVector EndLocation = StartLocation + ForwardVector * 10000.0f;

        FHitResult HitResult;
        FCollisionQueryParams QueryParams;
        QueryParams.AddIgnoredActor(this);
        QueryParams.AddIgnoredActor(Character);

        // 라인트레이스 통해 히트 체크
        if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, QueryParams))
        {
            ABaseZombie* HitZombie = Cast<ABaseZombie>(HitResult.GetActor());
            if (HitZombie)
            {
                if (Character->HasAuthority())
                {
                    // 서버에서 적중 여부 확인
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
                    // 클라이언트에서 적중 여부를 서버로 전송
                    ServerCheckHit(HitResult, GetWorld()->GetTimeSeconds(), StartLocation, ForwardVector);
                }
            }
        }
    }
}

// 클라이언트의 히트 확인 요청을 처리하는 서버 함수
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

    // 몸통에 적중한 경우
    if (HitResults.bHitBody)
    {
        HitZombie->TakeShot(HitResults, Damage);
        UGameplayStatics::ApplyDamage(HitZombie, Damage, Character->GetController(), this, UDamageType::StaticClass());
    }
}

// 서버에서 클라이언트의 히트 확인 요청을 유효화하는 함수
bool ABaseWeapon::ServerCheckHit_Validate(FHitResult HitResult, float HitTime, FVector StartLocation, FVector EndDirection)
{
    return true; // 추가적인 유효성 검사 로직이 필요할 수 있습니다
}

// 소음을 발생시키는 함수
void ABaseWeapon::WeaponNoise_Implementation(float Loudness)
{
    if (Character)
    {
        Character->MakeNoise(Loudness, Character, GetActorLocation());
    }
}

// 발사 가능한지 여부를 확인하는 함수
bool ABaseWeapon::CanFire()
{
    return true;
}

// 발사 종료 함수
void ABaseWeapon::FireEnd()
{
    WeaponState = EWeaponState::None;
}

// 재장전 종료 함수
void ABaseWeapon::ReloadEnd()
{
    WeaponState = EWeaponState::None;
}

// 재장전 함수
void ABaseWeapon::Reload()
{
    if (Character)
    {
        // 클라이언트에서 서버로 재장전 요청 전송
        if (Character->IsLocallyControlled() && !Character->HasAuthority())
        {
            ServerReload();
        }

        // 서버에서 직접 재장전 처리
        else if (Character->HasAuthority())
        {
            PerformReload();
        }
    }
}

// 실제 재장전을 수행하는 함수
void ABaseWeapon::PerformReload()
{
    if (WeaponState == EWeaponState::None)
    {
        if (Character == nullptr || Character->InventoryComp == nullptr)
        {
            return; // 캐릭터 또는 인벤토리 컴포넌트가 없는 경우 리턴
        }

        WeaponState = EWeaponState::Reload;

        // 모든 클라이언트에서 재장전 애니메이션 재생
        MulticastPlayReloadAnimation();

        // 소음 발생
        WeaponNoise(0.3f);  // 재장전 소음 크기 조정

        // 탄약 갱신 로직 추가
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

        // HUD 업데이트
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

// 서버에서 재장전을 처리하는 함수
void ABaseWeapon::ServerReload_Implementation()
{
    PerformReload();
}

// 재장전 애니메이션을 모든 클라이언트에서 재생하는 함수
void ABaseWeapon::MulticastPlayReloadAnimation_Implementation()
{
    if (ReloadAnimation)
    {
        SkeletalMeshComponent->PlayAnimation(ReloadAnimation, false);
    }
}

// 현재 무기 타입을 반환하는 함수
EWeaponType ABaseWeapon::GetCurrentWeaponType()
{
    return EWeaponType::None;
}

// 발사 애니메이션을 서버에서 재생하는 함수
void ABaseWeapon::ServerPlayFireAnimation_Implementation()
{
    MulticastPlayFireAnimation();
    WeaponNoise(1.f);
}

// 발사 애니메이션을 모든 클라이언트에서 재생하는 함수
void ABaseWeapon::MulticastPlayFireAnimation_Implementation()
{
    if (FireAnimation)
    {
        SkeletalMeshComponent->PlayAnimation(FireAnimation, false);
    }
}
