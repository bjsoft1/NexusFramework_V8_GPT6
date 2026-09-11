[CmdletBinding()]
param([ValidateSet('Debug','Release')][string]$Configuration = 'Debug')
. "$PSScriptRoot\Common.ps1"
$cmake = (Get-Command cmake -ErrorAction Stop).Source
$run = New-NexusRun 'Portable'
$build = Join-Path $ProjectRoot 'Saved\PortableBuild'
Invoke-NexusLogged $cmake @('-S',$ProjectRoot,'-B',$build,'-DCMAKE_CXX_STANDARD=20',"-DCMAKE_BUILD_TYPE=$Configuration") (Join-Path $run 'configure.log')
Invoke-NexusLogged $cmake @('--build',$build,'--config',$Configuration,'--parallel','2') (Join-Path $run 'build.log')
$test = Join-Path $build "$Configuration\nexus_tests.exe"
$exporter = Join-Path $build "$Configuration\nexus_preview.exe"
if (!(Test-Path -LiteralPath $test)) { $test = Join-Path $build 'nexus_tests.exe'; $exporter = Join-Path $build 'nexus_preview.exe' }
if (!(Test-Path -LiteralPath $test)) { throw 'CMake did not produce nexus_tests.exe.' }
$results = Join-Path $run 'Results'
Invoke-NexusLogged $test @($results) (Join-Path $run 'tests.log')
$reportFile = Join-Path $results 'results.json'
if (!(Test-Path -LiteralPath $reportFile)) { throw 'No test report was produced. Run cannot be called verified.' }
$report = Get-Content -Raw -LiteralPath $reportFile | ConvertFrom-Json
if ($report.failed -ne 0 -or $report.tests -ne 88) { throw 'Test report has failures or an unexpected test count. Inspect it before approval.' }
Invoke-NexusLogged $exporter @((Join-Path $run 'Generated')) (Join-Path $run 'export.log')
Write-Host "PORTABLE CORE: $($report.tests) tests; $($report.assertions) assertions; zero failures."
Write-Host "Evidence: $run. Unreal UI/rendering/build acceptance is separate."
