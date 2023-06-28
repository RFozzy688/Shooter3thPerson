// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ShooterAnimInstance.generated.h"

UENUM(BlueprintType)
enum class EOffsetState : uint8
{
    EOS_Aiming UMETA(DisplayName = "Aiming"),
    EOS_Hip UMETA(DisplayName = "Hip"),
    EOS_Reloading UMETA(DisplayName = "Reloading"),
    EOS_InAir UMETA(DisplayName = "InAir"),


    EOS_MAX UMETA(DisplayName = "DefaultMAX")
};

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

    /** Обработка расчетов для наклона во время бега */
    void Lean(float DeltaTime);

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

    /** Рыскание персонажа в этом кадре; обновляется только когда персонаж стоит и не находится в воздухе */
    float TIPCharacterYaw;

    /** Рыскание персонажа в предыдущем кадре; обновляется только когда персонаж стоит и не находится в воздухе */
    float TIPCharacterYawLastFrame;

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

    /** Состояние смещения; используется для определения того, какое смещение цели использовать */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
    EOffsetState OffsetState;

    /** Рыскать в этом кадре */
    FRotator CharacterRotation;

    /** Рыскать в предыдущем кадре */
    FRotator CharacterRotationLastFrame;

    /** Дельта рыскания, используемая для наклона в бегущем пространстве смешивания */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Lean, meta = (AllowPrivateAccess = "true"))
    float YawDelta;

    /** True при приседании */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Crouching, meta = (AllowPrivateAccess = "true"))
    bool bCrouching;
};
