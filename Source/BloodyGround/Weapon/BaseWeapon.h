#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Animation/AnimMontage.h"
#include "BaseWeapon.generated.h"

// ���� Ÿ���� �����ϴ� ������
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
    None        UMETA(DisplayName = "None"),
    Pistol      UMETA(DisplayName = "Pistol"),
    MachineGun  UMETA(DisplayName = "MachineGun")
};

// ���� ���¸� �����ϴ� ������
UENUM(BlueprintType)
enum class EWeaponState : uint8
{
    None        UMETA(DisplayName = "None"),
    Fire        UMETA(DisplayName = "Fire"),
    Reload      UMETA(DisplayName = "Reload")
};

UCLASS()
class BLOODYGROUND_API ABaseWeapon : public APawn
{
    GENERATED_BODY()

public:
    // ������: ���� Ŭ���� �ʱ�ȭ
    ABaseWeapon();

    // Ŭ���̾�Ʈ���� �߻� �ִϸ��̼��� ����ϱ� ���� �Լ�
    UFUNCTION(NetMulticast, Reliable)
        void MulticastPlayFireAnimation();

    // �������� �߻� �ִϸ��̼��� ����ϱ� ���� �Լ�
    UFUNCTION(Server, Reliable)
        void ServerPlayFireAnimation();

protected:
    // ���Ͱ� ���۵� �� ȣ��Ǵ� �Լ�
    virtual void BeginPlay() override;

    // ��Ʈ��ũ ���� �Ӽ��� �����ϴ� �Լ�
    virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

    // ���� �޽� ������Ʈ
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
        USkeletalMeshComponent* SkeletalMeshComponent;

    // ������ ĳ���Ϳ� ���� ����
    UPROPERTY()
        class ABaseCharacter* Character;

    // ���� ���� ������Ʈ
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Noise)
        class UPawnNoiseEmitterComponent* NoiseEmitter;

    // ���Ⱑ �߻� �������� ���θ� Ȯ���ϴ� �Լ�
    virtual bool CanFire();

public:
    // ���⸦ ��ü�ϴ� �Լ�
    UFUNCTION()
        virtual void ChangeWeapon();

    // �� �����Ӹ��� ȣ��Ǵ� ƽ �Լ�
    virtual void Tick(float DeltaTime) override;

    // ���� �߻� �Լ�
    virtual void Fire();

    // źâ�� �������ϴ� �Լ�
    UFUNCTION(BlueprintCallable)
        void Reload();

    // ���� �������� �����ϴ� �Լ�
    UFUNCTION()
        virtual void PerformReload();

    // �������� �������� ó���ϱ� ���� �Լ�
    UFUNCTION(Server, Reliable)
        void ServerReload();

    // Ŭ���̾�Ʈ���� ������ �ִϸ��̼��� ����ϱ� ���� �Լ�
    UFUNCTION(NetMulticast, Reliable)
        void MulticastPlayReloadAnimation();

    // ���� ���� Ÿ���� ��ȯ�ϴ� �Լ�
    virtual EWeaponType GetCurrentWeaponType();

    // źâ �� ���� ź�� ��
    UPROPERTY(Replicated, EditDefaultsOnly, Category = "Weapon")
        int32 CurrentAmmo;

    // źâ �ִ� �뷮
    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
        int32 Capacity;

    // ������ ���� ����
    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
        int32 Damage;

    // �߻� ���� �Լ�
    UFUNCTION(BlueprintCallable)
        virtual void FireEnd();

    // ������ ���� �Լ�
    UFUNCTION(BlueprintCallable)
        void ReloadEnd();

    // ���� �޽� ������Ʈ�� ��ȯ�ϴ� �ζ��� �Լ�
    FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() { return SkeletalMeshComponent; }

    // ���� ���� ���¸� ��ȯ�ϴ� �ζ��� �Լ�
    FORCEINLINE EWeaponState GetWeaponState() { return WeaponState; }

    // ���� ���� ����
    UPROPERTY(Replicated)
        EWeaponState WeaponState;

    // �߻� �ִϸ��̼� ��Ÿ��
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
        class UAnimMontage* FireAnimation;

    // ������ �ִϸ��̼� ��Ÿ��
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
        class UAnimMontage* ReloadAnimation;

    // ������ �߻���Ű�� �Լ�
    UFUNCTION(Server, Reliable)
        void WeaponNoise(float Loudness);

protected:
    // �������� Ŭ���̾�Ʈ�� ��Ʈ Ȯ�� ��û�� ó���ϴ� �Լ�
    UFUNCTION(Server, Reliable, WithValidation)
        void ServerCheckHit(FHitResult HitResult, float HitTime, FVector StartLocation, FVector EndDirection);

private:
    // ��Ʈ �Ӱ谪
    UPROPERTY()
        float HitThreshold;
};
