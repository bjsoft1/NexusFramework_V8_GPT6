Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
$script:ProjectRoot = Split-Path -Parent $PSScriptRoot
$script:ProjectFile = Join-Path $script:ProjectRoot 'NexusFrameworkV8.uproject'
function Resolve-NexusEngine([string]$EngineRoot) {
    $choices = @($EngineRoot, $env:UE_ROOT, 'C:\Program Files\Epic Games\UE_5.8')
    foreach ($choice in $choices) {
        if ([string]::IsNullOrWhiteSpace($choice)) { continue }
        $versionFile = Join-Path $choice 'Engine\Build\Build.version'
        $buildScript = Join-Path $choice 'Engine\Build\BatchFiles\Build.bat'
        if (!(Test-Path -LiteralPath $versionFile) -or !(Test-Path -LiteralPath $buildScript)) { continue }
        $version = Get-Content -Raw -LiteralPath $versionFile | ConvertFrom-Json
        if ($version.MajorVersion -ne 5 -or $version.MinorVersion -ne 8) {
            throw "This candidate targets UE 5.8. Found $($version.MajorVersion).$($version.MinorVersion) at $choice. Port/build verification is required for another version."
        }
        return (Resolve-Path -LiteralPath $choice).Path
    }
    throw 'Unreal Engine 5.8 was not found. Pass -EngineRoot or set UE_ROOT to the folder containing Engine.'
}
function New-NexusRun([string]$Kind) {
    $stamp = (Get-Date -Format 'yyyyMMdd-HHmmss') + '-' + [Guid]::NewGuid().ToString('N').Substring(0,8)
    $folder = Join-Path $script:ProjectRoot "Saved\Verification\$Kind-$stamp"
    New-Item -ItemType Directory -Path $folder -Force | Out-Null
    return $folder
}
function Invoke-NexusLogged([string]$Exe, [string[]]$Arguments, [string]$Log) {
    Write-Host "Running: $Exe $($Arguments -join ' ')"
    # Windows PowerShell treats native stderr as ErrorRecords. Let the exit code
    # determine success; preserve both streams in the log instead of hiding them.
    $oldPreference = $ErrorActionPreference
    try {
        $ErrorActionPreference = 'Continue'
        & $Exe @Arguments 2>&1 | Tee-Object -FilePath $Log | Out-Host
        $code = $LASTEXITCODE
    } finally { $ErrorActionPreference = $oldPreference }
    if ($code -ne 0) { throw "Command failed with exit code $code. See $Log" }
}
