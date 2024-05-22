#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Animation/AnimMontage.h"
#include "BaseWeapon.generated.h"

// 무기 타입을 정의하는 열거형
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
    None        UMETA(DisplayName = "None"),
    Pistol      UMETA(DisplayName = "Pistol"),
    MachineGun  UMETA(DisplayName = "MachineGun")
};

// 무기 상태를 정의하는 열거형
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
    // 생성자: 무기 클래스 초기화
    ABaseWeapon();

    // 클라이언트에서 발사 애니메이션을 재생하기 위한 함수
    UFUNCTION(NetMulticast, Reliable)
        void MulticastPlayFireAnimation();

    // 서버에서 발사 애니메이션을 재생하기 위한 함수
    UFUNCTION(Server, Reliable)
        void ServerPlayFireAnimation();

protected:
    // 액터가 시작될 때 호출되는 함수
    virtual void BeginPlay() override;

    // 네트워크 복제 속성을 설정하는 함수
    virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

    // 무기 메시 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
        USkeletalMeshComponent* SkeletalMeshComponent;

    // 소유한 캐릭터에 대한 참조
    UPROPERTY()
        class ABaseCharacter* Character;

    // 소음 방출 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Noise)
        class UPawnNoiseEmitterComponent* NoiseEmitter;

    // 무기가 발사 가능한지 여부를 확인하는 함수
    virtual bool CanFire();

public:
    // 무기를 교체하는 함수
    UFUNCTION()
        virtual void ChangeWeapon();

    // 매 프레임마다 호출되는 틱 함수
    virtual void Tick(float DeltaTime) override;

    // 무기 발사 함수
    virtual void Fire();

    // 탄창을 재장전하는 함수
    UFUNCTION(BlueprintCallable)
        void Reload();

    // 실제 재장전을 수행하는 함수
    UFUNCTION()
        virtual void PerformReload();

    // 서버에서 재장전을 처리하기 위한 함수
    UFUNCTION(Server, Reliable)
        void ServerReload();

    // 클라이언트에서 재장전 애니메이션을 재생하기 위한 함수
    UFUNCTION(NetMulticast, Reliable)
        void MulticastPlayReloadAnimation();

    // 현재 무기 타입을 반환하는 함수
    virtual EWeaponType GetCurrentWeaponType();

    // 탄창 내 현재 탄알 수
    UPROPERTY(Replicated, EditDefaultsOnly, Category = "Weapon")
        int32 CurrentAmmo;

    // 탄창 최대 용량
    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
        int32 Capacity;

    // 데미지 저장 변수
    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
        int32 Damage;

    // 발사 종료 함수
    UFUNCTION(BlueprintCallable)
        virtual void FireEnd();

    // 재장전 종료 함수
    UFUNCTION(BlueprintCallable)
        void ReloadEnd();

    // 무기 메시 컴포넌트를 반환하는 인라인 함수
    FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() { return SkeletalMeshComponent; }

    // 현재 무기 상태를 반환하는 인라인 함수
    FORCEINLINE EWeaponState GetWeaponState() { return WeaponState; }

    // 현재 무기 상태
    UPROPERTY(Replicated)
        EWeaponState WeaponState;

    // 발사 애니메이션 몽타주
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
        class UAnimMontage* FireAnimation;

    // 재장전 애니메이션 몽타주
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
        class UAnimMontage* ReloadAnimation;

    // 소음을 발생시키는 함수
    UFUNCTION(Server, Reliable)
        void WeaponNoise(float Loudness);

protected:
    // 서버에서 클라이언트의 히트 확인 요청을 처리하는 함수
    UFUNCTION(Server, Reliable, WithValidation)
        void ServerCheckHit(FHitResult HitResult, float HitTime, FVector StartLocation, FVector EndDirection);

private:
    // 히트 임계값
    UPROPERTY()
        float HitThreshold;
};
