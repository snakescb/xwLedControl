// This is auto-generated version information attributes by RelaxVersioner.2.5.0.0, Do not edit.
// Generated date: Thu, 24 Feb 2022 13:32:17 GMT

using System.Reflection;

[assembly: AssemblyVersion(@"0.0.63")]
[assembly: AssemblyFileVersion(@"2022.2.22.41733")]
[assembly: AssemblyInformationalVersion(@"0.0.63-71938f2ba44d06f062f917f458af8185cd011856")]
[assembly: AssemblyMetadata(@"Date",@"Tue, 22 Feb 2022 22:11:07 GMT")]
[assembly: AssemblyMetadata(@"Branch",@"develop")]
[assembly: AssemblyMetadata(@"Tags",@"")]
[assembly: AssemblyMetadata(@"Author",@"Christian Luethi <luethich80@gmail.com>")]
[assembly: AssemblyMetadata(@"Committer",@"Christian Luethi <luethich80@gmail.com>")]
[assembly: AssemblyMetadata(@"Message",@"Daily Work")]
[assembly: AssemblyMetadata(@"Build",@"")]
[assembly: AssemblyMetadata(@"Generated",@"Thu, 24 Feb 2022 13:32:17 GMT")]
[assembly: AssemblyMetadata(@"TargetFramework",@"netcoreapp3.1")]
[assembly: AssemblyMetadata(@"Platform",@"AnyCPU")]
[assembly: AssemblyMetadata(@"BuildOn",@"Windows_NT")]
[assembly: AssemblyMetadata(@"SdkVersion",@"6.0.101")]

namespace xwLedConfigurator
{
    internal static class ThisAssembly
    {
        public const string @AssemblyVersion = @"0.0.63";
        public const string @AssemblyFileVersion = @"2022.2.22.41733";
        public const string @AssemblyInformationalVersion = @"0.0.63-71938f2ba44d06f062f917f458af8185cd011856";
        public static class AssemblyMetadata
        {
            public const string @Date = @"Tue, 22 Feb 2022 22:11:07 GMT";
            public const string @Branch = @"develop";
            public const string @Tags = @"";
            public const string @Author = @"Christian Luethi <luethich80@gmail.com>";
            public const string @Committer = @"Christian Luethi <luethich80@gmail.com>";
            public const string @Message = @"Daily Work";
            public const string @Build = @"";
            public const string @Generated = @"Thu, 24 Feb 2022 13:32:17 GMT";
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

