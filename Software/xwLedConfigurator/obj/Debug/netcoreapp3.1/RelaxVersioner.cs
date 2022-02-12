// This is auto-generated version information attributes by RelaxVersioner.2.5.0.0, Do not edit.
// Generated date: Sat, 12 Feb 2022 01:04:23 GMT

using System.Reflection;

[assembly: AssemblyVersion(@"0.0.58")]
[assembly: AssemblyFileVersion(@"2022.2.10.39158")]
[assembly: AssemblyInformationalVersion(@"0.0.58-a5cb4696aad76d0588afb155ca5399c784dc6551")]
[assembly: AssemblyMetadata(@"Date",@"Thu, 10 Feb 2022 20:45:16 GMT")]
[assembly: AssemblyMetadata(@"Branch",@"develop")]
[assembly: AssemblyMetadata(@"Tags",@"")]
[assembly: AssemblyMetadata(@"Author",@"Christian Luethi <luethich80@gmail.com>")]
[assembly: AssemblyMetadata(@"Committer",@"Christian Luethi <luethich80@gmail.com>")]
[assembly: AssemblyMetadata(@"Message",@"Daily Work")]
[assembly: AssemblyMetadata(@"Build",@"")]
[assembly: AssemblyMetadata(@"Generated",@"Sat, 12 Feb 2022 01:04:23 GMT")]
[assembly: AssemblyMetadata(@"TargetFramework",@"netcoreapp3.1")]
[assembly: AssemblyMetadata(@"Platform",@"AnyCPU")]
[assembly: AssemblyMetadata(@"BuildOn",@"Windows_NT")]
[assembly: AssemblyMetadata(@"SdkVersion",@"6.0.101")]

namespace xwLedConfigurator
{
    internal static class ThisAssembly
    {
        public const string @AssemblyVersion = @"0.0.58";
        public const string @AssemblyFileVersion = @"2022.2.10.39158";
        public const string @AssemblyInformationalVersion = @"0.0.58-a5cb4696aad76d0588afb155ca5399c784dc6551";
        public static class AssemblyMetadata
        {
            public const string @Date = @"Thu, 10 Feb 2022 20:45:16 GMT";
            public const string @Branch = @"develop";
            public const string @Tags = @"";
            public const string @Author = @"Christian Luethi <luethich80@gmail.com>";
            public const string @Committer = @"Christian Luethi <luethich80@gmail.com>";
            public const string @Message = @"Daily Work";
            public const string @Build = @"";
            public const string @Generated = @"Sat, 12 Feb 2022 01:04:23 GMT";
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

