// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

UCLASS()
class SHOOTER_API AItem : public AActor
{
    GENERATED_BODY()
    
public:	
    // Sets default values for this actor's properties
    AItem();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    /** ���������� ��� ���������� AreaSphere */
    UFUNCTION()
    void OnSphereOverlap(
            UPrimitiveComponent* OverlappedComponent,
            AActor* OtherActor,
            UPrimitiveComponent* OtherComp,
            int32 OtherBodyIndex,
            bool bFromSweep,
            const FHitResult& SweepResult);

    /** ���������� ��� ���������� ���������� AreaSphere */
    UFUNCTION()
    void OnSphereEndOverlap(
            UPrimitiveComponent* OverlappedComponent,
            AActor* OtherActor,
            UPrimitiveComponent* OtherComp,
            int32 OtherBodyIndex);

public:	
    // Called every frame
    virtual void Tick(float DeltaTime) override;

private:
    /** Skeletal Mesh ��� �������� */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    USkeletalMeshComponent* ItemMesh;

    /** ����������� ����� ������������ � ����� ��� ����������� �������� HUD */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    class UBoxComponent* CollisionBox;

    /** ����������� ������, ����� ����� ������� �� ������� */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    class UWidgetComponent* PickupWidget;

    /** �������� ������������ ��������� ��� ������������ */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    class USphereComponent* AreaSphere;

    /** ���, ������� ���������� �� Pickup Widget */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    FString ItemName;

public:

    FORCEINLINE UWidgetComponent* GetPickupWidget() const { return PickupWidget; }
};
