#include "BattleComponent.h"
#include "Net/UnrealNetwork.h"

// 생성자
UBattleComponent::UBattleComponent()
{
    // 컴포넌트의 틱 비활성화
    PrimaryComponentTick.bCanEverTick = false;
    // 조준 상태 초기화
    bIsAiming = false;

    // 네트워크 복제 활성화
    SetIsReplicatedByDefault(true);
}

// 네트워크 복제를 위한 속성 설정
void UBattleComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
    // 부모 클래스의 네트워크 복제 설정 호출
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // bIsAiming 변수를 네트워크 복제에 포함시킴
    DOREPLIFETIME(UBattleComponent, bIsAiming);
}

// 컴포넌트가 게임 시작 시 초기화되는 함수
void UBattleComponent::BeginPlay()
{
    // 부모 클래스의 BeginPlay 호출
    Super::BeginPlay();
}

// 컴포넌트가 매 프레임마다 호출되는 틱 함수
void UBattleComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    // 부모 클래스의 TickComponent 호출
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

// 조준 시작 함수
void UBattleComponent::StartAiming()
{
    // 조준 상태를 true로 설정
    bIsAiming = true;
}

// 조준 종료 함수
void UBattleComponent::StopAiming()
{
    // 조준 상태를 false로 설정
    bIsAiming = false;
}

// 발사 시작 함수 (현재 구현되지 않음)
void UBattleComponent::StartFire()
{
    // 발사 관련 로직을 여기에 구현할 수 있음
}
