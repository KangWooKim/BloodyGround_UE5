// Fill out your copyright notice in the Description page of Project Settings.


#include "EliteZombieAnimInstance.h"
#include "EliteZombie.h"

void UEliteZombieAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    // 이동 속도 업데이트
    AEliteZombie* Zombie = Cast<AEliteZombie>(TryGetPawnOwner());
    if (Zombie)
    {
        ZombieInjuryState = Zombie->GetZombieInjuryState(); // 좀비의 부상 상태를 업데이트
    }
}
