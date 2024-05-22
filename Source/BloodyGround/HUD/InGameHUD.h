#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "InGameHUD.generated.h"

class UInGameWidget;

/**
 * AInGameHUD Ŭ������ ���� �� UI�� �����ϴ� HUD Ŭ�����Դϴ�.
 */
UCLASS()
class BLOODYGROUND_API AInGameHUD : public AHUD
{
    GENERATED_BODY()

public:

    // ���� ���� Ŭ���� ������ ���� ����
    UPROPERTY(EditAnywhere)
        TSubclassOf<class UUserWidget> InGameWidgetClass;

    // ĳ������ ü���� ������Ʈ�ϴ� �Լ�
    UFUNCTION()
        void UpdateHealth(float HealthPercentage);

    // ĳ������ ź�� ������ ������Ʈ�ϴ� �Լ�
    UFUNCTION()
        void UpdateAmmo(int32 AmmoInMagazine, int32 TotalAmmo);

    // ������ �ؽ�Ʈ�� ȭ�鿡 ǥ���ϴ� �Լ�
    UFUNCTION()
        void SetRespawnText();

    // ȭ�鿡�� ������ �ؽ�Ʈ�� �����ϴ� �Լ�
    UFUNCTION()
        void DeleteRespawnText();

    // ���� �ؽ�Ʈ�� �����ϴ� �Լ�
    UFUNCTION()
        void SetWinnerText(const FString& WinnerName);

protected:

    // HUD�� ó�� ������ �� ȣ��Ǵ� �Լ�
    virtual void BeginPlay() override;

private:

    // InGameWidget �ν��Ͻ��� ���� ������
    UPROPERTY()
        class UInGameWidget* InGameWidget;
};
