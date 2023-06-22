// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
    EIR_Damaged UMETA(DisplayName = "Damaged"),
    EIR_Common UMETA(DisplayName = "Common"),
    EIR_Uncommon UMETA(DisplayName = "Uncommon"),
    EIR_Rare UMETA(DisplayName = "Rare"),
    EIR_Legendary UMETA(DisplayName = "Legendary"),

    EIR_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EItemState : uint8
{
    EIS_Pickup UMETA(DisplayName = "Pickup"),
    EIS_EquipInterping UMETA(DisplayName = "EquipInterping"),
    EIS_PickedUp UMETA(DisplayName = "PickedUp"),
    EIS_Equipped UMETA(DisplayName = "Equipped"),
    EIS_Falling UMETA(DisplayName = "Falling"),

    EIS_MAX UMETA(DisplayName = "DefaultMAX")
};

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

    /** ������������� ������ ���������� �������� ActiveStars � ����������� �� �������� */
    void SetActiveStars();

    // ������������� �������� ���������� �������� � ����������� �� �������� ���������
    void SetItemProperties(EItemState State);

    /** ����������, ����� ItemInterpTimer �������� */
    void FinishInterping();

    /** ������������ ������������ ��������� � ��������� EquipInterping. */
    void ItemInterp(float DeltaTime);

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

    /** ���������� ��������� (�������, ���.) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    int32 ItemCount;

    /** Item rarity - ���������� ���������� ����� � Pickup Widget */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    EItemRarity ItemRarity;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    TArray<bool> ActiveStars;

    /** ��������� �������� */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    EItemState ItemState;

    /** ���� ������, ������������ ��� ������������ �������� �� ��� Z ��� ������������. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    class UCurveFloat* ItemZCurve;

    /** �������� ��������������, ����� ���������� ������������ */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    FVector ItemInterpStartLocation;
    /** ����� �������������� ����� ������� */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    FVector CameraTargetLocation;
    /** true ��� ������������ */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    bool bInterping;

    /** ������ ������������ */
    FTimerHandle ItemInterpTimer;
    /** ����������������� ������ � ������� */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    float ZCurveTime;

    /** ��������� �� character */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    class AShooterCharacter* Character;

    /** X � Y ��� �������� �� ����� ������������ � ��������� EquipInterping */
    float ItemInterpX;
    float ItemInterpY;

    /** ��������� �������� �������� ����� ������� � ������������� ��������� */
    float InterpInitialYawOffset;

public:

    FORCEINLINE UWidgetComponent* GetPickupWidget() const { return PickupWidget; }
    FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
    FORCEINLINE UBoxComponent* GetCollisionBox() const { return CollisionBox; }
    FORCEINLINE EItemState GetItemState() const { return ItemState; }
    void SetItemState(EItemState State);
    FORCEINLINE USkeletalMeshComponent* GetItemMesh() const { return ItemMesh; }

    /** ���������� �� ������ AShooterCharacter */
    void StartItemCurve(AShooterCharacter* Char);
};
