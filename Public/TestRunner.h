// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ForceField.h"
#include "TestRunner.generated.h"


UCLASS()
class COMPUTESHADERRUNNER_API ATestRunner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATestRunner();
	ForceField field;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ShaderDemo)
	class UTextureRenderTargetVolume* RenderTarget;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	uint32 TimeStamp;
	float TranscurredTime;
	bool Initialized;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void BeginDestroy() override;
};
