# BaseCharacter
ABaseCharacter()
클래스의 생성자로서, 캐릭터의 초기 설정을 담당합니다. 체력을 초기화하고, 카메라와 관련된 컴포넌트들을 설정합니다. 또한 인벤토리 컴포넌트와 소음 발생 컴포넌트를 추가하고, 네트워크 복제를 활성화하여 멀티플레이 게임에서도 캐릭터의 정보가 동기화될 수 있도록 합니다.

BeginPlay()
게임이 시작할 때 호출되며, 캐릭터 상태, 플레이어 컨트롤러, HUD를 초기화하고, 기본 무기를 설정합니다. 특정 조건 하에 무기를 생성하고 인벤토리에 추가하며, 플레이어 HUD에 체력 정보를 업데이트합니다.

Tick(float DeltaTime)
프레임마다 호출되어 캐릭터의 조준 회전과 FOV 보간을 처리합니다. 캐릭터의 시점(Field of View)이 조준 상태에 따라 부드럽게 변경됩니다.

HandleAimingRotation()
조준 시 캐릭터의 회전을 조절하여 조준 방향을 컨트롤러의 회전 방향과 일치시킵니다.

HitReactionEnd()
피격 반응이 끝났을 때 캐릭터 상태를 초기화합니다.

TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
캐릭터가 피해를 받았을 때 호출되며, 체력을 감소시키고, 사망 처리를 진행합니다. 또한 HUD에 체력 정보를 업데이트합니다.

HandleDeath()
캐릭터의 사망을 처리하며, 모든 입력을 비활성화하고, 일정 시간 후에 캐릭터를 리스폰합니다.

Respawn()
사망한 캐릭터를 게임 모드를 통해 다시 스폰합니다.

SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
입력 컴포넌트를 설정하여, 이동, 점프, 리로드, 무기 변경, 공격, 조준 등의 플레이어 입력을 처리합니다.

Reload()
현재 선택된 무기를 재장전합니다.

ChangeWeapon()
인벤토리 내의 다음 무기로 변경합니다. 네트워크 상황에 따라 서버에서 무기 변경을 요청할 수도 있습니다.

MoveForward(float Value) 및 MoveRight(float Value)
각각 전/후진 및 좌/우 이동을 처리합니다. 입력된 값에 따라 캐릭터의 이동 방향을 결정하고 이동을 실행합니다.

Turn(float Value) 및 LookUp(float Value)
캐릭터의 회전(Yaw)과 상하(Pitch)을 처리합니다.

Jump()
점프 기능을 실행합니다. 점프 시 발생하는 소리를 네트워크를 통해 다른 플레이어에게도 전달합니다.

AttackButtonPressed() 및 AttackButtonReleased()
공격 버튼이 눌렸을 때와 떼졌을 때의 동작을 처리합니다. 공격 시작과 종료 로직을 실행합니다.

AimButtonPressed() 및 AimButtonReleased()
조준 버튼이 눌렸을 때와 떼졌을 때의 동작을 처리합니다. 조준 시작과 종료를 BattleComponent를 통해 처리합니다.

#BaseCharacterAnimInstance

NativeUpdateAnimation(float DeltaSeconds)
이 메서드는 애니메이션 인스턴스의 주요 업데이트 사이클에서 호출되어, 캐릭터의 동작과 상태에 따라 애니메이션 변수들을 실시간으로 갱신합니다. 캐릭터의 속도와 방향성을 기반으로 전방 및 좌우 이동 속도를 계산하고, 이 데이터를 애니메이션 블루프린트의 변수로 전달합니다. 또한 캐릭터의 점프 상태(공중에 있는지 여부)와 조준 상태를 갱신하고, 현재 들고 있는 무기의 종류와 해당 무기 메쉬의 소켓 위치를 애니메이션 블루프린트로 전달하여 캐릭터의 손 위치 등을 정밀하게 조정할 수 있도록 합니다. 이 메서드는 캐릭터의 움직임과 애니메이션 사이의 일관성을 유지하는 데 중요한 역할을 합니다.
