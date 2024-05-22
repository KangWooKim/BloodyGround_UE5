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
    // AKeyItem 클래스의 생성자
    AKeyItem();

protected:
    // 게임이 시작될 때 호출되는 함수
    virtual void BeginPlay() override;

    // 매 프레임마다 호출되는 함수
    virtual void Tick(float DeltaTime) override;

    // SphereComponent를 멤버 변수로 선언
    UPROPERTY(VisibleAnywhere, Category = "Components")
        class USphereComponent* SphereComponent;

    // 오버랩 이벤트가 발생할 때 호출되는 함수
    UFUNCTION()
        void OnOverlapBegin(
            class UPrimitiveComponent* OverlappedComponent,
            class AActor* OtherActor,
            class UPrimitiveComponent* OtherComp,
            int32 OtherBodyIndex,
            bool bFromSweep,
            const FHitResult& SweepResult
        );

    // 모든 클라이언트에서 사운드를 재생하고 아이템을 제거하는 함수
    UFUNCTION(NetMulticast, Reliable)
        void Multicast_PlaySoundAndDestroy();

    // 모든 클라이언트에 승자 메시지를 표시하고 메인 메뉴로 이동하는 함수
    UFUNCTION(NetMulticast, Reliable)
        void Multicast_ShowWinnerAndReturnToMainMenu(const FString& WinnerName);

    // 메인 메뉴로 이동하는 함수
    UFUNCTION()
        void ReturnToMainMenu();

    // StaticMeshComponent를 멤버 변수로 선언
    UPROPERTY(VisibleAnywhere, Category = "Components")
        class UStaticMeshComponent* Mesh;

    // 사운드 큐를 멤버 변수로 선언
    UPROPERTY(EditAnywhere, Category = "Effects")
        class USoundCue* PickupSound;

private:
    // 아이템의 시작 위치를 저장하는 변수
    FVector StartLocation;

    // 애니메이션 실행 시간을 저장하는 변수
    float RunningTime;

    // 메인 메뉴로 이동하기 위한 타이머 핸들 선언
    FTimerHandle TimerHandle_ReturnToMainMenu;
};
