#include "ServerLocationComponent.h"
#include "GameFramework/Actor.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"

// 생성자
UServerLocationComponent::UServerLocationComponent()
{
    // 컴포넌트의 틱 활성화
    PrimaryComponentTick.bCanEverTick = true;

    // 데이터 유지 시간 및 기록 간격 설정
    TimeToKeepData = 10.0f; // 예: 10초 동안 데이터 유지
    RecordInterval = 0.1f; // 예: 0.1초마다 위치 기록
    LastRecordTime = 0.f;

    // 기존 컴포넌트 초기화
    CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Body"));
    LeftLegComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("LeftLeg"));
    RightLegComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("RightLeg"));

    // 적중 판단용 컴포넌트 초기화
    BodyHitCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("BodyHit"));
    LeftLegHitCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("LeftLegHit"));
    RightLegHitCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("RightLegHit"));

    // 적중 판단용 캡슐 배열 초기화
    HitCapsules = { BodyHitCapsule, LeftLegHitCapsule, RightLegHitCapsule, CapsuleComponent, LeftLegComponent, RightLegComponent };
}

// 컴포넌트가 게임 시작 시 초기화되는 함수
void UServerLocationComponent::BeginPlay()
{
    // 부모 클래스의 BeginPlay 호출
    Super::BeginPlay();
}

// 컴포넌트가 매 프레임마다 호출되는 틱 함수
void UServerLocationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    // 부모 클래스의 TickComponent 호출
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // 서버에서만 실행
    if (GetOwner()->GetLocalRole() == ROLE_Authority)
    {
        // 지정된 간격마다 위치 기록
        if (GetWorld()->TimeSince(LastRecordTime) >= RecordInterval)
        {
            RecordLocation();
            CleanupOldData();
        }
    }
}

// 위치 기록 함수
void UServerLocationComponent::RecordLocation()
{
    // 각 컴포넌트의 위치 및 회전 정보 기록
    FVector CapsuleLocation = CapsuleComponent->GetComponentLocation();
    FRotator CapsuleRotation = CapsuleComponent->GetComponentRotation();

    FVector LeftLegLocation = LeftLegComponent->GetComponentLocation();
    FRotator LeftLegRotation = LeftLegComponent->GetComponentRotation();

    FVector RightLegLocation = RightLegComponent->GetComponentLocation();
    FRotator RightLegRotation = RightLegComponent->GetComponentRotation();

    // 현재 시간 기록
    float CurrentTime = GetWorld()->GetTimeSeconds();

    // 기록된 위치 데이터를 배열에 추가
    LocationHistory.Add(FLocationTimeData(
        FComponentLocationData(CapsuleLocation, CapsuleRotation),
        FComponentLocationData(LeftLegLocation, LeftLegRotation),
        FComponentLocationData(RightLegLocation, RightLegRotation),
        CurrentTime
    ));
    // 마지막 기록 시간 업데이트
    LastRecordTime = CurrentTime;
}

// 오래된 데이터 정리 함수
void UServerLocationComponent::CleanupOldData()
{
    // 현재 시간 가져오기
    float CurrentTime = GetWorld()->GetTimeSeconds();

    // 지정된 시간보다 오래된 데이터를 제거
    LocationHistory.RemoveAll([CurrentTime, this](const FLocationTimeData& Data) {
        return CurrentTime - Data.TimeStamp > TimeToKeepData;
        });
}

// 보간된 위치 데이터 가져오기 함수
FLocationTimeData UServerLocationComponent::GetInterpolatedLocationData(float Time)
{
    FLocationTimeData PreviousData;
    FLocationTimeData NextData;

    // 보간할 이전 및 다음 데이터 찾기
    for (int32 i = 0; i < LocationHistory.Num() - 1; ++i)
    {
        if (Time >= LocationHistory[i].TimeStamp && Time < LocationHistory[i + 1].TimeStamp)
        {
            PreviousData = LocationHistory[i];
            NextData = LocationHistory[i + 1];
            break;
        }
    }

    // 데이터가 없을 경우 빈 데이터를 반환
    if (PreviousData.TimeStamp == 0.f && NextData.TimeStamp == 0.f)
    {
        return FLocationTimeData();
    }
    // 다음 데이터가 없을 경우 이전 데이터를 반환
    else if (NextData.TimeStamp == 0.f)
    {
        return PreviousData;
    }
    // 보간된 데이터를 계산하여 반환
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

// 트레이스를 통한 적중 검사 함수
FHitResultData  UServerLocationComponent::CheckHitWithTrace(const FVector& StartTrace, const FVector& EndTrace, const FLocationTimeData& LocationData)
{
    // 캡슐 컴포넌트 설정 및 업데이트
    UpdateHitCapsule(BodyHitCapsule, LocationData.CapsuleData);

    // Body에 대한 트레이스 수행
    bool bHitBody = PerformLineTrace(StartTrace, EndTrace, BodyHitCapsule);

    // 캡슐 컴포넌트 충돌 설정 복원
    RestoreCollisionSettings();

    // 왼쪽 및 오른쪽 다리에 대한 트레이스 수행
    UpdateHitCapsule(LeftLegHitCapsule, LocationData.LeftLegData);
    UpdateHitCapsule(RightLegHitCapsule, LocationData.RightLegData);
    bool bHitLeg = PerformLineTrace(StartTrace, EndTrace, LeftLegHitCapsule) || PerformLineTrace(StartTrace, EndTrace, RightLegHitCapsule);

    // 캡슐 컴포넌트 충돌 설정 복원
    RestoreCollisionSettings();

    return FHitResultData(bHitBody, bHitLeg);
}

// 서버에서 트레이스를 통한 적중 검사 함수
FHitResultData  UServerLocationComponent::ServerTrace(const FVector& StartTrace, const FVector& EndTrace, const FLocationTimeData& LocationData)
{
    // 캡슐 컴포넌트 설정 및 업데이트
    UpdateHitCapsule(CapsuleComponent, LocationData.CapsuleData);

    // Body에 대한 트레이스 수행
    bool bHitBody = PerformLineTrace(StartTrace, EndTrace, CapsuleComponent);

    // 캡슐 컴포넌트 충돌 설정 복원
    RestoreCollisionSettings();

    // 왼쪽 및 오른쪽 다리에 대한 트레이스 수행
    UpdateHitCapsule(LeftLegComponent, LocationData.LeftLegData);
    UpdateHitCapsule(RightLegComponent, LocationData.RightLegData);
    bool bHitLeg = PerformLineTrace(StartTrace, EndTrace, LeftLegComponent) || PerformLineTrace(StartTrace, EndTrace, RightLegComponent);

    // 캡슐 컴포넌트 충돌 설정 복원
    RestoreCollisionSettings();

    return FHitResultData(bHitBody, bHitLeg);
}

// 현재 위치 데이터 가져오기 함수
FLocationTimeData UServerLocationComponent::GetLocationData()
{
    // 각 컴포넌트의 위치 및 회전 정보 가져오기
    FVector CapsuleLocation = CapsuleComponent->GetComponentLocation();
    FRotator CapsuleRotation = CapsuleComponent->GetComponentRotation();

    FVector LeftLegLocation = LeftLegComponent->GetComponentLocation();
    FRotator LeftLegRotation = LeftLegComponent->GetComponentRotation();

    FVector RightLegLocation = RightLegComponent->GetComponentLocation();
    FRotator RightLegRotation = RightLegComponent->GetComponentRotation();

    // 현재 시간 가져오기
    float CurrentTime = GetWorld()->GetTimeSeconds();

    // 위치 데이터 반환
    return FLocationTimeData(
        FComponentLocationData(CapsuleLocation, CapsuleRotation),
        FComponentLocationData(LeftLegLocation, LeftLegRotation),
        FComponentLocationData(RightLegLocation, RightLegRotation),
        CurrentTime
    );
}

// 캡슐 컴포넌트의 위치 및 충돌 설정 업데이트 함수
void UServerLocationComponent::UpdateHitCapsule(UCapsuleComponent* HitCapsule, const FComponentLocationData& LocationData)
{
    HitCapsule->SetWorldLocation(LocationData.Location);
    HitCapsule->SetWorldRotation(LocationData.Rotation);
    // 기록된 원본 캡슐 크기로 설정
    HitCapsule->SetCapsuleSize(CapsuleComponent->GetUnscaledCapsuleRadius(), CapsuleComponent->GetUnscaledCapsuleHalfHeight());
    HitCapsule->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    HitCapsule->SetCollisionResponseToChannel(ECC_ComponentCollision, ECR_Block);
}

// 라인 트레이스를 수행하여 적중 여부를 확인하는 함수
bool UServerLocationComponent::PerformLineTrace(const FVector& StartTrace, const FVector& EndTrace, UCapsuleComponent* PrimaryCapsule)
{
    FHitResult HitResult;
    FCollisionQueryParams QueryParams;

    // 라인 트레이스 수행
    bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartTrace, EndTrace, ECC_ComponentCollision, QueryParams);
    // 적중한 컴포넌트가 PrimaryCapsule인지 확인
    return bHit && HitResult.GetComponent() == PrimaryCapsule;
}

// 캡슐 컴포넌트의 충돌 설정 복원 함수
void UServerLocationComponent::RestoreCollisionSettings()
{
    // 모든 캡슐 컴포넌트의 충돌 설정 복원
    for (UCapsuleComponent* Capsule : HitCapsules)
    {
        Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        Capsule->SetCollisionResponseToChannel(ECC_ComponentCollision, ECR_Ignore);
    }
}
