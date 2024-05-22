#include "ServerLocationComponent.h"
#include "GameFramework/Actor.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"

// ������
UServerLocationComponent::UServerLocationComponent()
{
    // ������Ʈ�� ƽ Ȱ��ȭ
    PrimaryComponentTick.bCanEverTick = true;

    // ������ ���� �ð� �� ��� ���� ����
    TimeToKeepData = 10.0f; // ��: 10�� ���� ������ ����
    RecordInterval = 0.1f; // ��: 0.1�ʸ��� ��ġ ���
    LastRecordTime = 0.f;

    // ���� ������Ʈ �ʱ�ȭ
    CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Body"));
    LeftLegComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("LeftLeg"));
    RightLegComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("RightLeg"));

    // ���� �Ǵܿ� ������Ʈ �ʱ�ȭ
    BodyHitCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("BodyHit"));
    LeftLegHitCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("LeftLegHit"));
    RightLegHitCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("RightLegHit"));

    // ���� �Ǵܿ� ĸ�� �迭 �ʱ�ȭ
    HitCapsules = { BodyHitCapsule, LeftLegHitCapsule, RightLegHitCapsule, CapsuleComponent, LeftLegComponent, RightLegComponent };
}

// ������Ʈ�� ���� ���� �� �ʱ�ȭ�Ǵ� �Լ�
void UServerLocationComponent::BeginPlay()
{
    // �θ� Ŭ������ BeginPlay ȣ��
    Super::BeginPlay();
}

// ������Ʈ�� �� �����Ӹ��� ȣ��Ǵ� ƽ �Լ�
void UServerLocationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    // �θ� Ŭ������ TickComponent ȣ��
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // ���������� ����
    if (GetOwner()->GetLocalRole() == ROLE_Authority)
    {
        // ������ ���ݸ��� ��ġ ���
        if (GetWorld()->TimeSince(LastRecordTime) >= RecordInterval)
        {
            RecordLocation();
            CleanupOldData();
        }
    }
}

// ��ġ ��� �Լ�
void UServerLocationComponent::RecordLocation()
{
    // �� ������Ʈ�� ��ġ �� ȸ�� ���� ���
    FVector CapsuleLocation = CapsuleComponent->GetComponentLocation();
    FRotator CapsuleRotation = CapsuleComponent->GetComponentRotation();

    FVector LeftLegLocation = LeftLegComponent->GetComponentLocation();
    FRotator LeftLegRotation = LeftLegComponent->GetComponentRotation();

    FVector RightLegLocation = RightLegComponent->GetComponentLocation();
    FRotator RightLegRotation = RightLegComponent->GetComponentRotation();

    // ���� �ð� ���
    float CurrentTime = GetWorld()->GetTimeSeconds();

    // ��ϵ� ��ġ �����͸� �迭�� �߰�
    LocationHistory.Add(FLocationTimeData(
        FComponentLocationData(CapsuleLocation, CapsuleRotation),
        FComponentLocationData(LeftLegLocation, LeftLegRotation),
        FComponentLocationData(RightLegLocation, RightLegRotation),
        CurrentTime
    ));
    // ������ ��� �ð� ������Ʈ
    LastRecordTime = CurrentTime;
}

// ������ ������ ���� �Լ�
void UServerLocationComponent::CleanupOldData()
{
    // ���� �ð� ��������
    float CurrentTime = GetWorld()->GetTimeSeconds();

    // ������ �ð����� ������ �����͸� ����
    LocationHistory.RemoveAll([CurrentTime, this](const FLocationTimeData& Data) {
        return CurrentTime - Data.TimeStamp > TimeToKeepData;
        });
}

// ������ ��ġ ������ �������� �Լ�
FLocationTimeData UServerLocationComponent::GetInterpolatedLocationData(float Time)
{
    FLocationTimeData PreviousData;
    FLocationTimeData NextData;

    // ������ ���� �� ���� ������ ã��
    for (int32 i = 0; i < LocationHistory.Num() - 1; ++i)
    {
        if (Time >= LocationHistory[i].TimeStamp && Time < LocationHistory[i + 1].TimeStamp)
        {
            PreviousData = LocationHistory[i];
            NextData = LocationHistory[i + 1];
            break;
        }
    }

    // �����Ͱ� ���� ��� �� �����͸� ��ȯ
    if (PreviousData.TimeStamp == 0.f && NextData.TimeStamp == 0.f)
    {
        return FLocationTimeData();
    }
    // ���� �����Ͱ� ���� ��� ���� �����͸� ��ȯ
    else if (NextData.TimeStamp == 0.f)
    {
        return PreviousData;
    }
    // ������ �����͸� ����Ͽ� ��ȯ
    else
    {
        float Alpha = (Time - PreviousData.TimeStamp) / (NextData.TimeStamp - PreviousData.TimeStamp);
        FLocationTimeData InterpolatedData;
        InterpolatedData.CapsuleData.Location = FMath::Lerp(PreviousData.CapsuleData.Location, NextData.CapsuleData.Location, Alpha);
        InterpolatedData.CapsuleData.Rotation = FMath::Lerp(PreviousData.CapsuleData.Rotation, NextData.CapsuleData.Rotation, Alpha);
        InterpolatedData.LeftLegData.Location = FMath::Lerp(PreviousData.LeftLegData.Location, NextData.LeftLegData.Location, Alpha);
        InterpolatedData.LeftLegData.Rotation = FMath::Lerp(PreviousData.LeftLegData.Rotation, NextData.LeftLegData.Rotation, Alpha);
        InterpolatedData.RightLegData.Location = FMath::Lerp(PreviousData.RightLegData.Location, NextData.RightLegData.Location, Alpha);
        InterpolatedData.RightLegData.Rotation = FMath::Lerp(PreviousData.RightLegData.Rotation, NextData.RightLegData.Rotation, Alpha);
        return InterpolatedData;
    }
}

// Ʈ���̽��� ���� ���� �˻� �Լ�
FHitResultData  UServerLocationComponent::CheckHitWithTrace(const FVector& StartTrace, const FVector& EndTrace, const FLocationTimeData& LocationData)
{
    // ĸ�� ������Ʈ ���� �� ������Ʈ
    UpdateHitCapsule(BodyHitCapsule, LocationData.CapsuleData);

    // Body�� ���� Ʈ���̽� ����
    bool bHitBody = PerformLineTrace(StartTrace, EndTrace, BodyHitCapsule);

    // ĸ�� ������Ʈ �浹 ���� ����
    RestoreCollisionSettings();

    // ���� �� ������ �ٸ��� ���� Ʈ���̽� ����
    UpdateHitCapsule(LeftLegHitCapsule, LocationData.LeftLegData);
    UpdateHitCapsule(RightLegHitCapsule, LocationData.RightLegData);
    bool bHitLeg = PerformLineTrace(StartTrace, EndTrace, LeftLegHitCapsule) || PerformLineTrace(StartTrace, EndTrace, RightLegHitCapsule);

    // ĸ�� ������Ʈ �浹 ���� ����
    RestoreCollisionSettings();

    return FHitResultData(bHitBody, bHitLeg);
}

// �������� Ʈ���̽��� ���� ���� �˻� �Լ�
FHitResultData  UServerLocationComponent::ServerTrace(const FVector& StartTrace, const FVector& EndTrace, const FLocationTimeData& LocationData)
{
    // ĸ�� ������Ʈ ���� �� ������Ʈ
    UpdateHitCapsule(CapsuleComponent, LocationData.CapsuleData);

    // Body�� ���� Ʈ���̽� ����
    bool bHitBody = PerformLineTrace(StartTrace, EndTrace, CapsuleComponent);

    // ĸ�� ������Ʈ �浹 ���� ����
    RestoreCollisionSettings();

    // ���� �� ������ �ٸ��� ���� Ʈ���̽� ����
    UpdateHitCapsule(LeftLegComponent, LocationData.LeftLegData);
    UpdateHitCapsule(RightLegComponent, LocationData.RightLegData);
    bool bHitLeg = PerformLineTrace(StartTrace, EndTrace, LeftLegComponent) || PerformLineTrace(StartTrace, EndTrace, RightLegComponent);

    // ĸ�� ������Ʈ �浹 ���� ����
    RestoreCollisionSettings();

    return FHitResultData(bHitBody, bHitLeg);
}

// ���� ��ġ ������ �������� �Լ�
FLocationTimeData UServerLocationComponent::GetLocationData()
{
    // �� ������Ʈ�� ��ġ �� ȸ�� ���� ��������
    FVector CapsuleLocation = CapsuleComponent->GetComponentLocation();
    FRotator CapsuleRotation = CapsuleComponent->GetComponentRotation();

    FVector LeftLegLocation = LeftLegComponent->GetComponentLocation();
    FRotator LeftLegRotation = LeftLegComponent->GetComponentRotation();

    FVector RightLegLocation = RightLegComponent->GetComponentLocation();
    FRotator RightLegRotation = RightLegComponent->GetComponentRotation();

    // ���� �ð� ��������
    float CurrentTime = GetWorld()->GetTimeSeconds();

    // ��ġ ������ ��ȯ
    return FLocationTimeData(
        FComponentLocationData(CapsuleLocation, CapsuleRotation),
        FComponentLocationData(LeftLegLocation, LeftLegRotation),
        FComponentLocationData(RightLegLocation, RightLegRotation),
        CurrentTime
    );
}

// ĸ�� ������Ʈ�� ��ġ �� �浹 ���� ������Ʈ �Լ�
void UServerLocationComponent::UpdateHitCapsule(UCapsuleComponent* HitCapsule, const FComponentLocationData& LocationData)
{
    HitCapsule->SetWorldLocation(LocationData.Location);
    HitCapsule->SetWorldRotation(LocationData.Rotation);
    // ��ϵ� ���� ĸ�� ũ��� ����
    HitCapsule->SetCapsuleSize(CapsuleComponent->GetUnscaledCapsuleRadius(), CapsuleComponent->GetUnscaledCapsuleHalfHeight());
    HitCapsule->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    HitCapsule->SetCollisionResponseToChannel(ECC_ComponentCollision, ECR_Block);
}

// ���� Ʈ���̽��� �����Ͽ� ���� ���θ� Ȯ���ϴ� �Լ�
bool UServerLocationComponent::PerformLineTrace(const FVector& StartTrace, const FVector& EndTrace, UCapsuleComponent* PrimaryCapsule)
{
    FHitResult HitResult;
    FCollisionQueryParams QueryParams;

    // ���� Ʈ���̽� ����
    bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartTrace, EndTrace, ECC_ComponentCollision, QueryParams);
    // ������ ������Ʈ�� PrimaryCapsule���� Ȯ��
    return bHit && HitResult.GetComponent() == PrimaryCapsule;
}

// ĸ�� ������Ʈ�� �浹 ���� ���� �Լ�
void UServerLocationComponent::RestoreCollisionSettings()
{
    // ��� ĸ�� ������Ʈ�� �浹 ���� ����
    for (UCapsuleComponent* Capsule : HitCapsules)
    {
        Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        Capsule->SetCollisionResponseToChannel(ECC_ComponentCollision, ECR_Ignore);
    }
}
