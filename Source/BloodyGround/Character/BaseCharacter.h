#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BloodyGround/Component/BattleComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "BaseCharacter.generated.h"

// ĳ���� ���¸� �����ϴ� ������
UENUM(BlueprintType)
enum class ECharacterState : uint8
{
    None    UMETA(DisplayName = "None"),
    Attacking  UMETA(DisplayName = "Attacking"),
    HitReact  UMETA(DisplayName = "HitReact"),
    Death  UMETA(DisplayName = "Death"),
    MAX UMETA(DisplayName = "MAX")
};

// ABaseCharacter Ŭ������ ĳ������ �⺻ ������ �����մϴ�.
UCLASS()
class BLOODYGROUND_API ABaseCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    // ������: �⺻ ������ �ʱ�ȭ�մϴ�.
    ABaseCharacter();

    // ĳ������ ü���� ��ȯ�ϴ� �Լ�
    FORCEINLINE float GetHealth() { return Health; }

protected:
    // ������ ���۵� �� ȣ��Ǵ� �Լ�
    virtual void BeginPlay() override;

    // ��Ʈ��ũ ������ �����ϴ� �Լ�
    virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

    // �������� ó���ϴ� �Լ�
    // @param DamageAmount ������ ��
    // @param DamageEvent ������ �̺�Ʈ
    // @param EventInstigator �������� ���� ��Ʈ�ѷ�
    // @param DamageCauser �������� ���� ����
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

    // ĳ���Ͱ� ������� �� ó���ϴ� �Լ�
    void HandleDeath();

    // �������� �߼Ҹ��� ó���ϴ� �Լ�
    UFUNCTION(Server, Reliable, BlueprintCallable)
        void ServerFootStep();

public:
    // �� �����Ӹ��� ȣ��Ǵ� �Լ�
    virtual void Tick(float DeltaTime) override;

    // �÷��̾� �Է��� �����ϴ� �Լ�
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // ������ �����ϴ� �Լ�
    virtual void StartAiming();

    // ������ �����ϴ� �Լ�
    virtual void StopAiming();

    // �ΰ��� HUD�� ��ȯ�ϴ� �Լ�
    FORCEINLINE class AInGameHUD* GetInGameHUD() { return PlayerHUD; }

    // ĳ������ ü�� ����
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Health")
        float Health;

    // ���� ������Ʈ
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
        UBattleComponent* BattleComp;

    // ĳ���� ���¸� ��ȯ�ϴ� �Լ�
    FORCEINLINE ECharacterState GetCharacterState() { return CharacterState; }

    // �κ��丮 ������Ʈ
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
        class UInventoryComponent* InventoryComp;

    // �������� ������ �����ϴ� �Լ�
    UFUNCTION(Server, Reliable)
        void ServerStopAttack();

    // ��Ƽĳ��Ʈ�� ������ �����ϴ� �Լ�
    UFUNCTION(NetMulticast, Reliable)
        void MulticastStopAttack();

    // �������� ó���ϴ� �Լ�
    UFUNCTION()
        void Reload();

    // �������� ���⸦ �����ϴ� �Լ�
    UFUNCTION(Server, Reliable, WithValidation)
        void ServerChangeWeapon();

    // ���� �� ȸ���� ó���ϴ� �Լ�
    UFUNCTION()
        void HandleAimingRotation();

    // �ǰ� ������ ������ �� ȣ��Ǵ� �Լ�
    UFUNCTION(BlueprintCallable)
        void HitReactionEnd();

    // ���� �������Ʈ Ŭ����
    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
        TSubclassOf<class APistol> PistolBlueprint;

    // �ӽŰ� �������Ʈ Ŭ����
    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
        TSubclassOf<class AMachineGun> MachineGunBlueprint;

    // ���� �߻� ������Ʈ
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Noise)
        class UPawnNoiseEmitterComponent* NoiseEmitter;

    // �������� ó���ϴ� �Լ�
    UFUNCTION(BlueprintCallable)
        void Respawn();

private:
    // ��/�� �̵��� ó���ϴ� �Լ�
    void MoveForward(float Value);

    // ��/�� �̵��� ó���ϴ� �Լ�
    void MoveRight(float Value);

    // �¿� ȸ���� ó���ϴ� �Լ�
    void Turn(float Value);

    // ���� ȸ���� ó���ϴ� �Լ�
    void LookUp(float Value);

    // ������ ó���ϴ� �Լ�
    virtual void Jump() override;

    // ���� ��ư�� ������ �� ȣ��Ǵ� �Լ�
    void AttackButtonPressed();

    // ���� ��ư�� ���� �� ȣ��Ǵ� �Լ�
    void AttackButtonReleased();

    // ���� ��ư�� ������ �� ȣ��Ǵ� �Լ�
    void AimButtonPressed();

    // ���� ��ư�� ���� �� ȣ��Ǵ� �Լ�
    void AimButtonReleased();

    // FOV ������ ó���ϴ� �Լ�
    void InterpFOV(float DeltaTime);

    // ī�޶� �� ������Ʈ
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
        USpringArmComponent* CameraBoom;

    // �ȷο� ī�޶� ������Ʈ
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
        UCameraComponent* FollowCamera;

    // ���⸦ �����ϴ� �Լ�
    UFUNCTION()
        void ChangeWeapon();

    // ĳ���� ���� ����
    UPROPERTY(Replicated)
        ECharacterState CharacterState;

    // �߼Ҹ� ũ�� ����
    UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
        float FootstepLoudness = 0.2f;

    // �÷��̾� ��Ʈ�ѷ� ����
    UPROPERTY()
        class APlayerController* PlayerController;

    // �ΰ��� HUD ����
    UPROPERTY()
        class AInGameHUD* PlayerHUD;

    // �⺻ FOV ��
    float DefaultFOV;

    // ���� �� FOV ��
    float AimedFOV;

    // FOV ���� �ӵ�
    float FOVInterpSpeed;
};
