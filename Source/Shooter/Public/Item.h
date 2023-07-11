// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
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

UENUM(BlueprintType)
enum class EItemType : uint8
{
    EIT_Ammo UMETA(DisplayName = "Ammo"),
    EIT_Weapon UMETA(DisplayName = "Weapon"),

    EIT_MAX UMETA(DisplayName = "DefaultMAX")
};

USTRUCT(BlueprintType)
struct FItemRarityTable : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FLinearColor GlowColor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FLinearColor LightColor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FLinearColor DarkColor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 NumberOfStars;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* IconBackground;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 CustomDepthStencil;
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
    virtual void SetItemProperties(EItemState State);

    /** ����������, ����� ItemInterpTimer �������� */
    void FinishInterping();

    /** ������������ ������������ ��������� � ��������� EquipInterping. */
    void ItemInterp(float DeltaTime);

    /** �������� �������������� �������������� �� ������ ���� �������� */
    FVector GetInterpLocation();

    void PlayPickupSound(bool bForcePlaySound = false);

    virtual void InitializeCustomDepth();

    virtual void OnConstruction(const FTransform& Transform) override;

    void EnableGlowMaterial();

    void UpdatePulse();
    void ResetPulseTimer();
    void StartPulseTimer();

public:	
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called in AShooterCharacter::GetPickupItem
    void PlayEquipSound(bool bForcePlaySound = false);

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
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Rarity, meta = (AllowPrivateAccess = "true"))
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

    /** ������, ������������ ��� ��������������� �������� ��� ������� */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    UCurveFloat* ItemScaleCurve;

    /** ���� ��������������� ��� �������� �������� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    class USoundCue* PickupSound;

    /** ����, ��������������� ��� ���������� �������� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    USoundCue* EquipSound;

    /** Enum ��� ���� ��������, ������� �������� ���� ������� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    EItemType ItemType;

    /** ������ ����� ������������, � ������� ���������������� ���� ������� */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    int32 InterpLocIndex;

    /** ������ ���������, ������� �� ������ �� �������� �� ����� ���������� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    int32 MaterialIndex;

    /** ������������ ���������, ������� �� ����� �������� �� ����� ���������� */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    UMaterialInstanceDynamic* DynamicMaterialInstance;

    /** ��������� ���������, ������������ � ����������� ������������� ��������� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    UMaterialInstance* MaterialInstance;

    bool bCanChangeCustomDepth;

    /** ������ ��� ���������� ������������� ����������� ��������� */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    class UCurveVector* PulseCurve;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    UCurveVector* InterpPulseCurve;

    FTimerHandle PulseTimer;

    /** ����� ��� PulseTimer */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    float PulseCurveTime;

    UPROPERTY(VisibleAnywhere, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    float GlowAmount;

    UPROPERTY(VisibleAnywhere, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    float FresnelExponent;

    UPROPERTY(VisibleAnywhere, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    float FresnelReflectFraction;

    /** ������ ��� ����� �������� � ��������� */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true"))
    UTexture2D* IconItem;

    /** ������ ����������� ��� ����� �������� � ��������� */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true"))
    UTexture2D* AmmoItem;

    /** ���� � ������� Inventory */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true"))
    int32 SlotIndex;

    /** True ����� ��������� ��������� ����� */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true"))
    bool bCharacterInventoryFull;

    /** ������� �������� ��������� */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = DataTable, meta = (AllowPrivateAccess = "true"))
    class UDataTable* ItemRarityDataTable;

    /** ���� � ���������� ��������� */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Rarity, meta = (AllowPrivateAccess = "true"))
    FLinearColor GlowColor;

    /** ������� ���� � ������� ������ */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Rarity, meta = (AllowPrivateAccess = "true"))
    FLinearColor LightColor;

    /** ������ ���� � ������� ������ */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Rarity, meta = (AllowPrivateAccess = "true"))
    FLinearColor DarkColor;

    /** ���������� ����� � ������� ������ */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Rarity, meta = (AllowPrivateAccess = "true"))
    int32 NumberOfStars;

    /** ������� ������ ��� ��������� */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Rarity, meta = (AllowPrivateAccess = "true"))
    UTexture2D* IconBackground;

public:

    FORCEINLINE UWidgetComponent* GetPickupWidget() const { return PickupWidget; }
    FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
    FORCEINLINE UBoxComponent* GetCollisionBox() const { return CollisionBox; }
    FORCEINLINE EItemState GetItemState() const { return ItemState; }
    void SetItemState(EItemState State);
    FORCEINLINE USkeletalMeshComponent* GetItemMesh() const { return ItemMesh; }
    FORCEINLINE USoundCue* GetPickupSound() const { return PickupSound; }
    FORCEINLINE USoundCue* GetEquipSound() const { return EquipSound; }
    FORCEINLINE int32 GetItemCount() const { return ItemCount; }
    FORCEINLINE int32 GetSlotIndex() const { return SlotIndex; }
    FORCEINLINE void SetSlotIndex(int32 Index) { SlotIndex = Index; }
    FORCEINLINE void SetCharacter(AShooterCharacter* Char) { Character = Char; }
    FORCEINLINE void SetCharacterInventoryFull(bool bFull) { bCharacterInventoryFull = bFull; }

    /** ���������� �� ������ AShooterCharacter */
    void StartItemCurve(AShooterCharacter* Char, bool bForcePlaySound = false);

    virtual void EnableCustomDepth();
    virtual void DisableCustomDepth();
    void DisableGlowMaterial();
};
