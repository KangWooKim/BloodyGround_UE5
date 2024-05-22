// Fill out your copyright notice in the Description page of Project Settings.

#include "InGameHUD.h"
#include "BloodyGround/HUD/InGameWidget.h"

// 캐릭터의 체력 업데이트 함수
void AInGameHUD::UpdateHealth(float HealthPercentage)
{
    // InGameWidget의 체력 바 업데이트 함수 호출
    InGameWidget->UpdateHealthBar(HealthPercentage);
}

// 캐릭터의 탄약 정보 업데이트 함수
void AInGameHUD::UpdateAmmo(int32 AmmoInMagazine, int32 TotalAmmo)
{
    // InGameWidget의 탄약 카운트 업데이트 함수 호출
    InGameWidget->UpdateAmmoCount(AmmoInMagazine, TotalAmmo);
}

// 리스폰 텍스트 설정 함수
void AInGameHUD::SetRespawnText()
{
    // InGameWidget의 리스폰 텍스트 설정 함수 호출
    InGameWidget->SetRespawnText();
}

// 리스폰 텍스트 삭제 함수
void AInGameHUD::DeleteRespawnText()
{
    // InGameWidget의 리스폰 텍스트 삭제 함수 호출
    InGameWidget->DeleteRespawnText();
}

// HUD 초기화 함수
void AInGameHUD::BeginPlay()
{
    // 부모 클래스의 BeginPlay 호출
    Super::BeginPlay();

    if (GetWorld()) {
        // 첫 번째 플레이어 컨트롤러 가져오기
        APlayerController* Controller = GetWorld()->GetFirstPlayerController();

        // 플레이어 컨트롤러와 InGameWidgetClass가 유효한 경우
        if (Controller && InGameWidgetClass) {
            // InGameWidget 인스턴스 생성
            InGameWidget = CreateWidget<UInGameWidget>(Controller, InGameWidgetClass);
            // InGameWidget을 뷰포트에 추가
            InGameWidget->AddToViewport();
            // 초기 탄약 정보 업데이트
            UpdateAmmo(10, 50);
        }
    }
}
