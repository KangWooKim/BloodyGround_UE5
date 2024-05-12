# BaseZombie

### `ABaseZombie()`
이 생성자는 `ABaseZombie` 클래스의 초기 설정을 담당합니다. 캐릭터의 기본 틱, 네트워크 복제, 캐릭터 움직임 설정을 초기화하고, 감지 컴포넌트와 서버 위치 컴포넌트를 추가합니다. 또한, 감지 컴포넌트에 시야와 청각 관련 설정을 초기화하며 플레이어를 보거나 들었을 때 호출될 함수를 등록합니다.

### `BeginPlay()`
게임 시작 시 호출되며, 좀비의 상태를 초기화하고, 최대 걷기 속도와 순찰 지점을 설정합니다.

### `GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const`
네트워크 복제를 위해 복제되어야 할 변수들을 설정합니다. 좀비 상태, 현재 타겟, 순찰 지점, 건강 상태 등이 포함됩니다.

### `PostInitializeComponents()`
초기 컴포넌트 설정 후 추가적인 설정을 수행합니다. 주로 이벤트 리스너들을 다시 바인딩합니다.

### `Tick(float DeltaTime)`
매 프레임 호출되며, 게임 상태에 따라 좀비의 행동을 업데이트합니다. 밤에는 자동으로 잠에 들도록 설정하고, 타겟의 상태에 따라 공격하거나 순찰을 재개합니다.

### `GetRandomPatrolPoint()`
랜덤한 순찰 지점을 반환합니다. 네비게이션 시스템을 사용하여 주변의 탐색 가능한 지점 중 하나를 선택합니다.

### `OnSeePlayer(APawn* Pawn)`
플레이어를 시각적으로 감지했을 때 호출됩니다. 좀비의 상태와 플레이어의 건강을 확인한 후에, 공격 대상으로 설정합니다.

### `OnHearNoise(APawn* NoiseInstigator, const FVector& Location, float Volume)`
소리를 들었을 때 호출됩니다. 소리의 원인이 된 객체가 좀비 상태나 건강 상태에 따라 반응을 달리 합니다.

### `WakeUp()`
좀비를 깨우는 함수로, 주로 좀비가 잠에서 깨어날 때 사용됩니다.

### `GoBackToSleep()`
좀비를 다시 재우는 함수로, 특정 타이머 후에 호출되어 좀비의 상태를 잠자는 상태로 변경합니다.

### `Attack(APawn* Target)`
주어진 대상을 공격하는 함수입니다. 좀비의 상태를 공격 상태로 변경하고, 타겟에 대한 공격을 시작합니다.

### `ApplyDamageToTarget()`
현재 타겟에 대해 실제 데미지를 적용합니다. 공격 범위 안에 타겟이 있는지 확인한 후 데미지를 적용합니다.

### `TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)`
좀비가 피해를 입었을 때 호출되며, 입은 피해만큼 건강을 감소시키고 사망 처리를 합니다. 필요에 따라 좀비의 상태를 변경하고, 공격자를 새로운 타겟으로 설정할 수 있습니다.

### `HitReactEnd()`
피해 반응이 끝난 후 호출되며, 좀비의 움직임을 다시 활성화하고 기본 상태로 돌아갑니다.

### `HandleDeath()`
좀비의 사망을 처리하는 함수로,

 모든 활동을 중단하고 사망 상태로 전환합니다.

### `DeathEnd()`
좀비의 사망 애니메이션이 끝난 후 호출되어, 좀비 객체를 제거합니다.

### `RestartPatrol()`
순찰을 다시 시작하는 함수로, 새로운 순찰 지점을 얻고 해당 위치로 이동을 시작합니다.

### `IsTargetInSight(APawn* Target)`
주어진 대상이 시야 범위 내에 있는지 확인합니다. 최근에 대상을 본 시간과 현재 시간을 비교하여 결정합니다.

### `TakeShot(FHitResultData HitResult, float WeaponDamage)`
좀비가 총에 맞았을 때 호출되는 함수로, 피해 처리와 관련된 로직을 수행합니다.

# EliteZombie

### `AEliteZombie()`
이 생성자는 `AEliteZombie` 클래스의 초기 설정을 담당합니다. 네트워크 복제, 캐릭터의 기본 건강 상태, 공격 범위, 데미지, 공격 쿨다운, 다리 데미지 누적 값을 설정합니다. 이 좀비는 일반 좀비보다 강력한 특성을 가지고 있습니다.

### `BeginPlay()`
게임 시작 시 호출되며, 좀비의 최대 걷기 속도를 설정합니다.

### `TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)`
좀비가 피해를 받았을 때 호출되며, 받은 피해만큼 건강을 감소시키고 사망 처리를 합니다. 추가적으로, 특정 상태에서는 피해를 무시하며, 일정 피해 이상을 받으면 'Hit reaction' 상태로 전환합니다.

### `GetDown()`
좀비가 다리를 다쳐 넘어질 때 호출되는 함수입니다. 이 상태에서는 좀비의 이동 속도가 크게 감소합니다.

### `DownEnd()`
좀비가 넘어진 상태에서 일어나는 것을 처리하는 함수입니다. 일정 시간 후에 다시 일어서는 `StandUp` 메서드를 호출합니다.

### `StandUp()`
좀비가 다시 일어서는 함수로, 이동 모드를 변경하고 상태를 업데이트합니다.

### `StandUpEnd()`
좀비가 완전히 회복되어 일상적인 활동을 재개할 수 있게 됩니다. 이동 속도를 원래대로 복구하고 상태를 초기화합니다.

### `Attack(APawn* Target)`
주어진 대상을 공격하는 함수입니다. 이는 상위 클래스의 공격 로직을 확장하여 특수한 좀비 공격 로직을 추가할 수 있습니다.

### `ApplyDamageToTarget()`
현재 타겟에 대해 실제 데미지를 적용합니다. 상위 클래스의 데미지 적용 로직을 사용합니다.

### `TakeShot(FHitResultData HitResult, float WeaponDamage)`
좀비가 총에 맞았을 때 호출되는 함수로, 피해 처리와 관련된 로직을 수행합니다. 특히 다리에 피해가 누적되면 `GetDown()` 메서드를 호출하여 좀비를 넘어뜨립니다.

### `GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const`
네트워크 복제를 위해 복제되어야 할 변수들을 설정합니다. 좀비의 부상 상태와 다리 데미지 누적 값이 포함됩니다.
