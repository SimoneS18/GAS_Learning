// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GASBaseCharacter.h"
#include "GASEnemyCharacter.generated.h"

/**
 * 
 */
UCLASS()
class GASPROJECT_API AGASEnemyCharacter : public AGASBaseCharacter
{
	GENERATED_BODY()
	
public:
	AGASEnemyCharacter();

protected:
	virtual void BeginPlay() override;
};
