// This is auto-generated version information attributes by RelaxVersioner.2.4.0.0, Do not edit.
// Generated date: Thu, 06 Jan 2022 19:54:23 GMT

using System.Reflection;

[assembly: AssemblyVersion(@"0.0.38")]
[assembly: AssemblyFileVersion(@"2022.1.6.16782")]
[assembly: AssemblyInformationalVersion(@"0.0.38-e77e456f287ffb67e2c4314d5e49dbd6f55b028e")]
[assembly: AssemblyMetadata(@"Date",@"Thu, 06 Jan 2022 08:19:25 GMT")]
[assembly: AssemblyMetadata(@"Branch",@"develop")]
[assembly: AssemblyMetadata(@"Tags",@"")]
[assembly: AssemblyMetadata(@"Author",@"Christian Luethi <luethich80@gmail.com>")]
[assembly: AssemblyMetadata(@"Committer",@"GitHub <noreply@github.com>")]
[assembly: AssemblyMetadata(@"Message",@"Update README.md")]
[assembly: AssemblyMetadata(@"Build",@"")]
[assembly: AssemblyMetadata(@"Generated",@"Thu, 06 Jan 2022 19:54:23 GMT")]
[assembly: AssemblyMetadata(@"TargetFramework",@"netcoreapp3.1")]
[assembly: AssemblyMetadata(@"Platform",@"AnyCPU")]
[assembly: AssemblyMetadata(@"BuildOn",@"Windows_NT")]
[assembly: AssemblyMetadata(@"SdkVersion",@"6.0.101")]

namespace xwLedConfigurator
{
    internal static class ThisAssembly
    {
        public const string @AssemblyVersion = @"0.0.38";
        public const string @AssemblyFileVersion = @"2022.1.6.16782";
        public const string @AssemblyInformationalVersion = @"0.0.38-e77e456f287ffb67e2c4314d5e49dbd6f55b028e";
        public static class AssemblyMetadata
        {
            public const string @Date = @"Thu, 06 Jan 2022 08:19:25 GMT";
            public const string @Branch = @"develop";
            public const string @Tags = @"";
            public const string @Author = @"Christian Luethi <luethich80@gmail.com>";
            public const string @Committer = @"GitHub <noreply@github.com>";
            public const string @Message = @"Update README.md";
            public const string @Build = @"";
            public const string @Generated = @"Thu, 06 Jan 2022 19:54:23 GMT";
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

