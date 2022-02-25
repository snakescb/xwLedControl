// This is auto-generated version information attributes by RelaxVersioner.2.5.0.0, Do not edit.
// Generated date: Fri, 25 Feb 2022 23:15:54 GMT

using System.Reflection;

[assembly: AssemblyVersion(@"0.0.37")]
[assembly: AssemblyFileVersion(@"2022.2.26.227")]
[assembly: AssemblyInformationalVersion(@"0.0.37-2ef94235f588c791266ca369af9779108057f677")]
[assembly: AssemblyMetadata(@"Date",@"Fri, 25 Feb 2022 23:07:34 GMT")]
[assembly: AssemblyMetadata(@"Branch",@"main")]
[assembly: AssemblyMetadata(@"Tags",@"")]
[assembly: AssemblyMetadata(@"Author",@"Christian Luethi <luethich80@gmail.com>")]
[assembly: AssemblyMetadata(@"Committer",@"GitHub <noreply@github.com>")]
[assembly: AssemblyMetadata(@"Message",@"Merge pull request #4 from snakescb/develop")]
[assembly: AssemblyMetadata(@"Build",@"")]
[assembly: AssemblyMetadata(@"Generated",@"Fri, 25 Feb 2022 23:15:54 GMT")]
[assembly: AssemblyMetadata(@"TargetFramework",@"netcoreapp3.1")]
[assembly: AssemblyMetadata(@"Platform",@"AnyCPU")]
[assembly: AssemblyMetadata(@"BuildOn",@"Windows_NT")]
[assembly: AssemblyMetadata(@"SdkVersion",@"6.0.101")]

namespace xwLedConfigurator
{
    internal static class ThisAssembly
    {
        public const string @AssemblyVersion = @"0.0.37";
        public const string @AssemblyFileVersion = @"2022.2.26.227";
        public const string @AssemblyInformationalVersion = @"0.0.37-2ef94235f588c791266ca369af9779108057f677";
        public static class AssemblyMetadata
        {
            public const string @Date = @"Fri, 25 Feb 2022 23:07:34 GMT";
            public const string @Branch = @"main";
            public const string @Tags = @"";
            public const string @Author = @"Christian Luethi <luethich80@gmail.com>";
            public const string @Committer = @"GitHub <noreply@github.com>";
            public const string @Message = @"Merge pull request #4 from snakescb/develop";
            public const string @Build = @"";
            public const string @Generated = @"Fri, 25 Feb 2022 23:15:54 GMT";
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

