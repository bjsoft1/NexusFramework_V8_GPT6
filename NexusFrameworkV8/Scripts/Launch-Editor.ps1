[CmdletBinding()]
param([string]$EngineRoot = '', [switch]$SkipBuild)
. "$PSScriptRoot\Common.ps1"
$engine = Resolve-NexusEngine $EngineRoot
if (!$SkipBuild) { & "$PSScriptRoot\Build-Editor.ps1" -EngineRoot $engine }
$editor = Join-Path $engine 'Engine\Binaries\Win64\UnrealEditor.exe'
if (!(Test-Path -LiteralPath $editor)) { throw "Editor executable missing: $editor" }
# Quote the project path explicitly; ProcessStartInfo joins ArgumentList on Windows PowerShell.
Start-Process -FilePath $editor -ArgumentList @(('"' + $ProjectFile + '"')) -WorkingDirectory $ProjectRoot
Write-Host 'In Unreal: Tools > Nexus V8 Road Editor. Start with Demo, then Build preview.'
