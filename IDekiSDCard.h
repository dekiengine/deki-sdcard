#pragma once

#include "providers/IDekiPackage.h"
#include "providers/IDekiFileSystem.h"

/**
 * @brief SD card operational states
 */
enum class SDCardState
{
    NotMounted,     // Card not mounted (may or may not be inserted)
    Mounting,       // Mount operation in progress
    Mounted,        // Card mounted and ready for file operations
    Error           // Mount failed or card error
};

/**
 * @brief SD card interface mode
 */
enum class SDCardMode : uint8_t
{
    SPI = 0,            // SPI mode (most compatible, uses MOSI/MISO/CLK/CS)
    SDMMC_1BIT = 1,     // 1-bit SD mode (faster than SPI)
    SDMMC_4BIT = 2      // 4-bit SD mode (fastest, uses more pins)
};

/**
 * @brief Abstract interface for SD card packages
 *
 * Extends IDekiPackage with SD card-specific functionality.
 * Provides filesystem access via IDekiFileSystem interface.
 */
class IDekiSDCard : public IDekiPackage
{
public:
    const char* GetPackageCategory() const override { return "storage"; }

    virtual bool Mount() = 0;
    virtual void Unmount() = 0;
    virtual SDCardState GetCardState() const = 0;
    virtual bool IsCardInserted() const = 0;
    virtual uint64_t GetTotalBytes() const = 0;
    virtual uint64_t GetFreeBytes() const = 0;
    virtual IDekiFileSystem* GetFileSystem() = 0;
    virtual const char* GetMountPoint() const = 0;
    virtual SDCardMode GetMode() const = 0;

    // ---- Storage Mode (USB Mass Storage) ----

    virtual bool SupportsStorageMode() const { return false; }
    virtual bool SetStorageMode(bool enabled) { (void)enabled; return false; }
    virtual bool IsStorageMode() const { return false; }
};
