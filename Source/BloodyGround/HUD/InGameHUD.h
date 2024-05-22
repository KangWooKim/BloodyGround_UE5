#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "InGameHUD.generated.h"

class UInGameWidget;

/**
 * AInGameHUD 클래스는 게임 중 UI를 관리하는 HUD 클래스입니다.
 */
UCLASS()
class BLOODYGROUND_API AInGameHUD : public AHUD
{
    GENERATED_BODY()

public:

    // 게임 위젯 클래스 참조를 위한 변수
    UPROPERTY(EditAnywhere)
        TSubclassOf<class UUserWidget> InGameWidgetClass;

    // 캐릭터의 체력을 업데이트하는 함수
    UFUNCTION()
        void UpdateHealth(float HealthPercentage);

    // 캐릭터의 탄약 정보를 업데이트하는 함수
    UFUNCTION()
        void UpdateAmmo(int32 AmmoInMagazine, int32 TotalAmmo);

    // 리스폰 텍스트를 화면에 표시하는 함수
    UFUNCTION()
        void SetRespawnText();

    // 화면에서 리스폰 텍스트를 제거하는 함수
    UFUNCTION()
        void DeleteRespawnText();

    // 승자 텍스트를 설정하는 함수
    UFUNCTION()
        void SetWinnerText(const FString& WinnerName);

protected:

    // HUD가 처음 생성될 때 호출되는 함수
    virtual void BeginPlay() override;

private:

    // InGameWidget 인스턴스에 대한 포인터
    UPROPERTY()
        class UInGameWidget* InGameWidget;
};
