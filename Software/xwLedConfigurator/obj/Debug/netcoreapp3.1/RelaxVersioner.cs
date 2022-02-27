// This is auto-generated version information attributes by RelaxVersioner.2.5.0.0, Do not edit.
// Generated date: Sun, 27 Feb 2022 23:07:49 GMT

using System.Reflection;

[assembly: AssemblyVersion(@"0.0.43")]
[assembly: AssemblyFileVersion(@"2022.2.26.31078")]
[assembly: AssemblyInformationalVersion(@"0.0.43-d48ef4326bb2caf8b22df80f38044319263d0fd6")]
[assembly: AssemblyMetadata(@"Date",@"Sat, 26 Feb 2022 16:15:57 GMT")]
[assembly: AssemblyMetadata(@"Branch",@"main")]
[assembly: AssemblyMetadata(@"Tags",@"")]
[assembly: AssemblyMetadata(@"Author",@"Christian Luethi <luethich80@gmail.com>")]
[assembly: AssemblyMetadata(@"Committer",@"Christian Luethi <luethich80@gmail.com>")]
[assembly: AssemblyMetadata(@"Message",@"V1.1 rebuild")]
[assembly: AssemblyMetadata(@"Build",@"")]
[assembly: AssemblyMetadata(@"Generated",@"Sun, 27 Feb 2022 23:07:49 GMT")]
[assembly: AssemblyMetadata(@"TargetFramework",@"netcoreapp3.1")]
[assembly: AssemblyMetadata(@"Platform",@"AnyCPU")]
[assembly: AssemblyMetadata(@"BuildOn",@"Windows_NT")]
[assembly: AssemblyMetadata(@"SdkVersion",@"6.0.101")]

namespace xwLedConfigurator
{
    internal static class ThisAssembly
    {
        public const string @AssemblyVersion = @"0.0.43";
        public const string @AssemblyFileVersion = @"2022.2.26.31078";
        public const string @AssemblyInformationalVersion = @"0.0.43-d48ef4326bb2caf8b22df80f38044319263d0fd6";
        public static class AssemblyMetadata
        {
            public const string @Date = @"Sat, 26 Feb 2022 16:15:57 GMT";
            public const string @Branch = @"main";
            public const string @Tags = @"";
            public const string @Author = @"Christian Luethi <luethich80@gmail.com>";
            public const string @Committer = @"Christian Luethi <luethich80@gmail.com>";
            public const string @Message = @"V1.1 rebuild";
            public const string @Build = @"";
            public const string @Generated = @"Sun, 27 Feb 2022 23:07:49 GMT";
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

