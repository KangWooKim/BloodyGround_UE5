#include "BattleComponent.h"
#include "Net/UnrealNetwork.h"

// ������
UBattleComponent::UBattleComponent()
{
    // ������Ʈ�� ƽ ��Ȱ��ȭ
    PrimaryComponentTick.bCanEverTick = false;
    // ���� ���� �ʱ�ȭ
    bIsAiming = false;

    // ��Ʈ��ũ ���� Ȱ��ȭ
    SetIsReplicatedByDefault(true);
}

// ��Ʈ��ũ ������ ���� �Ӽ� ����
void UBattleComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
    // �θ� Ŭ������ ��Ʈ��ũ ���� ���� ȣ��
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // bIsAiming ������ ��Ʈ��ũ ������ ���Խ�Ŵ
    DOREPLIFETIME(UBattleComponent, bIsAiming);
}

// ������Ʈ�� ���� ���� �� �ʱ�ȭ�Ǵ� �Լ�
void UBattleComponent::BeginPlay()
{
    // �θ� Ŭ������ BeginPlay ȣ��
    Super::BeginPlay();
}

// ������Ʈ�� �� �����Ӹ��� ȣ��Ǵ� ƽ �Լ�
void UBattleComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    // �θ� Ŭ������ TickComponent ȣ��
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

// ���� ���� �Լ�
void UBattleComponent::StartAiming()
{
    // ���� ���¸� true�� ����
    bIsAiming = true;
}

// ���� ���� �Լ�
void UBattleComponent::StopAiming()
{
    // ���� ���¸� false�� ����
    bIsAiming = false;
}

// �߻� ���� �Լ� (���� �������� ����)
void UBattleComponent::StartFire()
{
    // �߻� ���� ������ ���⿡ ������ �� ����
}
