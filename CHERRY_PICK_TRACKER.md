# Cherry-Pick Tracker

## OneCryptoPkg Commits to Cherry-Pick from pr/opensslpkg to pr/onecryptopkg

This file tracks commits made to OneCryptoPkg on the `pr/opensslpkg` branch that need to be cherry-picked back to `pr/onecryptopkg`.

### Instructions
1. Make changes to OneCryptoPkg on `pr/opensslpkg`
2. Commit with descriptive message prefixed with `[OneCryptoPkg]`
3. Record commit hash below
4. When ready, switch to `pr/onecryptopkg` and cherry-pick these commits

### Commits to Cherry-Pick

| Commit Hash | Date | Description | Status |
|-------------|------|-------------|--------|
| a79583b | 2025-Nov-11 | [OneCryptoPkg] Move OneCrypto headers to MU_BASECORE/CryptoPkg | Ready |
| a48ab0f | 2025-Nov-11 | [OneCryptoPkg] Remove duplicate InternalTlsLib.h | Ready |
| 6bfde78 | 2025-Nov-12 | [OneCryptoPkg] Rename OneCryptoDefinitions.h to BaseCryptDefs.h | Ready |
| 91f2a82 | 2025-Nov-12 | [OneCryptoPkg] Update MANUAL_UPDATE_GUIDE.md for BaseCryptDefs.h rename | Ready |
| 978c97d | 2025-Nov-12 | [OneCryptoPkg] Update include from OneCryptoLibrary.h to BaseCryptLib.h | Ready |

### Cherry-Pick Command Template
```bash
git checkout pr/onecryptopkg
git cherry-pick <commit-hash>
```

### Notes
- Only cherry-pick commits that modify OneCryptoPkg files
- OpensslPkg-only changes should stay on pr/opensslpkg
- Review each commit before cherry-picking to ensure it's OneCryptoPkg-specific
