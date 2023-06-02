// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ShooterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTER_API UShooterAnimInstance : public UAnimInstance
{
    GENERATED_BODY()
    
public:

    // работает как тик и вызывается в каждом кадре из анимационного блюпринта
    UFUNCTION(BlueprintCallable)
    void UpdateAnimationProperties(float DeltaTime);

    // подобна BeginPlay() для акторов
    virtual void NativeInitializeAnimation() override;

private:

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
    class AShooterCharacter* ShooterCharacter;

    /** Скорость персонажа */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
    float Speed;

    /** Находится ли персонаж в воздухе */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
    bool bIsInAir;

    /** Двигается ли персонаж */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
    bool bIsAccelerating;

    /** Смещение рыскания, используемое для бокового движения */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
    float MovementOffsetYaw;
};
