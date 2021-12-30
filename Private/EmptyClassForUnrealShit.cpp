// Fill out your copyright notice in the Description page of Project Settings.


#include "EmptyClassForUnrealShit.h"
#include "ForceField.h"

// Sets default values
AEmptyClassForUnrealShit::AEmptyClassForUnrealShit()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEmptyClassForUnrealShit::BeginPlay()
{
	Super::BeginPlay();
	//ForceField forceField;
	//forceField.BeginRendering();
	
}

// Called every frame
void AEmptyClassForUnrealShit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

