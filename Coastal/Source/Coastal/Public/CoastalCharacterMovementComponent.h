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

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))

class COASTAL_API UCoastalCharacterMovementComponent : public UCharacterMovementComponent
{
    GENERATED_BODY()

    class FSavedMove_Coastal : public FSavedMove_Character
    {
    public:
        // flags
        uint8 Saved_bWantsToSprint : 1;

        // dictate whether new move is the same and does not need to be sent separately
        virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter,
                                    float MaxDelta) const override;
        virtual void Clear() override;
        virtual uint8 GetCompressedFlags() const override;
        virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel,
                                FNetworkPredictionData_Client_Character& ClientData) override;
        virtual void PrepMoveFor(ACharacter* C) override;
    };

    class FNetworkPredictionData_Client_Coastal : public FNetworkPredictionData_Client_Character
    {
    public:
        FNetworkPredictionData_Client_Coastal(const UCharacterMovementComponent& ClientMovement);

        // Override saved move class with our custom class
        virtual FSavedMovePtr AllocateNewMove() override;
    };

    // parameters
    UPROPERTY(EditDefaultsOnly) float MaxSpeed_Walk = 500.f;
    UPROPERTY(EditDefaultsOnly) float MaxSpeed_Sprint = 1000.f;

    UPROPERTY(EditDefaultsOnly) float MinSpeed_Skate = 300.f;
    UPROPERTY(EditDefaultsOnly) float EnterImpulse_Skate = 400.f;
    UPROPERTY(EditDefaultsOnly) float GravityForce_Skate = 4000.f;
    UPROPERTY(EditDefaultsOnly) float Friction_Skate = 1.3f;

    // transient
    UPROPERTY(Transient) ACoastalCharacter* CoastalCharacterOwner;

    // flags
    bool Safe_bWantsToSprint;
    bool Safe_bPrevWantsToCrouch;

public:
    UCoastalCharacterMovementComponent();

    virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;

protected:
    virtual void InitializeComponent() override;

    virtual void UpdateFromCompressedFlags(uint8 Flags) override;

    virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;

    virtual void PhysCustom(float DeltaTime, int32 Iterations) override;

public:
    void EnterSkate();
    void ExitSkate();
    void PhysSkate(float DeltaTime, int32 Iterations);

    std::optional<FVector> GetHitNormalCharacter() const;
    std::optional<FVector> GetHitNormalCharacterEquipment() const;

public:
    UFUNCTION(BlueprintCallable) void SprintPressed();
    UFUNCTION(BlueprintCallable) void SprintReleased();

    UFUNCTION(BlueprintCallable) void CrouchPressed();
    UFUNCTION(BlueprintCallable) void SkatePressed();

    UFUNCTION(BlueprintPure) bool IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const;
};
