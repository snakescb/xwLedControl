// This is auto-generated version information attributes by RelaxVersioner.2.5.0.0, Do not edit.
// Generated date: Sat, 26 Feb 2022 16:15:21 GMT

using System.Reflection;

[assembly: AssemblyVersion(@"0.0.42")]
[assembly: AssemblyFileVersion(@"2022.2.26.31004")]
[assembly: AssemblyInformationalVersion(@"0.0.42-8164ce70e38afa95abd50282adc33112101f4a70")]
[assembly: AssemblyMetadata(@"Date",@"Sat, 26 Feb 2022 16:13:29 GMT")]
[assembly: AssemblyMetadata(@"Branch",@"main")]
[assembly: AssemblyMetadata(@"Tags",@"V1.1")]
[assembly: AssemblyMetadata(@"Author",@"Christian Luethi <luethich80@gmail.com>")]
[assembly: AssemblyMetadata(@"Committer",@"GitHub <noreply@github.com>")]
[assembly: AssemblyMetadata(@"Message",@"Merge pull request #6 from snakescb/develop")]
[assembly: AssemblyMetadata(@"Build",@"")]
[assembly: AssemblyMetadata(@"Generated",@"Sat, 26 Feb 2022 16:15:21 GMT")]
[assembly: AssemblyMetadata(@"TargetFramework",@"netcoreapp3.1")]
[assembly: AssemblyMetadata(@"Platform",@"AnyCPU")]
[assembly: AssemblyMetadata(@"BuildOn",@"Windows_NT")]
[assembly: AssemblyMetadata(@"SdkVersion",@"6.0.101")]

namespace xwLedConfigurator
{
    internal static class ThisAssembly
    {
        public const string @AssemblyVersion = @"0.0.42";
        public const string @AssemblyFileVersion = @"2022.2.26.31004";
        public const string @AssemblyInformationalVersion = @"0.0.42-8164ce70e38afa95abd50282adc33112101f4a70";
        public static class AssemblyMetadata
        {
            public const string @Date = @"Sat, 26 Feb 2022 16:13:29 GMT";
            public const string @Branch = @"main";
            public const string @Tags = @"V1.1";
            public const string @Author = @"Christian Luethi <luethich80@gmail.com>";
            public const string @Committer = @"GitHub <noreply@github.com>";
            public const string @Message = @"Merge pull request #6 from snakescb/develop";
            public const string @Build = @"";
            public const string @Generated = @"Sat, 26 Feb 2022 16:15:21 GMT";
            public const string @TargetFramework = @"netcoreapp3.1";
            public const string @Platform = @"AnyCPU";
            public const string @BuildOn = @"Windows_NT";
            public const string @SdkVersion = @"6.0.101";
        }
    }
}

#if NET10 || NET11 || NET20 || NET30 || NET35 || NET40

namespace System.Reflection
{
    [AttributeUsage(AttributeTargets.Assembly, AllowMultiple = true, Inherited = false)]
    internal sealed class AssemblyMetadataAttribute : Attribute
    {
        public AssemblyMetadataAttribute(string key, string value)
        {
            this.Key = key;
            this.Value = value;
        }
        public string Key { get; private set; }
        public string Value { get; private set; }
    }
}

#endif

