#pragma once

#include <deki/providers/IPackage.h>
#include <deki/providers/IFileSystem.h>

namespace DekiSdCard
{

/// SD card states.
enum class SDCardState
{
    NotMounted,  // not mounted (a card may or may not be inserted)
    Mounting,    // mounting
    Mounted,     // mounted and ready for file operations
    Error        // the mount failed, or the card reported an error
};

/// How the card is wired.
enum class SDCardMode : uint8_t
{
    SPI = 0,        // SPI (works on most boards; MOSI/MISO/CLK/CS)
    SDMMC1Bit = 1,  // 1-bit SD mode (faster than SPI)
    SDMMC4Bit = 2   // 4-bit SD mode (fastest, uses more pins)
};

/// An SD card driver: a Deki::IPackage that mounts the card and gives access
/// to its files through Deki::IFileSystem.
class IDekiSDCard : public Deki::IPackage
{
public:
    const char* GetPackageCategory() const override { return "storage"; }

    virtual bool Mount() = 0;
    virtual void Unmount() = 0;
    virtual SDCardState GetCardState() const = 0;
    virtual bool IsCardInserted() const = 0;
    virtual uint64_t GetTotalBytes() const = 0;
    virtual uint64_t GetFreeBytes() const = 0;
    virtual Deki::IFileSystem* GetFileSystem() = 0;
    virtual const char* GetMountPoint() const = 0;
    virtual SDCardMode GetMode() const = 0;

    // ---- Storage Mode (USB Mass Storage) ----

    virtual bool SupportsStorageMode() const { return false; }
    virtual bool SetStorageMode(bool enabled)
    {
        (void)enabled;
        return false;
    }
    virtual bool IsStorageMode() const { return false; }
};

}  // namespace DekiSdCard
