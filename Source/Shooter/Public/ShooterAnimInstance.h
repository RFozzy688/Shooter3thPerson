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

    UShooterAnimInstance();

    // работает как тик и вызывается в каждом кадре из анимационного блюпринта
    UFUNCTION(BlueprintCallable)
    void UpdateAnimationProperties(float DeltaTime);

    // подобна BeginPlay() для акторов
    virtual void NativeInitializeAnimation() override;

protected:

    /** Обработка поворота места, переменные */
    void TurnInPlace();

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

    /** Смещение рыскания в последнем кадре */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
    float LastMovementOffsetYaw;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
    bool bAiming;

    /** Рыскание персонажа в этом кадре */
    float CharacterYaw;

    /** Рыскание персонажа в предыдущем кадре */
    float CharacterYawLastFrame;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
    float RootYawOffset;

    /** Значение кривой вращения в этом кадре */
    float RotationCurve;
    /** Значение кривой вращения в последнем кадре */
    float RotationCurveLastFrame;

    /** Шаг вращения цели, используемый для смещения цели */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
    float Pitch;

    /** Истинно при перезарядке, используется для предотвращения смещения прицеливания при перезарядке */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
    bool bReloading;
};
