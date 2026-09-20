# Deki SD Card

Docs: https://dekiengine.github.io/deki-sdcard/ (components and properties, generated from the code)

SD card peripheral interface for the Deki Engine. Defines `IDekiSDCard`: mount and unmount, card presence and state, capacity reporting, mount point, and a `Deki::IFileSystem` handle for the mounted volume. Optional mass-storage mode is exposed for backends that support it.

`SDCardComponent` lets a card be configured and mounted from the editor.

Part of [Deki Engine](https://github.com/dekiengine/deki-engine).

## Namespace

Types live in `DekiSdCard`. Scene files store the qualified name, and so does code:

```cpp
using namespace DekiSdCard;
obj->AddComponent<SomeComponent>();
```

Scenes saved before 0.16.0 used bare names and still load; saving writes the current one.

## Install

Package Manager in the Deki Editor, or `DekiEditor --packages-add deki-sdcard <project>`.

## License

Apache 2.0. See [LICENSE](LICENSE).
