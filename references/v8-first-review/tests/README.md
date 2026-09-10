# Reproduce prototype checks

Open the prototype without installing any dependencies. The commands below are only for automated verification.

From the extracted package root:

```powershell
python tests/static_checks.py
python -m pip install playwright
python -m playwright install chromium
python tests/browser_smoke.py --screenshots
python tests/interaction_smoke.py
```

`browser_smoke.py` accepts `--chromium "C:\path\to\chrome.exe"` to use a specific executable, and optional `--start 1 --end 60`. It otherwise looks for Chromium/Google Chrome on PATH, then uses the Playwright installation. The interaction runner similarly uses PATH or the Playwright installation.

These scripts render the delivered self-contained HTML in an inline browser document. They do not navigate a local HTTP server or invoke Unreal. They overwrite their own generated `evidence/*results.json`, render results, and optional screenshot evidence; they never change the uploaded project archive or a UE project. Back up customized evidence before rerunning.

The test context deliberately has no persistent origin. Tests verify unavailable-storage handling, not local-file persistence. JavaScript syntax checks use Node when available; no npm packages are required.

`static_checks.py` checks reference and payload consistency in the synthetic fixture. It is not a runtime schema certification, full graph solver, physics test, or proof of legal traffic behavior.
