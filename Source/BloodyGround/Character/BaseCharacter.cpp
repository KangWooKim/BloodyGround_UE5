#include "BaseCharacter.h"
#include "Net/UnrealNetwork.h"
#include "BloodyGround/Weapon/BaseWeapon.h"
#include "BloodyGround/Weapon/MachineGun.h"
#include "BloodyGround/Weapon/Pistol.h"
#include "BloodyGround/Component/InventoryComponent.h"
#include "Components/PawnNoiseEmitterComponent.h"
#include "Kismet/GameplayStatics.h"
#include "BloodyGround/BloodyGroundGameModeBase.h"
#include "BloodyGround/HUD/InGameHUD.h"
#include "BloodyGround/HUD/InGameWidget.h"

// 클래스 생성자
ABaseCharacter::ABaseCharacter()
{
    // 기본 설정: 틱 가능, 네트워크 복제 설정, 체력 초기화
    PrimaryActorTick.bCanEverTick = true;
    Health = 100.0f;
    SetReplicates(true);
    SetReplicateMovement(true);

    // BattleComponent 추가
    BattleComp = CreateDefaultSubobject<UBattleComponent>(TEXT("BattleComponent"));

    // 카메라 붐 생성 및 설정
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 300.0f; // 카메라와 캐릭터 사이의 거리 설정
    CameraBoom->bUsePawnControlRotation = true; // 카메라가 캐릭터의 회전을 따라감
    CameraBoom->SocketOffset = FVector(0.0f, 50.0f, 50.0f); // 카메라 위치 조정

    // 카메라 생성 및 설정
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // 카메라 붐에 카메라 부착
    FollowCamera->bUsePawnControlRotation = false; // 카메라가 붐의 회전을 따라가지 않게 함

    // FOV 초기값 설정
    DefaultFOV = 90.0f;
    AimedFOV = 65.0f;  // 조준 시 FOV 값
    FOVInterpSpeed = 20.0f; // FOV 변경 속도

    // 인벤토리 컴포넌트 초기화
    InventoryComp = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));

    // 소음 발생 컴포넌트 초기화
    NoiseEmitter = CreateDefaultSubobject<UPawnNoiseEmitterComponent>(TEXT("Noise Emitter"));

    PlayerController = Cast<APlayerController>(GetController());
}

// 게임 시작 시 초기화
void ABaseCharacter::BeginPlay()
{
    Super::BeginPlay();

    Tags.AddUnique(FName("Player")); // 플레이어 태그 추가
    CharacterState = ECharacterState::None; // 캐릭터 상태 초기화

    PlayerController = PlayerController == nullptr ? Cast<APlayerController>(GetController()) : PlayerController;

    if (PlayerController)
    {
        PlayerHUD = Cast<AInGameHUD>(PlayerController->GetHUD());
    }

    if (PlayerHUD)
    {
        PlayerHUD->UpdateHealth(1.f); // HUD에 초기 체력 업데이트
        PlayerHUD->DeleteRespawnText(); // Respawn 텍스트 삭제
    }

    // 기본 무기 및 탄알 설정
    if (HasAuthority())
    {
        InitializeWeaponsAndAmmo();
    }

    // 카메라 기본 FOV 설정
    if (FollowCamera)
    {
        FollowCamera->SetFieldOfView(DefaultFOV);
    }
}

void ABaseCharacter::InitializeWeaponsAndAmmo()
{
    // 권총 생성 및 설정
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.Instigator = GetInstigator();

    APistol* NewPistol = GetWorld()->SpawnActor<APistol>(PistolBlueprint, this->GetActorLocation(), this->GetActorRotation(), SpawnParams);
    if (NewPistol)
    {
        InventoryComp->AddWeapon(NewPistol);
        InventoryComp->SetInitWeapon(NewPistol);
        NewPistol->SetActorHiddenInGame(false);

        // 무기를 캐릭터의 소켓에 부착
        const FName WeaponSocketName(TEXT("WeaponSocket_Pistol"));
        NewPistol->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocketName);
    }

    // 머신건 생성 및 설정
    AMachineGun* NewMachineGun = GetWorld()->SpawnActor<AMachineGun>(MachineGunBlueprint, this->GetActorLocation(), this->GetActorRotation(), SpawnParams);
    if (NewMachineGun)
    {
        InventoryComp->AddWeapon(NewMachineGun);
        NewMachineGun->SetActorHiddenInGame(true);

        // 무기를 캐릭터의 소켓에 부착
        const FName WeaponSocketName(TEXT("WeaponSocket_Rifle"));
        NewMachineGun->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocketName);
    }

    // 탄알 설정
    InventoryComp->SetPistolAmmo(InitialPistolAmmo);
    InventoryComp->SetMachineGunAmmo(InitialMachineGunAmmo);

    // HUD 업데이트
    if (PlayerHUD)
    {
        PlayerHUD->UpdateAmmo(NewPistol->CurrentAmmo, InitialPistolAmmo);
        PlayerHUD->UpdateHealth(1.f); // HUD에 초기 체력 업데이트
        PlayerHUD->DeleteRespawnText(); // Respawn 텍스트 삭제
    }
}

// 매 프레임마다 호출
void ABaseCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    PlayerController = PlayerController == nullptr ? Cast<APlayerController>(GetController()) : PlayerController;
    if (PlayerController)
    {
        PlayerHUD = PlayerHUD == nullptr ? Cast<AInGameHUD>(PlayerController->GetHUD()) : PlayerHUD;
    }

    // 조준 상태에 따른 캐릭터 회전 처리
    HandleAimingRotation();

    // FOV 보간 처리
    InterpFOV(DeltaTime);
}

// 조준 상태에 따른 캐릭터 회전 처리
void ABaseCharacter::HandleAimingRotation()
{
    if (BattleComp && BattleComp->bIsAiming && Controller)
    {
        // 조준 중일 때 컨트롤러의 회전을 따라감
        const FRotator NewRotation = Controller->GetControlRotation();
        SetActorRotation(FRotator(0.0f, NewRotation.Yaw, 0.0f));
    }
    // 조준 중이 아닐 때는 별도의 회전 처리를 하지 않음
}

// 피격 반응 종료 처리
void ABaseCharacter::HitReactionEnd()
{
    CharacterState = ECharacterState::None;
}

// 네트워크 복제 설정
void ABaseCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // 체력 변수 복제 설정
    DOREPLIFETIME(ABaseCharacter, Health);
    DOREPLIFETIME(ABaseCharacter, CharacterState);
}

// 데미지 처리 함수
float ABaseCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    if (CharacterState == ECharacterState::Death || DamageCauser && DamageCauser->IsA(ABaseCharacter::StaticClass()))
    {
        return 0.f;
    }

    Health -= DamageAmount;

    if (PlayerHUD) {
        PlayerHUD->UpdateHealth(Health / 100.f);
    }

    if (Health <= 0)
    {
        HandleDeath();
        if (PlayerHUD)
        {
            PlayerHUD->SetRespawnText();
        }
        return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    }

    CharacterState = ECharacterState::HitReact;

    return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

// 사망 처리 함수
void ABaseCharacter::HandleDeath()
{
    CharacterState = ECharacterState::Death;

    // 모든 입력 비활성화
    DisableInput(Cast<APlayerController>(GetController()));

    // 인벤토리의 모든 무기 제거
    InventoryComp->DestroyAllWeapons();

    // 사망 처리 후 5초 뒤에 Respawn 함수 호출
    FTimerHandle RespawnTimerHandle;
    GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, this, &ABaseCharacter::Respawn, 5.0f, false);
}

// 리스폰 처리 함수
void ABaseCharacter::Respawn()
{
    // GameMode를 통해 새 캐릭터 스폰
    if (ABloodyGroundGameModeBase* GM = Cast<ABloodyGroundGameModeBase>(GetWorld()->GetAuthGameMode()))
    {
        GM->RespawnPlayer(Cast<APlayerController>(GetController()));
    }

    // 사망한 캐릭터 제거
    Destroy();
}

// 플레이어 입력 설정
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // 이동 및 점프 입력 바인딩
    PlayerInputComponent->BindAxis("MoveForward", this, &ABaseCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &ABaseCharacter::MoveRight);
    PlayerInputComponent->BindAxis("Turn", this, &ABaseCharacter::Turn);
    PlayerInputComponent->BindAxis("LookUp", this, &ABaseCharacter::LookUp);

    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ABaseCharacter::Jump);
    PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ABaseCharacter::Reload);
    PlayerInputComponent->BindAction("ChangeWeapon", IE_Pressed, this, &ABaseCharacter::ChangeWeapon);
    PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &ABaseCharacter::AttackButtonPressed);
    PlayerInputComponent->BindAction("Attack", IE_Released, this, &ABaseCharacter::AttackButtonReleased);
    PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ABaseCharacter::AimButtonPressed);
    PlayerInputComponent->BindAction("Aim", IE_Released, this, &ABaseCharacter::AimButtonReleased);
}

// 재장전 처리 함수
void ABaseCharacter::Reload()
{
    if (InventoryComp && InventoryComp->GetCurrentWeapon())
    {
        InventoryComp->GetCurrentWeapon()->Reload();

        // HUD 업데이트
        if (PlayerHUD)
        {
            ABaseWeapon* CurrentWeapon = InventoryComp->GetCurrentWeapon();
            if (CurrentWeapon->IsA<APistol>())
            {
                PlayerHUD->UpdateAmmo(CurrentWeapon->CurrentAmmo, InventoryComp->GetPistolAmmo());
            }
            else if (CurrentWeapon->IsA<AMachineGun>())
            {
                PlayerHUD->UpdateAmmo(CurrentWeapon->CurrentAmmo, InventoryComp->GetMachineGunAmmo());
            }
        }
    }
}

// 무기 변경 처리 함수
void ABaseCharacter::ChangeWeapon()
{
    if (HasAuthority())
    {
        if (InventoryComp && InventoryComp->GetCurrentWeapon())
        {
            InventoryComp->ChangeWeapon();
        }
    }
    else
    {
        ServerChangeWeapon();
    }
}

// 서버에서 무기 변경 처리 함수
void ABaseCharacter::ServerChangeWeapon_Implementation()
{
    InventoryComp->ChangeWeapon();
}

// 서버 무기 변경 유효성 검사 함수
bool ABaseCharacter::ServerChangeWeapon_Validate()
{
    return true; // 유효성 검사 로직
}

// 서버에서 발소리 처리 함수
void ABaseCharacter::ServerFootStep_Implementation()
{
    this->MakeNoise(FootstepLoudness, this, GetActorLocation());
}

// 전/후 이동 처리 함수
void ABaseCharacter::MoveForward(float Value)
{
    if (CharacterState != ECharacterState::None)
    {
        return;
    }

    if ((Controller != nullptr) && (Value != 0.0f))
    {
        // 캐릭터의 방향 찾기
        const FRotator Rotation = Controller->GetControlRotation();
        const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);
        AddMovementInput(Direction, Value);
        ServerFootStep();
    }
}

// 좌/우 이동 처리 함수
void ABaseCharacter::MoveRight(float Value)
{
    if (CharacterState != ECharacterState::None)
    {
        return;
    }

    if ((Controller != nullptr) && (Value != 0.0f))
    {
        // 캐릭터의 방향 찾기
        const FRotator Rotation = Controller->GetControlRotation();
        const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);
        AddMovementInput(Direction, Value);
        ServerFootStep();
    }
}

// 좌우 회전 처리 함수
void ABaseCharacter::Turn(float Value)
{
    AddControllerYawInput(Value);
}

// 상하 회전 처리 함수
void ABaseCharacter::LookUp(float Value)
{
    AddControllerPitchInput(Value);
}

// 점프 처리 함수
void ABaseCharacter::Jump()
{
    if (CharacterState != ECharacterState::None)
    {
        return;
    }

    Super::Jump();
    ServerFootStep();
}

// 공격 버튼 누름 처리 함수
void ABaseCharacter::AttackButtonPressed()
{
    if (InventoryComp && InventoryComp->GetCurrentWeapon())
    {
        InventoryComp->GetCurrentWeapon()->Fire();

        // HUD 업데이트
        if (PlayerHUD)
        {
            ABaseWeapon* CurrentWeapon = InventoryComp->GetCurrentWeapon();
            if (CurrentWeapon->IsA<APistol>())
            {
                PlayerHUD->UpdateAmmo(CurrentWeapon->CurrentAmmo, InventoryComp->GetPistolAmmo());
            }
            else if (CurrentWeapon->IsA<AMachineGun>())
            {
                PlayerHUD->UpdateAmmo(CurrentWeapon->CurrentAmmo, InventoryComp->GetMachineGunAmmo());
            }
        }
    }
}

// 공격 버튼 뗌 처리 함수
void ABaseCharacter::AttackButtonReleased()
{
    if (HasAuthority())
    {
        if (InventoryComp && InventoryComp->GetCurrentWeapon())
        {
            InventoryComp->GetCurrentWeapon()->FireEnd();
        }
    }
    else
    {
        ServerStopAttack();
    }
}

// 서버에서 공격 중지 처리 함수
void ABaseCharacter::ServerStopAttack_Implementation()
{
    MulticastStopAttack();
}

// 멀티캐스트 공격 중지 처리 함수
void ABaseCharacter::MulticastStopAttack_Implementation()
{
    if (InventoryComp && InventoryComp->GetCurrentWeapon())
    {
        InventoryComp->GetCurrentWeapon()->FireEnd();
    }
}

// 조준 버튼 누름 처리 함수
void ABaseCharacter::AimButtonPressed()
{
    if (BattleComp)
    {
        BattleComp->StartAiming();  // BattleComponent에 조준 시작을 알림
        StartAiming();              // BaseCharacter 내부에서 추가적인 조준 로직 처리
    }
}

// 조준 버튼 뗌 처리 함수
void ABaseCharacter::AimButtonReleased()
{
    if (BattleComp)
    {
        BattleComp->StopAiming();   // BattleComponent에 조준 종료를 알림
        StopAiming();               // BaseCharacter 내부에서 추가적인 조준 해제 로직 처리
    }
}

// 조준 시작 로직
void ABaseCharacter::StartAiming()
{
    // 조준 시작 시 필요한 추가 로직 처리
}

// 조준 해제 로직
void ABaseCharacter::StopAiming()
{
    // 조준 해제 시 필요한 추가 로직 처리
}

// FOV 보간 처리 함수
void ABaseCharacter::InterpFOV(float DeltaTime)
{
    if (FollowCamera)
    {
        // 조준 상태에 따른 목표 FOV 결정
        float TargetFOV = BattleComp && BattleComp->bIsAiming ? AimedFOV : DefaultFOV;
        // 현재 FOV에서 목표 FOV로 보간
        float NewFOV = FMath::FInterpTo(FollowCamera->FieldOfView, TargetFOV, DeltaTime, FOVInterpSpeed);
        FollowCamera->SetFieldOfView(NewFOV);
    }
}
