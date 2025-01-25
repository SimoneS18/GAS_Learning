// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GASBaseAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GASHealthAttributeSet.generated.h"

/*
DECLARE_MULTICAST_DELEGATE_FourParams: A Macro that defines a delegate with four parameters. Can have multiple listeners (functions) bound to it
FDamageTakenEvent: Name of this delegate - used to define it in the class

	AActor* EffectInstigator ? The actor responsible for applying the effect (e.g., the player who fired the shot).
	AActor* EffectCauser ? The actor that directly caused the damage (e.g., a projectile that hit the target).
	const FGameplayTagContainer& GameplayTagContainer ? A container holding tags that describe the type of effect (e.g., "Fire", "Poison", etc.).
	float Damage ? The amount of damage dealt.
	Vector HitLocation  ? Helps determine where the damage was applied (e.g., headshot vs. body shot).
	FVector HitNormal  ? Can be useful for knockback effects or directional-based reactions.
*/
DECLARE_MULTICAST_DELEGATE_FourParams(
	FDamageTakenEvent,
	AActor*,																// EffectInstigator
	AActor*,																// EffectCauser
	const FGameplayTagContainer&,				// GameplayTagContainer
	float																	// Damage
	/*,
	Vector // HitLocation,
	FVector //HitNormal
	*/
)


UCLASS()
class GASPROJECT_API UGASHealthAttributeSet : public UGASBaseAttributeSet
{
		GENERATED_BODY()

	public:
		UGASHealthAttributeSet();

		// Health
		// character Health
		UPROPERTY(BlueprintReadOnly, Category = "Attributes", Meta = (AllowPrivateAccess = true))
		FGameplayAttributeData Health;
		ATTRIBUTE_ACCESSORS(UGASHealthAttributeSet, Health);

		// character Max Health
		UPROPERTY(BlueprintReadOnly, Category = "Attributes", Meta = (AllowPrivateAccess = true))
		FGameplayAttributeData MaxHealth;
		ATTRIBUTE_ACCESSORS(UGASHealthAttributeSet, MaxHealth);

		// health that regens over time
		UPROPERTY(BlueprintReadOnly, Category = "Attributes", Meta = (AllowPrivateAccess = true))
		FGameplayAttributeData HealthRegenRate;
		ATTRIBUTE_ACCESSORS(UGASHealthAttributeSet, HealthRegenRate);

		// Shield

		// absorbed before the health is start to be taken
		UPROPERTY(BlueprintReadOnly, Category = "Attributes", Meta = (AllowPrivateAccess = true))
		FGameplayAttributeData Shield;
		ATTRIBUTE_ACCESSORS(UGASHealthAttributeSet, Shield);

		// the max shield the character can have
		UPROPERTY(BlueprintReadOnly, Category = "Attributes", Meta = (AllowPrivateAccess = true))
		FGameplayAttributeData MaxShield;
		ATTRIBUTE_ACCESSORS(UGASHealthAttributeSet, MaxShield);

		// the sheild regen time
		UPROPERTY(BlueprintReadOnly, Category = "Attributes", Meta = (AllowPrivateAccess = true))
		FGameplayAttributeData ShieldRegenRate;
		ATTRIBUTE_ACCESSORS(UGASHealthAttributeSet, ShieldRegenRate);

		// Combat
		// 
		// DDamage Received
		UPROPERTY(BlueprintReadOnly, Category = "Attributes", Meta = (AllowPrivateAccess = true))
		FGameplayAttributeData InDamage;
		ATTRIBUTE_ACCESSORS(UGASHealthAttributeSet, InDamage);

		// Critical Chance
		UPROPERTY(BlueprintReadOnly, Category = "Attributes", Meta = (AllowPrivateAccess = true))
		FGameplayAttributeData CritChance;
		ATTRIBUTE_ACCESSORS(UGASHealthAttributeSet, CritChance);

		// Critical Hit Resistance (reduces critical damage taken) 
		UPROPERTY(BlueprintReadOnly, Category = "Attributes", Meta = (AllowPrivateAccess = true))
		FGameplayAttributeData CritMultiplier;
		ATTRIBUTE_ACCESSORS(UGASHealthAttributeSet, CritMultiplier);

		// Critical Hit Resistance (reduces critical damage taken)
		UPROPERTY(BlueprintReadOnly, Category = "Attributes", Meta = (AllowPrivateAccess = true))
		FGameplayAttributeData CritResistance;
		ATTRIBUTE_ACCESSORS(UGASHealthAttributeSet, CritResistance);

		// Delegate for when damage is taken
		mutable FDamageTakenEvent OnDamageTaken;

	protected:

		virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
		virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
		virtual void ClampAttributeOnChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
		virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

		void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

		UFUNCTION()
		virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);

		UFUNCTION()
		virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

		UFUNCTION()
		virtual void OnRep_HealthRegenRate(const FGameplayAttributeData& OldHealthRegenRate);

		UFUNCTION()
		virtual void OnRep_Shield(const FGameplayAttributeData& OldShield);

		UFUNCTION()
		virtual void OnRep_MaxShield(const FGameplayAttributeData& OldMaxShield);

		UFUNCTION()
		virtual void OnRep_ShieldRegenRate(const FGameplayAttributeData& OldShieldRegenRate);

		UFUNCTION()
		virtual void OnRep_InDamage(const FGameplayAttributeData& OldInDamage);

		UFUNCTION()
		virtual void OnRep_CritMultiplier(const FGameplayAttributeData& OldCritMultiplier);

		UFUNCTION()
		virtual void OnRep_CritResistance(const FGameplayAttributeData& OldCritResistance);
	};