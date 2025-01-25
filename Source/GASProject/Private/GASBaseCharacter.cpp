

// Fill out your copyright notice in the Description page of Project Settings.

#include "GASBaseCharacter.h"
#include "AbilitySystemComponent.h"
#include "GASGameplayAbility.h"
#include "GASHealthAttributeSet.h"

// Sets default values
AGASBaseCharacter::AGASBaseCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>("AbilitySystemComp");

	HealthAttributeSet = CreateDefaultSubobject<UGASHealthAttributeSet>("HealthAttributeSet");
}

// Called when the game starts or when spawned
void AGASBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (HealthAttributeSet)
	{
		UE_LOG(LogTemp, Warning, TEXT("Binding to OnDamageTaken"));
		HealthAttributeSet->OnDamageTaken.AddUObject(this, &AGASBaseCharacter::OnDamageTakenChanged);
	}

	// subscribe to attribute changing
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(HealthAttributeSet->GetHealthAttribute()).AddUObject(this, &AGASBaseCharacter::OnHealthAttributeChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(HealthAttributeSet->GetShieldAttribute()).AddUObject(this, &AGASBaseCharacter::OnShieldAttributeChanged);
	
}

UAbilitySystemComponent* AGASBaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AGASBaseCharacter::InitializeAbilities()
{
	// Give Abilities, Server Only
	if (!HasAuthority() || !AbilitySystemComponent)
		return;

	for (TSubclassOf<UGASGameplayAbility>& Ability : DefaultAbilities)
	{
		FGameplayAbilitySpecHandle SpecHandle = AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(Ability, 1, static_cast<int32>(Ability.GetDefaultObject()->GetAbilityInputID()), this));

	}
}

void AGASBaseCharacter::InitializeEffects()
{
	// Give Effects, Server Only
	if (!HasAuthority() || !AbilitySystemComponent)
		return;

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	for (TSubclassOf<UGameplayEffect>& Effect : DefaultEffects)
	{
		FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(Effect, 1, AbilitySystemComponent->MakeEffectContext());
		if (SpecHandle.IsValid())
		{
			FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
}

void AGASBaseCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (!AbilitySystemComponent)
		return;

	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	InitializeAbilities();
	InitializeEffects();
}

void AGASBaseCharacter::OnDamageTakenChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayTagContainer& GameplayerTagContainer, float Damage)
{
	OnDamageTaken(DamageInstigator, DamageCauser, GameplayerTagContainer, Damage);
}

void AGASBaseCharacter::OnHealthAttributeChanged(const FOnAttributeChangeData& Data)
{
	OnHealthChanged(Data.OldValue, Data.NewValue);
}

void AGASBaseCharacter::OnShieldAttributeChanged(const FOnAttributeChangeData& Data)
{
	OnShieldChanged(Data.OldValue, Data.NewValue);
}