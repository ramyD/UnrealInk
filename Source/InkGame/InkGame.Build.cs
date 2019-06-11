using System.IO;
using UnrealBuildTool;

public class InkGame : ModuleRules
{
	public InkGame( ReadOnlyTargetRules target ) : base(target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange( new string[] { "Core", "CoreUObject", "Engine", "Ink" } );
		PrivateDependencyModuleNames.AddRange( new string[] { "RenderCore" } );

        //The path for the header files
        PublicIncludePaths.AddRange(new string[] { Path.Combine(ModuleDirectory, "Public") });

        //The path for the source files
        PrivateIncludePaths.AddRange(new string[] { Path.Combine(ModuleDirectory,"Private") });
    }
}