using UnrealBuildTool;

public class ComputeShaderRunner : ModuleRules
{
	public ComputeShaderRunner(ReadOnlyTargetRules Target) : base(Target)
	{
		PublicDependencyModuleNames.AddRange(new string[] { });

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			// All the modules we want as string
			"Core",
			"CoreUObject",
			"Engine",
			"Renderer",
			"RenderCore",
			"RHI",
			"Projects"
		});
	}
}
