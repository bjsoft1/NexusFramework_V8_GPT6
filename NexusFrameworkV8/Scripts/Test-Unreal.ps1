[CmdletBinding()]
param([string]$EngineRoot = '', [switch]$SkipBuild)
. "$PSScriptRoot\Common.ps1"
$engine = Resolve-NexusEngine $EngineRoot
if (!$SkipBuild) { & "$PSScriptRoot\Build-Editor.ps1" -EngineRoot $engine }
$run = New-NexusRun 'UnrealAutomation'
$reportDir = Join-Path $run 'Report'
$engineLog = Join-Path $run 'Unreal.log'
$cmd = Join-Path $engine 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe'
# NullRHI tests logic/transactions, NOT actual viewport/rendering/material behavior.
Invoke-NexusLogged $cmd @(
    $ProjectFile,'-unattended','-nop4','-NullRHI','-NoSound',
    '-ExecCmds=Automation RunTests NexusV8.Automated',
    '-TestExit=Automation Test Queue Empty',"-ReportExportPath=$reportDir","-log=$engineLog"
) (Join-Path $run 'console.log')
$index = Join-Path $reportDir 'index.json'
if (!(Test-Path -LiteralPath $index)) { throw "Automation produced no index.json; NOT VERIFIED. Inspect $run" }
$data = Get-Content -Raw -LiteralPath $index | ConvertFrom-Json
$testsProperty = $data.PSObject.Properties['tests']
if ($null -eq $testsProperty) { throw 'Unknown automation report shape. Inspect manually; do not mark PASS.' }
$tests = @($testsProperty.Value | Where-Object { $_.fullTestPath -like 'NexusV8.Automated.*' })
if ($tests.Count -ne 8) { throw "Expected 8 native automation results, found $($tests.Count). Run is incomplete." }
$bad = @($tests | Where-Object { $_.state -ne 'Success' })
if ($bad.Count -gt 0) { throw "$($bad.Count) native tests were not successful. Inspect $index" }
Write-Host "8 native logic/transaction tests succeeded. Evidence: $run"
Write-Host 'The 30 Phase 1/2 acceptance scenarios and visual/real-Landscape checks are still separate.'
