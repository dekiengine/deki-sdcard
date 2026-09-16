# Deki SD Card

Documentation: https://dekiengine.github.io/deki-sdcard/ (components and properties, generated from the code)

SD card peripheral interface for the Deki Engine. Defines `IDekiSDCard`: mount and unmount, card presence and state, capacity reporting, mount point, and an `IDekiFileSystem` handle for the mounted volume. Optional mass-storage mode is exposed for backends that support it.

`SDCardComponent` lets a card be configured and mounted from the editor.

Part of the [Deki Engine](https://github.com/dekiengine/deki-engine) package ecosystem.

## Namespace

This package's types live in `DekiSdCard`. Scene files store the qualified
name, so a component is `DekiSdCard::SomeComponent` there, and code naming one
needs the namespace:

```cpp
using namespace DekiSdCard;
obj->AddComponent<SomeComponent>();
```

Scenes saved before 0.16.0 used bare names and still load: every component
records what it used to be called, and a save writes the current name.

## Installation

Install via the Package Manager inside the Deki Editor.

## License

Licensed under the Apache License, Version 2.0. See [LICENSE](LICENSE) for details.
