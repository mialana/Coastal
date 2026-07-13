// Copyright 2026, Amy Liu. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CoastalCharacterMovementComponent.generated.h"

class ACoastalCharacter;

UENUM(BlueprintType)

enum ECustomMovementMode
{
    CMOVE_None UMETA(DisplayName = "Custom None"),
    CMOVE_Skate UMETA(DisplayName = "Custom Skate"),
    CMOVE_MAX UMETA(Hidden),
};

class FSavedMove_Coastal : public FSavedMove_Character
{
public:
    virtual void Clear() override;
    virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel,
                            FNetworkPredictionData_Client_Character& ClientData) override;
    // dictate whether new move is the same and does not need to be sent separately
    virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const override;
    virtual void PrepMoveFor(ACharacter* C) override;
    virtual uint8 GetCompressedFlags() const override;

    // compressed flags
    uint8 Saved_bWantsToSprint : 1;

    // standard remote procedure calls (rpc) flags
};

class FNetworkPredictionData_Client_Coastal : public FNetworkPredictionData_Client_Character
{
public:
    FNetworkPredictionData_Client_Coastal(const UCharacterMovementComponent& ClientMovement);

    // Override saved move class with our custom class
    virtual FSavedMovePtr AllocateNewMove() override;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))

class COASTAL_API UCoastalCharacterMovementComponent : public UCharacterMovementComponent
{
    GENERATED_BODY()

public:
    UCoastalCharacterMovementComponent();

    virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;

protected:
    virtual void InitializeComponent() override;

    virtual void UpdateFromCompressedFlags(uint8 Flags) override;

    virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
    virtual void PhysCustom(float DeltaTime, int32 Iterations) override;

    virtual bool CanAttemptJump() const override;

    virtual float GetMaxSpeed() const override;
    virtual float GetMaxAcceleration() const override;
    virtual float GetMaxBrakingDeceleration() const override;

    virtual void SetDefaultMovementMode() override;

private:
    void EnterSkate() const;
    void ExitSkate();
    void PhysSkate(float DeltaTime, int32 Iterations);

    std::optional<FVector> GetHitNormalCharacter() const;
    std::optional<FVector> GetHitNormalCharacterEquipment() const;

public:
    UFUNCTION(BlueprintPure) bool IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const;

    UFUNCTION(BlueprintCallable) void SprintPressed();
    UFUNCTION(BlueprintCallable) void SprintReleased();

    UFUNCTION(BlueprintCallable) void SkatePressed();

public:
    UPROPERTY(Category = "Character Movement: MovementMode", EditAnywhere, BlueprintReadWrite)
    TEnumAsByte<ECustomMovementMode> DefaultCustomMovementMode;

    UPROPERTY(EditDefaultsOnly) float Walk_SprintMaxSpeed = 1000.f;
    UPROPERTY(EditDefaultsOnly) float Skate_MaxSpeed = 1000.f;
    UPROPERTY(EditDefaultsOnly) float Skate_SprintMaxSpeed = 1500.f;
    UPROPERTY(EditDefaultsOnly) float Skate_FrictionFactor = 0.5f;
    UPROPERTY(EditDefaultsOnly) float Skate_MaxAcceleration = 300.f;
    UPROPERTY(EditDefaultsOnly) float Skate_BrakingDeceleration = 100.f;

    // transient
    UPROPERTY(Transient) ACoastalCharacter* CoastalCharacterOwner;

    // compressed flags
    bool Safe_bWantsToSprint;

    // standard remote procedure calls (rpc) flags
};
