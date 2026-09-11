[CmdletBinding()]
param([string]$EngineRoot = '')
. "$PSScriptRoot\Common.ps1"
$engine = Resolve-NexusEngine $EngineRoot
$run = New-NexusRun 'EditorBuild'
@{ engine=$engine; project=$ProjectFile; verification='BUILD_ONLY'; utc=[DateTime]::UtcNow.ToString('o') } |
    ConvertTo-Json | Set-Content -LiteralPath (Join-Path $run 'environment.json') -Encoding UTF8
try {
    Invoke-NexusLogged (Join-Path $engine 'Engine\Build\BatchFiles\Build.bat') @(
        'NexusFrameworkV8Editor','Win64','Development',"-Project=$ProjectFile",'-WaitMutex','-NoHotReloadFromIDE'
    ) (Join-Path $run 'build.log')
    Write-Host "EDITOR BUILD SUCCEEDED. This is not a phase acceptance result. Logs: $run"
} catch {
    $_ | Out-String | Set-Content -LiteralPath (Join-Path $run 'failure.txt')
    throw
}
