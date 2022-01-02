// This is auto-generated version information attributes by RelaxVersioner.2.4.0.0, Do not edit.
// Generated date: Sun, 02 Jan 2022 12:48:44 GMT

using System.Reflection;

[assembly: AssemblyVersion(@"0.0.32")]
[assembly: AssemblyFileVersion(@"2022.1.2.23308")]
[assembly: AssemblyInformationalVersion(@"0.0.32-634e4fe1f09038b3f151f58ef1dc0dfa4b53eeb9")]
[assembly: AssemblyMetadata(@"Date",@"Sun, 02 Jan 2022 11:56:57 GMT")]
[assembly: AssemblyMetadata(@"Branch",@"develop")]
[assembly: AssemblyMetadata(@"Tags",@"")]
[assembly: AssemblyMetadata(@"Author",@"Christian Luethi <luethich80@gmail.com>")]
[assembly: AssemblyMetadata(@"Committer",@"Christian Luethi <luethich80@gmail.com>")]
[assembly: AssemblyMetadata(@"Message",@"Develop Test")]
[assembly: AssemblyMetadata(@"Build",@"")]
[assembly: AssemblyMetadata(@"Generated",@"Sun, 02 Jan 2022 12:48:44 GMT")]
[assembly: AssemblyMetadata(@"TargetFramework",@"netcoreapp3.1")]
[assembly: AssemblyMetadata(@"Platform",@"AnyCPU")]
[assembly: AssemblyMetadata(@"BuildOn",@"Windows_NT")]
[assembly: AssemblyMetadata(@"SdkVersion",@"6.0.101")]

namespace xwLedConfigurator
{
    internal static class ThisAssembly
    {
        public const string @AssemblyVersion = @"0.0.32";
        public const string @AssemblyFileVersion = @"2022.1.2.23308";
        public const string @AssemblyInformationalVersion = @"0.0.32-634e4fe1f09038b3f151f58ef1dc0dfa4b53eeb9";
        public static class AssemblyMetadata
        {
            public const string @Date = @"Sun, 02 Jan 2022 11:56:57 GMT";
            public const string @Branch = @"develop";
            public const string @Tags = @"";
            public const string @Author = @"Christian Luethi <luethich80@gmail.com>";
            public const string @Committer = @"Christian Luethi <luethich80@gmail.com>";
            public const string @Message = @"Develop Test";
            public const string @Build = @"";
            public const string @Generated = @"Sun, 02 Jan 2022 12:48:44 GMT";
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

