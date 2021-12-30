// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "RenderGraphUtils.h"
#include "Runtime/Engine/Classes/Engine/TextureRenderTarget2D.h"

struct  FForceFieldCSParameters
{
	UTextureRenderTarget2D* RenderTarget;


	FIntPoint GetRenderTargetSize() const
	{
		return CachedRenderTargetSize;
	}

	FForceFieldCSParameters() { }
	FForceFieldCSParameters(UTextureRenderTarget2D* IORenderTarget)
		: RenderTarget(IORenderTarget)
	{
		CachedRenderTargetSize = RenderTarget ? FIntPoint(RenderTarget->SizeX, RenderTarget->SizeY) : FIntPoint::ZeroValue;
	}

private:
	FIntPoint CachedRenderTargetSize;
public:
	uint32 TimeStamp;
};
/**
 * 
 */
class COMPUTESHADERRUNNER_API ForceField
{
public:
	ForceField();
	~ForceField();
	
	void BeginRendering();
	void EndRendering();
	void UpdateParameters(FForceFieldCSParameters& DrawParameters);
private:
	void Execute_RenderThread(FRDGBuilder&  builder, const FSceneTextures& SceneTextures);
	
	//The delegate handle to our function that will be executed each frame by the renderer
	FDelegateHandle OnPostResolvedSceneColorHandle;
	//Cached Shader Manager Parameters
	FForceFieldCSParameters cachedParams;
	//Whether we have cached parameters to pass to the shader or not
	volatile bool bCachedParamsAreValid;

	//Reference to a pooled render target where the shader will write its output
	TRefCountPtr<IPooledRenderTarget> ComputeShaderOutput;
	
};
