// This is auto-generated version information attributes by RelaxVersioner.2.5.0.0, Do not edit.
// Generated date: Thu, 10 Feb 2022 19:36:24 GMT

using System.Reflection;

[assembly: AssemblyVersion(@"0.0.57")]
[assembly: AssemblyFileVersion(@"2022.2.8.2232")]
[assembly: AssemblyInformationalVersion(@"0.0.57-8bebf2706ce029671d426a07d0c5e4eaeda2a709")]
[assembly: AssemblyMetadata(@"Date",@"Tue, 08 Feb 2022 00:14:25 GMT")]
[assembly: AssemblyMetadata(@"Branch",@"develop")]
[assembly: AssemblyMetadata(@"Tags",@"")]
[assembly: AssemblyMetadata(@"Author",@"Christian Luethi <luethich80@gmail.com>")]
[assembly: AssemblyMetadata(@"Committer",@"Christian Luethi <luethich80@gmail.com>")]
[assembly: AssemblyMetadata(@"Message",@"Daily Work")]
[assembly: AssemblyMetadata(@"Build",@"")]
[assembly: AssemblyMetadata(@"Generated",@"Thu, 10 Feb 2022 19:36:24 GMT")]
[assembly: AssemblyMetadata(@"TargetFramework",@"netcoreapp3.1")]
[assembly: AssemblyMetadata(@"Platform",@"AnyCPU")]
[assembly: AssemblyMetadata(@"BuildOn",@"Windows_NT")]
[assembly: AssemblyMetadata(@"SdkVersion",@"6.0.101")]

namespace xwLedConfigurator
{
    internal static class ThisAssembly
    {
        public const string @AssemblyVersion = @"0.0.57";
        public const string @AssemblyFileVersion = @"2022.2.8.2232";
        public const string @AssemblyInformationalVersion = @"0.0.57-8bebf2706ce029671d426a07d0c5e4eaeda2a709";
        public static class AssemblyMetadata
        {
            public const string @Date = @"Tue, 08 Feb 2022 00:14:25 GMT";
            public const string @Branch = @"develop";
            public const string @Tags = @"";
            public const string @Author = @"Christian Luethi <luethich80@gmail.com>";
            public const string @Committer = @"Christian Luethi <luethich80@gmail.com>";
            public const string @Message = @"Daily Work";
            public const string @Build = @"";
            public const string @Generated = @"Thu, 10 Feb 2022 19:36:24 GMT";
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

