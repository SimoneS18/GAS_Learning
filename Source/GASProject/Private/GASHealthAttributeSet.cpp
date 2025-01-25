// Fill out your copyright notice in the Description page of Project Settings.


#include "GASHealthAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

UGASHealthAttributeSet::UGASHealthAttributeSet()
	:
	Health(40.0f),
	MaxHealth(100.0f),
	HealthRegenRate(1.0f),
	Shield(0.0f),
	MaxShield(50.0f),
	ShieldRegenRate(1.0f),
	CritChance(0.0f),
	CritMultiplier(2.0f),
	CritResistance(0.0f)
{ }

void UGASHealthAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
	ClampAttributeOnChange(Attribute, NewValue);
}

void UGASHealthAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	// This is called whenever attributes change, so for max health/mana we want to scale the current totals to match
	Super::PreAttributeChange(Attribute, NewValue);
	ClampAttributeOnChange(Attribute, NewValue);
}

void UGASHealthAttributeSet::ClampAttributeOnChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	else if (Attribute == GetShieldAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxShield());
	}
}

void UGASHealthAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
	UAbilitySystemComponent* Source = Context.GetOriginalInstigatorAbilitySystemComponent();
	const FGameplayTagContainer& SourceTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();

	// Compute the delta between old and new, if it is available
	float DeltaValue = 0;
	if (Data.EvaluatedData.ModifierOp == EGameplayModOp::Type::Additive)
	{
		// If this was additive, store the raw delta value to be passed along later
		DeltaValue = Data.EvaluatedData.Magnitude;
	}

	// Get the Target actor, which should be our owner
	AActor* TargetActor = nullptr;
	AController* TargetController = nullptr;
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
	}

	if (Data.EvaluatedData.Attribute == GetInDamageAttribute())
	{
		float InDamageDone = GetInDamage();

		// is the damage greater then 0?
		if (InDamageDone > 0.0f)
		{
			// checking if there are any listeners for the OnDamageTaken event before broadcasting the damage.
			if (OnDamageTaken.IsBound())
			{
				const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
				AActor* Instigator = EffectContext.GetOriginalInstigator();
				AActor* Causer = EffectContext.GetEffectCauser();

				OnDamageTaken.Broadcast(Instigator, Causer, Data.EffectSpec.CapturedSourceTags.GetSpecTags(), Data.EvaluatedData.Magnitude);
			}

			// if the shield is greater then 0, then we want to subtract from the shield first then health
			if (GetShield() > 0.0f)
			{
				const float NewSheild = GetShield() - InDamageDone;
				InDamageDone -= GetShield();
				SetShield(FMath::Clamp(NewSheild, 0.0f, GetMaxHealth()));
			}

			/// is the damage still greater then 0 and health is greater then 0?
			if (InDamageDone > 0.0f && GetHealth() > 0.0f)
			{
				const float NewHealth = GetHealth() - InDamageDone;
				SetHealth(FMath::Clamp(NewHealth, 0.0f, GetMaxHealth()));
			}
		}
		// 
		// 		const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
		// 		const FHitResult* Hit = EffectContext.GetHitResult();
		// 		if (Hit)
		// 		{
		// 			UE_LOG(LogTemp, Warning, TEXT("Hit Bone: %s"), *Hit->BoneName.ToString());
		// 		}

				// You're resetting the damage value after applying it to shields and health, so it won't affect future calculations.
		SetInDamage(0.0f);

	}
}

void UGASHealthAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UGASHealthAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGASHealthAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGASHealthAttributeSet, HealthRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGASHealthAttributeSet, Shield, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGASHealthAttributeSet, MaxShield, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGASHealthAttributeSet, ShieldRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGASHealthAttributeSet, InDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGASHealthAttributeSet, CritMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGASHealthAttributeSet, CritResistance, COND_None, REPNOTIFY_Always);
}

void UGASHealthAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGASHealthAttributeSet, Health, OldHealth);
}

void UGASHealthAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGASHealthAttributeSet, MaxHealth, OldMaxHealth);
}

void UGASHealthAttributeSet::OnRep_HealthRegenRate(const FGameplayAttributeData& OldHealthRegenRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGASHealthAttributeSet, HealthRegenRate, OldHealthRegenRate);
}

void UGASHealthAttributeSet::OnRep_Shield(const FGameplayAttributeData& OldShield)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGASHealthAttributeSet, Shield, OldShield);
}

void UGASHealthAttributeSet::OnRep_MaxShield(const FGameplayAttributeData& OldMaxShield)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGASHealthAttributeSet, MaxShield, OldMaxShield);
}

void UGASHealthAttributeSet::OnRep_ShieldRegenRate(const FGameplayAttributeData& OldShieldRegenRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGASHealthAttributeSet, ShieldRegenRate, OldShieldRegenRate);
}

void UGASHealthAttributeSet::OnRep_InDamage(const FGameplayAttributeData& OldInDamage)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGASHealthAttributeSet, InDamage, OldInDamage);
}

void UGASHealthAttributeSet::OnRep_CritMultiplier(const FGameplayAttributeData& OldCritMultiplier)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGASHealthAttributeSet, CritMultiplier, OldCritMultiplier);
}

void UGASHealthAttributeSet::OnRep_CritResistance(const FGameplayAttributeData& OldCritResistance)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGASHealthAttributeSet, CritResistance, OldCritResistance);
}
