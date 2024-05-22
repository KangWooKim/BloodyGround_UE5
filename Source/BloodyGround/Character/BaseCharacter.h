#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BloodyGround/Component/BattleComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "BaseCharacter.generated.h"

// 캐릭터 상태를 정의하는 열거형
UENUM(BlueprintType)
enum class ECharacterState : uint8
{
    None    UMETA(DisplayName = "None"),
    Attacking  UMETA(DisplayName = "Attacking"),
    HitReact  UMETA(DisplayName = "HitReact"),
    Death  UMETA(DisplayName = "Death"),
    MAX UMETA(DisplayName = "MAX")
};

// ABaseCharacter 클래스는 캐릭터의 기본 동작을 정의합니다.
UCLASS()
class BLOODYGROUND_API ABaseCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    // 생성자: 기본 설정을 초기화합니다.
    ABaseCharacter();

    // 캐릭터의 체력을 반환하는 함수
    FORCEINLINE float GetHealth() { return Health; }

protected:
    // 게임이 시작될 때 호출되는 함수
    virtual void BeginPlay() override;

    // 네트워크 복제를 설정하는 함수
    virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

    // 데미지를 처리하는 함수
    // @param DamageAmount 데미지 양
    // @param DamageEvent 데미지 이벤트
    // @param EventInstigator 데미지를 입힌 컨트롤러
    // @param DamageCauser 데미지를 입힌 액터
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

    // 캐릭터가 사망했을 때 처리하는 함수
    void HandleDeath();

    // 서버에서 발소리를 처리하는 함수
    UFUNCTION(Server, Reliable, BlueprintCallable)
        void ServerFootStep();

public:
    // 매 프레임마다 호출되는 함수
    virtual void Tick(float DeltaTime) override;

    // 플레이어 입력을 설정하는 함수
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // 조준을 시작하는 함수
    virtual void StartAiming();

    // 조준을 중지하는 함수
    virtual void StopAiming();

    // 인게임 HUD를 반환하는 함수
    FORCEINLINE class AInGameHUD* GetInGameHUD() { return PlayerHUD; }

    // 캐릭터의 체력 변수
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Health")
        float Health;

    // 전투 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
        UBattleComponent* BattleComp;

    // 캐릭터 상태를 반환하는 함수
    FORCEINLINE ECharacterState GetCharacterState() { return CharacterState; }

    // 인벤토리 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
        class UInventoryComponent* InventoryComp;

    // 서버에서 공격을 중지하는 함수
    UFUNCTION(Server, Reliable)
        void ServerStopAttack();

    // 멀티캐스트로 공격을 중지하는 함수
    UFUNCTION(NetMulticast, Reliable)
        void MulticastStopAttack();

    // 재장전을 처리하는 함수
    UFUNCTION()
        void Reload();

    // 서버에서 무기를 변경하는 함수
    UFUNCTION(Server, Reliable, WithValidation)
        void ServerChangeWeapon();

    // 조준 시 회전을 처리하는 함수
    UFUNCTION()
        void HandleAimingRotation();

    // 피격 반응이 끝났을 때 호출되는 함수
    UFUNCTION(BlueprintCallable)
        void HitReactionEnd();

    // 권총 블루프린트 클래스
    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
        TSubclassOf<class APistol> PistolBlueprint;

    // 머신건 블루프린트 클래스
    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
        TSubclassOf<class AMachineGun> MachineGunBlueprint;

    // 소음 발생 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Noise)
        class UPawnNoiseEmitterComponent* NoiseEmitter;

    // 리스폰을 처리하는 함수
    UFUNCTION(BlueprintCallable)
        void Respawn();

private:
    // 전/후 이동을 처리하는 함수
    void MoveForward(float Value);

    // 좌/우 이동을 처리하는 함수
    void MoveRight(float Value);

    // 좌우 회전을 처리하는 함수
    void Turn(float Value);

    // 상하 회전을 처리하는 함수
    void LookUp(float Value);

    // 점프를 처리하는 함수
    virtual void Jump() override;

    // 공격 버튼을 눌렀을 때 호출되는 함수
    void AttackButtonPressed();

    // 공격 버튼을 뗐을 때 호출되는 함수
    void AttackButtonReleased();

    // 조준 버튼을 눌렀을 때 호출되는 함수
    void AimButtonPressed();

    // 조준 버튼을 뗐을 때 호출되는 함수
    void AimButtonReleased();

    // FOV 보간을 처리하는 함수
    void InterpFOV(float DeltaTime);

    // 카메라 붐 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
        USpringArmComponent* CameraBoom;

    // 팔로우 카메라 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
        UCameraComponent* FollowCamera;

    // 무기를 변경하는 함수
    UFUNCTION()
        void ChangeWeapon();

    // 캐릭터 상태 변수
    UPROPERTY(Replicated)
        ECharacterState CharacterState;

    // 발소리 크기 변수
    UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
        float FootstepLoudness = 0.2f;

    // 플레이어 컨트롤러 변수
    UPROPERTY()
        class APlayerController* PlayerController;

    // 인게임 HUD 변수
    UPROPERTY()
        class AInGameHUD* PlayerHUD;

    // 기본 FOV 값
    float DefaultFOV;

    // 조준 시 FOV 값
    float AimedFOV;

    // FOV 변경 속도
    float FOVInterpSpeed;
};
