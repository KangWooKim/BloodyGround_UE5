# BloodyGround (UE5 Third-Person Shooter)

BloodyGround는 언리얼 엔진 5로 만든 3인칭 멀티플레이 슈팅 프로젝트입니다. "빠른 조준 감각"과 "신뢰할 수 있는 동기화"를 목표로 캐릭터, 전투, AI, 세션 플로우를 컴포넌트화했습니다.

## 한눈에 보기
- **엔진 & 타겟:** Unreal Engine 5 · 멀티플레이 TPS 프로토타입
- **핵심:** 기능은 컴포넌트/서브시스템으로 분리, 네트워크 판정은 항상 서버 권위 기반, HUD는 상태와 즉시 동기화
- **빠르게 확인:** 아래 각 섹션은 실제 소스 경로와 대응되는 스니펫을 포함하므로 파일을 열지 않아도 설계 의도를 파악할 수 있습니다.

---

## 1. 캐릭터 입력 루프
`ABaseCharacter`는 이동·조준·무기 관리 같은 입력을 책임지고, 나머지는 컴포넌트에게 위임합니다. 생성자에서 어떤 컴포넌트가 묶여 있는지 바로 확인할 수 있습니다.

```cpp
// Source/BloodyGround/Character/BaseCharacter.cpp
ABaseCharacter::ABaseCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    Health = 100.0f;
    SetReplicates(true);
    SetReplicateMovement(true);

    BattleComp = CreateDefaultSubobject<UBattleComponent>(TEXT("BattleComponent"));
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    InventoryComp = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
    NoiseEmitter = CreateDefaultSubobject<UPawnNoiseEmitterComponent>(TEXT("Noise Emitter"));
}
```

- **리스폰 절차:** `ABloodyGroundGameModeBase::RespawnPlayer`가 서버에서 PlayerStart를 고르고 새 캐릭터를 소환합니다.
- **HUD 초기화:** `BeginPlay`에 HUD 포인터를 확보하고 체력·탄약 UI를 동기화합니다.

```cpp
// Source/BloodyGround/BloodyGroundGameModeBase.cpp
void ABloodyGroundGameModeBase::RespawnPlayer(APlayerController* PC)
{
    if (!PC || !HasAuthority()) return;

    TArray<AActor*> PlayerStarts;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);
    const int32 Index = FMath::RandRange(0, PlayerStarts.Num() - 1);

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    ABaseCharacter* NewCharacter = GetWorld()->SpawnActor<ABaseCharacter>(DefaultPawnClass,
        PlayerStarts[Index]->GetActorLocation(), PlayerStarts[Index]->GetActorRotation(), SpawnParams);

    if (NewCharacter)
    {
        PC->Possess(NewCharacter);
        if (AInGameHUD* PlayerHUD = Cast<AInGameHUD>(PC->GetHUD()))
        {
            PlayerHUD->UpdateHealth(1.f);
            PlayerHUD->DeleteRespawnText();
            PlayerHUD->UpdateAmmo(10, 50);
        }
    }
}
```

---

## 2. 전투 & 인벤토리 복제
전투 상태는 `UBattleComponent`, 무기 목록과 탄약은 `UInventoryComponent`가 관리합니다. 두 컴포넌트 모두 Replicate 설정이 기본값이라 네트워크 동기화를 직접 확인할 수 있습니다.

```cpp
// Source/BloodyGround/Component/BattleComponent.cpp
UBattleComponent::UBattleComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    bIsAiming = false;
    SetIsReplicatedByDefault(true);
}

void UBattleComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UBattleComponent, bIsAiming);
}
```

```cpp
// Source/BloodyGround/Component/InventoryComponent.cpp
UInventoryComponent::UInventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
}

void UInventoryComponent::ChangeWeapon()
{
    if (Weapons.Num() <= 1) return;
    const int32 CurrentIndex = Weapons.IndexOfByKey(CurrentWeapon);
    const int32 NextIndex = (CurrentIndex + 1) % Weapons.Num();

    if (CurrentWeapon)
    {
        CurrentWeapon->WeaponState = EWeaponState::None;
        CurrentWeapon->SetActorHiddenInGame(true);
    }

    CurrentWeapon = Weapons[NextIndex];
    CurrentWeapon->SetActorHiddenInGame(false);
    if (CurrentWeapon) CurrentWeapon->ChangeWeapon();
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UInventoryComponent, Weapons);
    DOREPLIFETIME(UInventoryComponent, CurrentWeapon);
    DOREPLIFETIME(UInventoryComponent, PistolAmmo);
    DOREPLIFETIME(UInventoryComponent, MachineGunAmmo);
}
```

---

## 3. 무기 & 서버 판정
`ABaseWeapon`은 발사 요청을 서버 권위로 수렴시키고, `UServerLocationComponent`가 저장한 위치 히스토리를 통해 히트 판정을 재검증합니다.

```cpp
// Source/BloodyGround/Weapon/BaseWeapon.cpp
void ABaseWeapon::Fire()
{
    if (Character->HasAuthority())
    {
        MulticastPlayFireAnimation();
        WeaponNoise(1.f);
    }
    else if (Character->IsLocallyControlled())
    {
        ServerPlayFireAnimation();
    }

    WeaponState = EWeaponState::Fire;
    const FVector Start = SkeletalMeshComponent->GetSocketLocation(TEXT("MuzzleFlash"));
    const FVector End = Start + Character->GetControlRotation().Vector() * 10000.0f;

    FHitResult HitResult;
    if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, QueryParams))
    {
        if (ABaseZombie* HitZombie = Cast<ABaseZombie>(HitResult.GetActor()))
        {
            if (Character->HasAuthority())
            {
                const FLocationTimeData Recorded = HitZombie->GetServerLocationComponent()->GetLocationData();
                const FHitResultData Verified = HitZombie->GetServerLocationComponent()->ServerTrace(Start, End, Recorded);
                if (Verified.bHitBody)
                {
                    HitZombie->TakeShot(Verified, Damage);
                    UGameplayStatics::ApplyDamage(HitZombie, Damage, Character->GetController(), this, UDamageType::StaticClass());
                }
            }
            else if (Character->IsLocallyControlled())
            {
                ServerCheckHit(HitResult, GetWorld()->GetTimeSeconds(), Start, Character->GetControlRotation().Vector());
            }
        }
    }
}
```

```cpp
// Source/BloodyGround/Component/ServerLocationComponent.cpp
void UServerLocationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (GetOwner()->GetLocalRole() == ROLE_Authority && GetWorld()->TimeSince(LastRecordTime) >= RecordInterval)
    {
        RecordLocation();
        CleanupOldData();
    }
}

void UServerLocationComponent::RecordLocation()
{
    const float Now = GetWorld()->GetTimeSeconds();
    LocationHistory.Add(FLocationTimeData(
        FComponentLocationData(CapsuleComponent->GetComponentLocation(), CapsuleComponent->GetComponentRotation()),
        FComponentLocationData(LeftLegComponent->GetComponentLocation(), LeftLegComponent->GetComponentRotation()),
        FComponentLocationData(RightLegComponent->GetComponentLocation(), RightLegComponent->GetComponentRotation()),
        Now));
    LastRecordTime = Now;
}
```

---

## 4. AI 상태 머신
좀비 AI는 `ABaseZombie`가 순찰/추적/공격/수면 상태를 복제하고, 엘리트 좀비는 추가적으로 다리 부상 루프를 제공합니다.

```cpp
// Source/BloodyGround/Enemy/BaseZombie.cpp
ABaseZombie::ABaseZombie()
{
    PrimaryActorTick.bCanEverTick = true;
    SetReplicates(true);
    SetReplicateMovement(true);

    PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
    PawnSensingComp->SightRadius = 5000.f;
    PawnSensingComp->OnSeePawn.AddDynamic(this, &ABaseZombie::OnSeePlayer);
    PawnSensingComp->OnHearNoise.AddDynamic(this, &ABaseZombie::OnHearNoise);

    ZombieState = EZombieState::None;
    PatrolRadius = 1000.0f;
    TimeToSleep = 5.0f;
    Health = 50.0f;

    ServerLocationComp = CreateDefaultSubobject<UServerLocationComponent>(TEXT("ServerLocationComp"));
    ServerLocationComp->GetBodyComponent()->SetupAttachment(GetMesh(), FName("Spine"));
}

void ABaseZombie::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (ABloodyGroundGameState* GameState = GetWorld()->GetGameState<ABloodyGroundGameState>())
    {
        if (GameState->DayNightCycle == EDayNightCycle::Night && ZombieState != EZombieState::Sleep && !CurrentTarget)
        {
            if (!GetWorldTimerManager().IsTimerActive(SleepTimerHandle))
            {
                GetWorldTimerManager().SetTimer(SleepTimerHandle, this, &ABaseZombie::GoBackToSleep, TimeToSleep, false);
            }
        }
    }

    if (AAIController* AIController = Cast<AAIController>(GetController()))
    {
        if (CurrentTarget)
        {
            if (FVector::Dist(GetActorLocation(), CurrentTarget->GetActorLocation()) <= AttackRange)
            {
                const float CurrentTime = GetWorld()->GetTimeSeconds();
                if (CurrentTime - LastAttackTime >= AttackCooldown)
                {
                    Attack(CurrentTarget);
                    LastAttackTime = CurrentTime;
                }
            }
            else if (!IsTargetInSight(CurrentTarget))
            {
                CurrentTarget = nullptr;
                RestartPatrol();
            }
            else
            {
                ZombieState = EZombieState::None;
                AIController->MoveToActor(CurrentTarget);
            }
        }
        else if (AIController->GetPathFollowingComponent()->DidMoveReachGoal())
        {
            PatrolPoint = GetRandomPatrolPoint();
            AIController->MoveToLocation(PatrolPoint);
        }
    }
}
```

```cpp
// Source/BloodyGround/Enemy/EliteZombie.cpp
float AEliteZombie::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
    AController* EventInstigator, AActor* DamageCauser)
{
    if (DamageCauser && DamageCauser->IsA(ABaseZombie::StaticClass())) return 0.0f;
    if (ZombieState == EZombieState::Death) return 0.0f;

    const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    Health -= ActualDamage;

    if (Health <= 0)
    {
        HandleDeath();
        return ActualDamage;
    }

    if (ZombieState != EZombieState::HitReact && ZombieInjuryState != EZombieInjuryState::Injured)
    {
        GetCharacterMovement()->SetMovementMode(MOVE_None);
        ZombieState = EZombieState::HitReact;
        GetWorldTimerManager().SetTimer(TimerHandle_HitReactEnd, this, &AEliteZombie::HitReactEnd, 1.0f, false);
    }

    return ActualDamage;
}
```

---

## 5. 월드 상태 & 세션 흐름
낮/밤 전환과 RTT 모니터링은 게임 상태와 플레이어 컨트롤러에서 주기적으로 처리합니다.

```cpp
// Source/BloodyGround/GameState/BloodyGroundGameState.cpp
void ABloodyGroundGameState::BeginPlay()
{
    Super::BeginPlay();
    if (HasAuthority())
    {
        GetWorldTimerManager().SetTimer(DayNightCycleTimerHandle,
            this, &ABloodyGroundGameState::ToggleDayNightCycle, 60.0f, true);
    }
}

void ABloodyGroundGameState::OnRep_DayNightChanged()
{
    const bool bIsDay = DayNightCycle == EDayNightCycle::Day;
    AdjustSunlight(bIsDay);
}

void ABloodyGroundGameState::AdjustSunlight(bool bIsDay)
{
    TargetSunlightIntensity = bIsDay ? 3.0f : 0.1f;
    if (!Sunlight)
    {
        Sunlight = Cast<ADirectionalLight>(UGameplayStatics::GetActorOfClass(
            GetWorld(), ADirectionalLight::StaticClass()));
    }
    if (Sunlight)
    {
        CurrentSunlightIntensity = Sunlight->GetLightComponent()->Intensity;
        GetWorldTimerManager().SetTimer(SunlightAdjustmentTimerHandle,
            this, &ABloodyGroundGameState::UpdateSunlight, 0.1f, true);
    }
}
```

```cpp
// Source/BloodyGround/PlayerController/BloodyGroundPlayerController.cpp
void ABloodyGroundPlayerController::PlayerTick(float DeltaTime)
{
    Super::PlayerTick(DeltaTime);

    if (GetWorld()->TimeSince(LastRTTUpdateTime) > 1.0f && PlayerState && IsLocalController())
    {
        RoundTripTime = PlayerState->GetPing() * 0.001f;
        LastRTTUpdateTime = GetWorld()->GetTimeSeconds();
    }
}
```

---

## 6. HUD & 위젯 연동
HUD는 전적으로 `UInGameWidget`에 위임합니다. 아래 스니펫은 UI와 연동되는 함수가 어떻게 연결되는지 보여줍니다.

```cpp
// Source/BloodyGround/HUD/InGameHUD.cpp
void AInGameHUD::BeginPlay()
{
    Super::BeginPlay();

    if (UWorld* World = GetWorld())
    {
        if (APlayerController* Controller = World->GetFirstPlayerController())
        {
            if (InGameWidgetClass)
            {
                InGameWidget = CreateWidget<UInGameWidget>(Controller, InGameWidgetClass);
                InGameWidget->AddToViewport();
                UpdateAmmo(10, 50);
            }
        }
    }
}

void AInGameHUD::UpdateHealth(float HealthPercentage)
{
    if (InGameWidget)
    {
        InGameWidget->UpdateHealthBar(HealthPercentage);
    }
}
```

```cpp
// Source/BloodyGround/HUD/InGameWidget.cpp
void UInGameWidget::UpdateAmmoCount(int32 AmmoInMagazine, int32 TotalAmmo)
{
    if (AmmoText)
    {
        const FString AmmoDisplay = FString::Printf(TEXT("%d / %d"), AmmoInMagazine, TotalAmmo);
        AmmoText->SetText(FText::FromString(AmmoDisplay));
    }
}
```

---

## 7. 세션 UI & 온라인 서브시스템
플레이어는 `UMenu` 위젯을 통해 세션을 만들거나 검색하고, `UMultiplayerSessionsSubsystem`이 언리얼 온라인 서브시스템과 직접 통신합니다.

```cpp
// MultiplayerSessions/Private/Menu.cpp
void UMenu::NativeConstruct()
{
    Super::NativeConstruct();

    if (CreateButton) CreateButton->OnClicked.AddDynamic(this, &UMenu::OnCreateButtonClicked);
    if (HostButton)   HostButton->OnClicked.AddDynamic(this, &UMenu::OnHostButtonClicked);
    if (JoinButton)   JoinButton->OnClicked.AddDynamic(this, &UMenu::OnJoinButtonClicked);
    if (PlayButton)   PlayButton->OnClicked.AddDynamic(this, &UMenu::OnPlayButtonClicked);

    if (MapOption1) MapOption1->OnCheckStateChanged.AddDynamic(this, &UMenu::OnMapOptionChanged);
    if (MapOption2) MapOption2->OnCheckStateChanged.AddDynamic(this, &UMenu::OnMapOptionChanged);

    SetJoinMenuVisibility(false);
    SetHostMenuVisibility(false);
}
```

```cpp
// MultiplayerSessions/Private/MultiplayerSessionsSubsystem.cpp
UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem()
    : CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete))
    , FindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionsComplete))
    , JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete))
    , DestroySessionCompleteDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnDestroySessionComplete))
    , StartSessionCompleteDelegate(FOnStartSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnStartSessionComplete))
{
    if (IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get())
    {
        SessionInterface = Subsystem->GetSessionInterface();
    }
}

void UMultiplayerSessionsSubsystem::CreateSession(int32 NumPublicConnections, FString MatchType,
    FString RoomName, FString RoomPassword)
{
    if (!SessionInterface.IsValid()) return;

    LastSessionSettings = MakeShareable(new FOnlineSessionSettings());
    LastSessionSettings->bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";
    LastSessionSettings->NumPublicConnections = NumPublicConnections;
    LastSessionSettings->bAllowJoinInProgress = true;
    LastSessionSettings->bAllowJoinViaPresence = true;
    LastSessionSettings->bShouldAdvertise = true;
    LastSessionSettings->bUsesPresence = true;
    LastSessionSettings->Set(FName("CurrentMap"), MatchType, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
    LastSessionSettings->Set(FName("RoomName"), RoomName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
    LastSessionSettings->Set(FName("RoomPassword"), RoomPassword, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

    const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
    if (!SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *LastSessionSettings))
    {
        SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
        MultiplayerOnCreateSessionComplete.Broadcast(false);
    }
}
```

---

## 8. 온보딩 팁
- **폴더별 README**: `Source/BloodyGround/**/ReadMe.md` 파일에는 각 클래스의 역할과 공개 메서드가 요약되어 있습니다.
