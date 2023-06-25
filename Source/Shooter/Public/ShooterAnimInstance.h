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

    // �������� ��� ��� � ���������� � ������ ����� �� ������������� ���������
    UFUNCTION(BlueprintCallable)
    void UpdateAnimationProperties(float DeltaTime);

    // ������� BeginPlay() ��� �������
    virtual void NativeInitializeAnimation() override;

protected:

    /** ��������� �������� �����, ���������� */
    void TurnInPlace();

private:

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
    class AShooterCharacter* ShooterCharacter;

    /** �������� ��������� */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
    float Speed;

    /** ��������� �� �������� � ������� */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
    bool bIsInAir;

    /** ��������� �� �������� */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
    bool bIsAccelerating;

    /** �������� ��������, ������������ ��� �������� �������� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
    float MovementOffsetYaw;

    /** �������� �������� � ��������� ����� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
    float LastMovementOffsetYaw;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
    bool bAiming;

    /** �������� ��������� � ���� ����� */
    float CharacterYaw;

    /** �������� ��������� � ���������� ����� */
    float CharacterYawLastFrame;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
    float RootYawOffset;

    /** �������� ������ �������� � ���� ����� */
    float RotationCurve;
    /** �������� ������ �������� � ��������� ����� */
    float RotationCurveLastFrame;

    /** ��� �������� ����, ������������ ��� �������� ���� */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
    float Pitch;

    /** ������� ��� �����������, ������������ ��� �������������� �������� ������������ ��� ����������� */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
    bool bReloading;

    /** ��������� ��������; ������������ ��� ����������� ����, ����� �������� ���� ������������ */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
    EOffsetState OffsetState;
};
