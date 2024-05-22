#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundCue.h"
#include "KeyItem.generated.h"

UCLASS()
class BLOODYGROUND_API AKeyItem : public AActor
{
    GENERATED_BODY()

public:
    // AKeyItem Ŭ������ ������
    AKeyItem();

protected:
    // ������ ���۵� �� ȣ��Ǵ� �Լ�
    virtual void BeginPlay() override;

    // �� �����Ӹ��� ȣ��Ǵ� �Լ�
    virtual void Tick(float DeltaTime) override;

    // SphereComponent�� ��� ������ ����
    UPROPERTY(VisibleAnywhere, Category = "Components")
        class USphereComponent* SphereComponent;

    // ������ �̺�Ʈ�� �߻��� �� ȣ��Ǵ� �Լ�
    UFUNCTION()
        void OnOverlapBegin(
            class UPrimitiveComponent* OverlappedComponent,
            class AActor* OtherActor,
            class UPrimitiveComponent* OtherComp,
            int32 OtherBodyIndex,
            bool bFromSweep,
            const FHitResult& SweepResult
        );

    // ��� Ŭ���̾�Ʈ���� ���带 ����ϰ� �������� �����ϴ� �Լ�
    UFUNCTION(NetMulticast, Reliable)
        void Multicast_PlaySoundAndDestroy();

    // ��� Ŭ���̾�Ʈ�� ���� �޽����� ǥ���ϰ� ���� �޴��� �̵��ϴ� �Լ�
    UFUNCTION(NetMulticast, Reliable)
        void Multicast_ShowWinnerAndReturnToMainMenu(const FString& WinnerName);

    // ���� �޴��� �̵��ϴ� �Լ�
    UFUNCTION()
        void ReturnToMainMenu();

    // StaticMeshComponent�� ��� ������ ����
    UPROPERTY(VisibleAnywhere, Category = "Components")
        class UStaticMeshComponent* Mesh;

    // ���� ť�� ��� ������ ����
    UPROPERTY(EditAnywhere, Category = "Effects")
        class USoundCue* PickupSound;

private:
    // �������� ���� ��ġ�� �����ϴ� ����
    FVector StartLocation;

    // �ִϸ��̼� ���� �ð��� �����ϴ� ����
    float RunningTime;

    // ���� �޴��� �̵��ϱ� ���� Ÿ�̸� �ڵ� ����
    FTimerHandle TimerHandle_ReturnToMainMenu;
};
