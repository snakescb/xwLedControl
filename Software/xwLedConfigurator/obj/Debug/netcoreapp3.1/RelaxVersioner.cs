// This is auto-generated version information attributes by RelaxVersioner.2.4.0.0, Do not edit.
// Generated date: Tue, 04 Jan 2022 18:52:00 GMT

using System.Reflection;

[assembly: AssemblyVersion(@"0.0.36")]
[assembly: AssemblyFileVersion(@"2022.1.2.31169")]
[assembly: AssemblyInformationalVersion(@"0.0.36-17fb546f462bb6fd40636a5921bd4a335fa68ef8")]
[assembly: AssemblyMetadata(@"Date",@"Sun, 02 Jan 2022 16:18:59 GMT")]
[assembly: AssemblyMetadata(@"Branch",@"develop")]
[assembly: AssemblyMetadata(@"Tags",@"")]
[assembly: AssemblyMetadata(@"Author",@"Christian Luethi <luethich80@gmail.com>")]
[assembly: AssemblyMetadata(@"Committer",@"Christian Luethi <luethich80@gmail.com>")]
[assembly: AssemblyMetadata(@"Message",@"Daily upload")]
[assembly: AssemblyMetadata(@"Build",@"")]
[assembly: AssemblyMetadata(@"Generated",@"Tue, 04 Jan 2022 18:52:00 GMT")]
[assembly: AssemblyMetadata(@"TargetFramework",@"netcoreapp3.1")]
[assembly: AssemblyMetadata(@"Platform",@"AnyCPU")]
[assembly: AssemblyMetadata(@"BuildOn",@"Windows_NT")]
[assembly: AssemblyMetadata(@"SdkVersion",@"6.0.101")]

namespace xwLedConfigurator
{
    internal static class ThisAssembly
    {
        public const string @AssemblyVersion = @"0.0.36";
        public const string @AssemblyFileVersion = @"2022.1.2.31169";
        public const string @AssemblyInformationalVersion = @"0.0.36-17fb546f462bb6fd40636a5921bd4a335fa68ef8";
        public static class AssemblyMetadata
        {
            public const string @Date = @"Sun, 02 Jan 2022 16:18:59 GMT";
            public const string @Branch = @"develop";
            public const string @Tags = @"";
            public const string @Author = @"Christian Luethi <luethich80@gmail.com>";
            public const string @Committer = @"Christian Luethi <luethich80@gmail.com>";
            public const string @Message = @"Daily upload";
            public const string @Build = @"";
            public const string @Generated = @"Tue, 04 Jan 2022 18:52:00 GMT";
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

