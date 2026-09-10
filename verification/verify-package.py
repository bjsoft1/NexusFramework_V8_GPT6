#!/usr/bin/env python3
"""Verify extracted documentation bytes and nested ZIP CRCs. Does not test Unreal."""
import hashlib
import json
import sys
import zipfile
from pathlib import Path

def main() -> int:
    root = Path(__file__).resolve().parent.parent
    try:
        manifest = json.loads((root / 'MANIFEST.json').read_text(encoding='utf-8'))
    except (OSError, ValueError) as exc:
        print(f'Cannot read MANIFEST.json: {exc}', file=sys.stderr)
        return 2
    errors = []
    checked = 0
    for entry in manifest.get('files', []):
        relative = entry.get('path', '')
        file_path = (root / relative).resolve()
        try:
            file_path.relative_to(root)
            if not relative or not file_path.is_file():
                raise ValueError('missing file or empty path')
            digest = hashlib.sha256()
            with file_path.open('rb') as stream:
                for chunk in iter(lambda: stream.read(1024 * 1024), b''):
                    digest.update(chunk)
            if digest.hexdigest() != entry.get('sha256'):
                raise ValueError('SHA-256 differs from packaged version')
            if file_path.stat().st_size != entry.get('bytes'):
                raise ValueError('file size differs from manifest')
            if file_path.suffix.lower() == '.zip':
                with zipfile.ZipFile(file_path) as archive:
                    corrupt = archive.testzip()
                    if corrupt:
                        raise ValueError(f'ZIP CRC failure: {corrupt}')
            checked += 1
        except (OSError, ValueError, zipfile.BadZipFile) as exc:
            errors.append(f'{relative}: {exc}')
    if not manifest.get('files'):
        errors.append('Manifest has no file records.')
    for error in errors:
        print('FAIL:', error)
    print(f'Checked {checked} packaged files; {len(errors)} errors.')
    print('This checks documentation integrity only; engine scenarios remain unexecuted.')
    return 1 if errors else 0

if __name__ == '__main__':
    raise SystemExit(main())
