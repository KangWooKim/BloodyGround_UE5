#include "BaseZombieAnimInstance.h"
#include "BaseZombie.h"

void UBaseZombieAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    // 이동 속도 업데이트
    ABaseZombie* Zombie = Cast<ABaseZombie>(TryGetPawnOwner());
    if (Zombie)
    {
        MovementSpeed = Zombie->GetVelocity().Size(); // 좀비의 그라운드 스피드

        ZombieState = Zombie->GetZombieState(); // 좀비의 현재 행동 상태
    }
}
