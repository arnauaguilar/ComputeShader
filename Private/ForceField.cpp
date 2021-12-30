// Fill out your copyright notice in the Description page of Project Settings.


#include "ForceField.h"
#include "GlobalShader.h"
#include "ShaderParameterStruct.h"
#include "RenderTargetPool.h"
#include "RHI.h"



#include "Modules/ModuleManager.h"



#define NUM_THREADS_PER_GROUP_DIMENSION 32

class FForceFieldCS : public FGlobalShader
{
	DECLARE_GLOBAL_SHADER(FForceFieldCS)
	SHADER_USE_PARAMETER_STRUCT(FForceFieldCS, FGlobalShader)
	
	BEGIN_SHADER_PARAMETER_STRUCT(FParameters,)
		SHADER_PARAMETER_UAV(RWTexture2D<float>, OutputTexture)
		SHADER_PARAMETER(FVector2D, Dimensions)
		SHADER_PARAMETER(UINT, TimeStamp)
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}

	static inline void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);

		//We're using it here to add some preprocessor defines. That way we don't have to change both C++ and HLSL code when we change the value for NUM_THREADS_PER_GROUP_DIMENSION
		OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_X"), NUM_THREADS_PER_GROUP_DIMENSION);
		OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_Y"), NUM_THREADS_PER_GROUP_DIMENSION);
		OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_Z"), 1);
	}
};
IMPLEMENT_GLOBAL_SHADER(FForceFieldCS, "/CustomShaders/ForceFieldCS.usf"/*"/Engine/Private/ComputeGenerateMips.usf"*/, "MainCS", SF_Compute);




ForceField::ForceField()
{
}


void ForceField::BeginRendering()
{
	//If the handle is already initalized and valid, no need to do anything
	if (OnPostResolvedSceneColorHandle.IsValid())
	{
		return;
	}
	//Get the Renderer Module and add our entry to the callbacks so it can be executed each frame after the scene rendering is done
	const FName RendererModuleName("Renderer");
	IRendererModule* RendererModule = FModuleManager::GetModulePtr<IRendererModule>(RendererModuleName);
	if (RendererModule)
	{
		OnPostResolvedSceneColorHandle = RendererModule->GetResolvedSceneColorCallbacks().AddRaw(this, &ForceField::Execute_RenderThread);
	}
}

//Stop the compute shader execution
void ForceField::EndRendering()
{
	//If the handle is not valid then there's no cleanup to do
	if (!OnPostResolvedSceneColorHandle.IsValid())
	{
		return;
	}
	//Get the Renderer Module and remove our entry from the ResolvedSceneColorCallbacks
	const FName RendererModuleName("Renderer");
	IRendererModule* RendererModule = FModuleManager::GetModulePtr<IRendererModule>(RendererModuleName);
	if (RendererModule)
	{
		RendererModule->GetResolvedSceneColorCallbacks().Remove(OnPostResolvedSceneColorHandle);
	}

	OnPostResolvedSceneColorHandle.Reset();
	
}

void ForceField::UpdateParameters(FForceFieldCSParameters& DrawParameters)
{
	cachedParams = DrawParameters;
	bCachedParamsAreValid = true;
}


void ForceField::Execute_RenderThread(FRDGBuilder&  builder, const FSceneTextures& SceneTextures)
{
	
	FRHICommandListImmediate& RHICmdList = builder.RHICmdList;

	//If there's no cached parameters to use, skip
	//If no Render Target is supplied in the cachedParams, skip
	if (!(bCachedParamsAreValid && cachedParams.RenderTarget))
	{
		return;
	}

	//Render Thread Assertion
	check(IsInRenderingThread());

	//If the render target is not valid, get an element from the render target pool by supplying a Descriptor
	if (!ComputeShaderOutput.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Not Valid"));
		FPooledRenderTargetDesc ComputeShaderOutputDesc(FPooledRenderTargetDesc::Create2DDesc(cachedParams.GetRenderTargetSize(), cachedParams.RenderTarget->GetRenderTargetResource()->TextureRHI->GetFormat(), FClearValueBinding::None, TexCreate_None, TexCreate_ShaderResource | TexCreate_UAV, false));
		ComputeShaderOutputDesc.DebugName = TEXT("ForceFieldCS_Output_RenderTarget1");
		GRenderTargetPool.FindFreeElement(RHICmdList, ComputeShaderOutputDesc, ComputeShaderOutput, TEXT("ForceFieldCS_Output_RenderTarget2"));
		
	}

	//Specify the resource transition, we're executing this in post scene rendering so we set it to Graphics to Compute
	ERHIAccess transitionType = ERHIAccess::SRVMask;
	
	//RHICmdList.TransitionResource(EResourceTransitionAccess::ERWBarrier, EResourceTransitionPipeline::EGfxToCompute, ComputeShaderOutput->GetRenderTargetItem().UAV);
    RHICmdList.TransitionResource(transitionType, cachedParams.RenderTarget->GetRenderTargetResource()->TextureRHI);
	
	FForceFieldCS::FParameters PassParameters;
	PassParameters.OutputTexture = ComputeShaderOutput->GetRenderTargetItem().UAV;
	PassParameters.Dimensions = FVector2D(cachedParams.GetRenderTargetSize().X, cachedParams.GetRenderTargetSize().Y);
	PassParameters.TimeStamp = cachedParams.TimeStamp;

	//Get a reference to our shader type from global shader map
	TShaderMapRef<FForceFieldCS> forceFieldCS(GetGlobalShaderMap(GMaxRHIFeatureLevel));

	//Dispatch the compute shader
	FComputeShaderUtils::Dispatch(RHICmdList, forceFieldCS, PassParameters,
		FIntVector(FMath::DivideAndRoundUp(cachedParams.GetRenderTargetSize().X, NUM_THREADS_PER_GROUP_DIMENSION),
							FMath::DivideAndRoundUp(cachedParams.GetRenderTargetSize().Y, NUM_THREADS_PER_GROUP_DIMENSION),
							1));

	//Copy shader's output to the render target provided by the client
	RHICmdList.CopyTexture(ComputeShaderOutput->GetRenderTargetItem().ShaderResourceTexture, cachedParams.RenderTarget->GetRenderTargetResource()->TextureRHI, FRHICopyTextureInfo());
	
	//Unbind the previously bound render targets
	
	GRenderTargetPool.FreeUnusedResource(ComputeShaderOutput);

	
}
