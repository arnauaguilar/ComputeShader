// Fill out your copyright notice in the Description page of Project Settings.


#include "TestRunner.h"


// Sets default values
ATestRunner::ATestRunner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATestRunner::BeginPlay()
{
	Super::BeginPlay();
	field = ForceField();
	field.BeginRendering();
	
}

// Called every frame
void ATestRunner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FForceFieldCSParameters parameters(RenderTarget);
	TimeStamp++;
	parameters.TimeStamp = TimeStamp;
	field.UpdateParameters(parameters);

}
void ATestRunner::BeginDestroy()
{
	Super::BeginDestroy();
	field.EndRendering();


}

