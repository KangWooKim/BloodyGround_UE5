#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/CapsuleComponent.h"
#include "ServerLocationComponent.generated.h"

class UCapsuleComponent;

// 위치와 시간을 함께 저장하는 구조체
USTRUCT(BlueprintType)
struct FComponentLocationData {
    GENERATED_BODY()

        FVector Location;    // 컴포넌트의 위치
    FRotator Rotation;   // 컴포넌트의 회전

    FComponentLocationData() : Location(FVector::ZeroVector), Rotation(FRotator::ZeroRotator) {}
    FComponentLocationData(FVector InLocation, FRotator InRotation) : Location(InLocation), Rotation(InRotation) {}
};

USTRUCT(BlueprintType)
struct FLocationTimeData {
    GENERATED_BODY()

        FComponentLocationData CapsuleData; // 캡슐 컴포넌트의 위치와 회전 데이터
    FComponentLocationData LeftLegData; // 왼쪽 다리 컴포넌트의 위치와 회전 데이터
    FComponentLocationData RightLegData; // 오른쪽 다리 컴포넌트의 위치와 회전 데이터
    float TimeStamp; // 위치 데이터가 기록된 시간

    FLocationTimeData() : TimeStamp(0.f) {}
    FLocationTimeData(FComponentLocationData InCapsule, FComponentLocationData InLeftLeg, FComponentLocationData InRightLeg, float InTimeStamp)
        : CapsuleData(InCapsule), LeftLegData(InLeftLeg), RightLegData(InRightLeg), TimeStamp(InTimeStamp) {}
};

USTRUCT(BlueprintType)
struct FHitResultData {
    GENERATED_BODY()

        bool bHitBody; // 신체에 대한 적중 여부
    bool bHitLeg; // 다리에 대한 적중 여부

    FHitResultData() : bHitBody(false), bHitLeg(false) {}
    FHitResultData(bool InHitBody, bool InHitLeg) : bHitBody(InHitBody), bHitLeg(InHitLeg) {}
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BLOODYGROUND_API UServerLocationComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    // 생성자
    UServerLocationComponent();

    // 신체 컴포넌트를 반환
    FORCEINLINE UCapsuleComponent* GetBodyComponent() { return CapsuleComponent; }

    // 왼쪽 다리 컴포넌트를 반환
    FORCEINLINE UCapsuleComponent* GetLeftLegComponent() { return LeftLegComponent; }

    // 오른쪽 다리 컴포넌트를 반환
    FORCEINLINE UCapsuleComponent* GetRightLegComponent() { return RightLegComponent; }

    // 주어진 시간에 대해 보간된 위치 데이터를 반환
    UFUNCTION(BlueprintCallable, Category = "ServerLocation")
        FLocationTimeData GetInterpolatedLocationData(float Time);

    // 지정된 트레이스 경로와 위치 데이터로 적중 여부를 검사
    UFUNCTION(BlueprintCallable, Category = "ServerLocation")
        FHitResultData CheckHitWithTrace(const FVector& StartTrace, const FVector& EndTrace, const FLocationTimeData& LocationData);

    // 서버에서 지정된 트레이스 경로와 위치 데이터로 적중 여부를 검사
    UFUNCTION(BlueprintCallable, Category = "ServerLocation")
        FHitResultData ServerTrace(const FVector& StartTrace, const FVector& EndTrace, const FLocationTimeData& LocationData);

    // 현재 위치 데이터를 반환
    UFUNCTION(BlueprintCallable, Category = "ServerLocation")
        FLocationTimeData GetLocationData();

protected:
    // 컴포넌트가 게임 시작 시 초기화되는 함수
    virtual void BeginPlay() override;

    // 컴포넌트가 매 프레임마다 호출되는 틱 함수
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    UPROPERTY()
        TArray<FLocationTimeData> LocationHistory; // 위치 데이터 히스토리

    UPROPERTY()
        float TimeToKeepData; // 오래된 데이터를 얼마나 오래 보관할지
    UPROPERTY()
        float RecordInterval; // 데이터를 얼마나 자주 기록할지
    UPROPERTY()
        float LastRecordTime; // 마지막으로 데이터를 기록한 시간

    UPROPERTY(VisibleAnywhere, Category = "Components", meta = (AllowPrivateAccess = "true"))
        UCapsuleComponent* CapsuleComponent; // 신체 컴포넌트

    UPROPERTY(VisibleAnywhere, Category = "Components", meta = (AllowPrivateAccess = "true"))
        UCapsuleComponent* LeftLegComponent; // 왼쪽 다리 컴포넌트

    UPROPERTY(VisibleAnywhere, Category = "Components", meta = (AllowPrivateAccess = "true"))
        UCapsuleComponent* RightLegComponent; // 오른쪽 다리 컴포넌트

    UPROPERTY(VisibleAnywhere, Category = "Components", meta = (AllowPrivateAccess = "true"))
        UCapsuleComponent* BodyHitCapsule; // 신체 적중 판단용 캡슐 컴포넌트

    UPROPERTY(VisibleAnywhere, Category = "Components", meta = (AllowPrivateAccess = "true"))
        UCapsuleComponent* LeftLegHitCapsule; // 왼쪽 다리 적중 판단용 캡슐 컴포넌트

    UPROPERTY(VisibleAnywhere, Category = "Components", meta = (AllowPrivateAccess = "true"))
        UCapsuleComponent* RightLegHitCapsule; // 오른쪽 다리 적중 판단용 캡슐 컴포넌트

    TArray<UCapsuleComponent*> HitCapsules; // 적중 판단용 캡슐 컴포넌트 배열

    // 현재 위치를 기록하는 함수
    UFUNCTION()
        void RecordLocation();

    // 오래된 데이터를 정리하는 함수
    UFUNCTION()
        void CleanupOldData();

    // 적중 판단용 캡슐 컴포넌트를 업데이트하는 함수
    void UpdateHitCapsule(UCapsuleComponent* HitCapsule, const FComponentLocationData& LocationData);

    // 라인 트레이스를 수행하여 적중 여부를 확인하는 함수
    bool PerformLineTrace(const FVector& StartTrace, const FVector& EndTrace, UCapsuleComponent* PrimaryCapsule);

    // 캡슐 컴포넌트의 충돌 설정을 복원하는 함수
    void RestoreCollisionSettings();
};
