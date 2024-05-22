#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/CapsuleComponent.h"
#include "ServerLocationComponent.generated.h"

class UCapsuleComponent;

// ��ġ�� �ð��� �Բ� �����ϴ� ����ü
USTRUCT(BlueprintType)
struct FComponentLocationData {
    GENERATED_BODY()

        FVector Location;    // ������Ʈ�� ��ġ
    FRotator Rotation;   // ������Ʈ�� ȸ��

    FComponentLocationData() : Location(FVector::ZeroVector), Rotation(FRotator::ZeroRotator) {}
    FComponentLocationData(FVector InLocation, FRotator InRotation) : Location(InLocation), Rotation(InRotation) {}
};

USTRUCT(BlueprintType)
struct FLocationTimeData {
    GENERATED_BODY()

        FComponentLocationData CapsuleData; // ĸ�� ������Ʈ�� ��ġ�� ȸ�� ������
    FComponentLocationData LeftLegData; // ���� �ٸ� ������Ʈ�� ��ġ�� ȸ�� ������
    FComponentLocationData RightLegData; // ������ �ٸ� ������Ʈ�� ��ġ�� ȸ�� ������
    float TimeStamp; // ��ġ �����Ͱ� ��ϵ� �ð�

    FLocationTimeData() : TimeStamp(0.f) {}
    FLocationTimeData(FComponentLocationData InCapsule, FComponentLocationData InLeftLeg, FComponentLocationData InRightLeg, float InTimeStamp)
        : CapsuleData(InCapsule), LeftLegData(InLeftLeg), RightLegData(InRightLeg), TimeStamp(InTimeStamp) {}
};

USTRUCT(BlueprintType)
struct FHitResultData {
    GENERATED_BODY()

        bool bHitBody; // ��ü�� ���� ���� ����
    bool bHitLeg; // �ٸ��� ���� ���� ����

    FHitResultData() : bHitBody(false), bHitLeg(false) {}
    FHitResultData(bool InHitBody, bool InHitLeg) : bHitBody(InHitBody), bHitLeg(InHitLeg) {}
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BLOODYGROUND_API UServerLocationComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    // ������
    UServerLocationComponent();

    // ��ü ������Ʈ�� ��ȯ
    FORCEINLINE UCapsuleComponent* GetBodyComponent() { return CapsuleComponent; }

    // ���� �ٸ� ������Ʈ�� ��ȯ
    FORCEINLINE UCapsuleComponent* GetLeftLegComponent() { return LeftLegComponent; }

    // ������ �ٸ� ������Ʈ�� ��ȯ
    FORCEINLINE UCapsuleComponent* GetRightLegComponent() { return RightLegComponent; }

    // �־��� �ð��� ���� ������ ��ġ �����͸� ��ȯ
    UFUNCTION(BlueprintCallable, Category = "ServerLocation")
        FLocationTimeData GetInterpolatedLocationData(float Time);

    // ������ Ʈ���̽� ��ο� ��ġ �����ͷ� ���� ���θ� �˻�
    UFUNCTION(BlueprintCallable, Category = "ServerLocation")
        FHitResultData CheckHitWithTrace(const FVector& StartTrace, const FVector& EndTrace, const FLocationTimeData& LocationData);

    // �������� ������ Ʈ���̽� ��ο� ��ġ �����ͷ� ���� ���θ� �˻�
    UFUNCTION(BlueprintCallable, Category = "ServerLocation")
        FHitResultData ServerTrace(const FVector& StartTrace, const FVector& EndTrace, const FLocationTimeData& LocationData);

    // ���� ��ġ �����͸� ��ȯ
    UFUNCTION(BlueprintCallable, Category = "ServerLocation")
        FLocationTimeData GetLocationData();

protected:
    // ������Ʈ�� ���� ���� �� �ʱ�ȭ�Ǵ� �Լ�
    virtual void BeginPlay() override;

    // ������Ʈ�� �� �����Ӹ��� ȣ��Ǵ� ƽ �Լ�
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    UPROPERTY()
        TArray<FLocationTimeData> LocationHistory; // ��ġ ������ �����丮

    UPROPERTY()
        float TimeToKeepData; // ������ �����͸� �󸶳� ���� ��������
    UPROPERTY()
        float RecordInterval; // �����͸� �󸶳� ���� �������
    UPROPERTY()
        float LastRecordTime; // ���������� �����͸� ����� �ð�

    UPROPERTY(VisibleAnywhere, Category = "Components", meta = (AllowPrivateAccess = "true"))
        UCapsuleComponent* CapsuleComponent; // ��ü ������Ʈ

    UPROPERTY(VisibleAnywhere, Category = "Components", meta = (AllowPrivateAccess = "true"))
        UCapsuleComponent* LeftLegComponent; // ���� �ٸ� ������Ʈ

    UPROPERTY(VisibleAnywhere, Category = "Components", meta = (AllowPrivateAccess = "true"))
        UCapsuleComponent* RightLegComponent; // ������ �ٸ� ������Ʈ

    UPROPERTY(VisibleAnywhere, Category = "Components", meta = (AllowPrivateAccess = "true"))
        UCapsuleComponent* BodyHitCapsule; // ��ü ���� �Ǵܿ� ĸ�� ������Ʈ

    UPROPERTY(VisibleAnywhere, Category = "Components", meta = (AllowPrivateAccess = "true"))
        UCapsuleComponent* LeftLegHitCapsule; // ���� �ٸ� ���� �Ǵܿ� ĸ�� ������Ʈ

    UPROPERTY(VisibleAnywhere, Category = "Components", meta = (AllowPrivateAccess = "true"))
        UCapsuleComponent* RightLegHitCapsule; // ������ �ٸ� ���� �Ǵܿ� ĸ�� ������Ʈ

    TArray<UCapsuleComponent*> HitCapsules; // ���� �Ǵܿ� ĸ�� ������Ʈ �迭

    // ���� ��ġ�� ����ϴ� �Լ�
    UFUNCTION()
        void RecordLocation();

    // ������ �����͸� �����ϴ� �Լ�
    UFUNCTION()
        void CleanupOldData();

    // ���� �Ǵܿ� ĸ�� ������Ʈ�� ������Ʈ�ϴ� �Լ�
    void UpdateHitCapsule(UCapsuleComponent* HitCapsule, const FComponentLocationData& LocationData);

    // ���� Ʈ���̽��� �����Ͽ� ���� ���θ� Ȯ���ϴ� �Լ�
    bool PerformLineTrace(const FVector& StartTrace, const FVector& EndTrace, UCapsuleComponent* PrimaryCapsule);

    // ĸ�� ������Ʈ�� �浹 ������ �����ϴ� �Լ�
    void RestoreCollisionSettings();
};
