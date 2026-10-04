# Changelog

Notable changes to `deki-sdcard`. Engine and editor changes are in the
[engine changelog](https://github.com/dekiengine/deki-engine/blob/master/CHANGELOG.md).

A package's `minEngine` names the engine version it needs. Before 1.0 a
breaking change bumps the minor across the editor, the engine and every
package together, so a package with no changes of its own is still released
alongside one that has them.

## Unreleased

### Changed
- **Names follow the code style** (deki-engine/docs/codestyle): types, functions and enum values are PascalCase, constants kPascalCase, members m_PascalCase, locals and parameters camelCase. The code is formatted with clang-format 22.
- The functions the editor finds by name are PascalCase: DekiSDCardRegisterComponents, DekiSDCardGetAutoComponentCount, DekiSDCardEnsureRegistered and the rest. Built against engine ABI 21; a build of this package from before does not load and is rebuilt.
- `SDCardMode` values are `SDMMC1Bit` and `SDMMC4Bit`. Scenes that saved the old names still load.

## 0.17.0

### Changed
- `minEngine` 0.17.0. Reflection ABI 20: the package must be rebuilt.
- The card holds the game's assets only when the build keeps them in
  external storage (`ProjectSettings::GetAssetStorage()`); it then loads them
  with `AssetManager::LoadAssetRoot("S:/")`. Otherwise it is plain storage and
  the engine has already loaded the assets from internal storage.

### Added
- **`required`** on `SDCardComponent` (default on, as before). Off, a boot with
  no card mounted carries on rather than stopping: the screen and controls
  come up, and the assets on `S:/` do not load. For a handheld whose card is
  a slot on the side.

## 0.16.0

### Changed
- **Moved into the `DekiSdCard` namespace.** Every component was declared at global
  scope, which made its identity a bare class name — the name a scene file
  stores and the name the registry keys on — so two packages defining one name
  collided there with nothing to tell them apart. Each component carries
  `DEKI_FORMER_NAME` with the name it was saved under before, so existing
  scenes load unchanged and are written back qualified on the next save.
  Code naming these types needs the namespace: `using namespace DekiSdCard;` or a
  qualified name.
- Enum properties are stored by name rather than by number, so appending to an
  enum or reordering one no longer changes what a saved scene means. Files
  written before this still read.
- `minEngine` 0.16.0. Reflection ABI 17: the package must be rebuilt.

## 0.15.0

### Changed
- Allocates through the engine with an explicit region instead of `new[]` and
  `malloc`.
