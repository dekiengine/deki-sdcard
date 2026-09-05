#pragma once

#include <cstdint>
#include "SetupComponent.h"
#include "reflection/DekiProperty.h"
#include "IDekiSDCard.h"

/**
 * @brief Component to configure and initialize SD card at runtime
 *
 * Add this component to your prefab to enable SD card filesystem.
 * Set the GPIO pins for your board, and the SD card will be
 * initialized when the prefab loads.
 *
 * The actual SD card implementation is provided by whichever platform
 * integration package is loaded; it registers itself via DekiSDCard::SetFactory().
 *
 * Once mounted, all asset loading will automatically use the SD card
 * via the Deki::FileSystem with the S:/ prefix.
 */
class SDCardComponent : public Deki::SetupComponent
{
public:
    DEKI_COMPONENT(SDCardComponent, Deki::SetupComponent, "System", "3836a84e-5cec-4849-bcc7-b524ad0baa5d", "")
    DEKI_DESCRIPTION("Mounts an SD card at boot so assets can load from it.")

    /** @brief Interface mode: SPI or SDMMC 1-bit */
    DEKI_EXPORT
    SDCardMode mode = SDCardMode::SPI;

    DEKI_VISIBLE_WHEN(mode, SPI)
    DEKI_EXPORT
    DEKI_RANGE(0, 48)
    int32_t mosiPin = 23;

    DEKI_VISIBLE_WHEN(mode, SPI)
    DEKI_EXPORT
    DEKI_RANGE(0, 48)
    int32_t misoPin = 19;

    DEKI_VISIBLE_WHEN(mode, SPI)
    DEKI_EXPORT
    DEKI_RANGE(0, 48)
    int32_t csPin = 5;

    DEKI_VISIBLE_WHEN(mode, SPI)
    DEKI_EXPORT
    DEKI_RANGE(1, 40)
    int32_t spiMhz = 20;

    DEKI_VISIBLE_WHEN(mode, SDMMC_1BIT, SDMMC_4BIT)
    DEKI_EXPORT
    DEKI_RANGE(1, 40)
    int32_t sdmmcMhz = 20;

    DEKI_VISIBLE_WHEN(mode, SDMMC_1BIT, SDMMC_4BIT)
    DEKI_EXPORT
    DEKI_RANGE(0, 48)
    int32_t cmdPin = 15;

    DEKI_VISIBLE_WHEN(mode, SDMMC_1BIT, SDMMC_4BIT)
    DEKI_EXPORT
    DEKI_RANGE(0, 48)
    int32_t d0Pin = 2;

    DEKI_VISIBLE_WHEN(mode, SDMMC_4BIT)
    DEKI_EXPORT
    DEKI_RANGE(0, 48)
    int32_t d1Pin = 4;

    DEKI_VISIBLE_WHEN(mode, SDMMC_4BIT)
    DEKI_EXPORT
    DEKI_RANGE(0, 48)
    int32_t d2Pin = 12;

    DEKI_VISIBLE_WHEN(mode, SDMMC_4BIT)
    DEKI_EXPORT
    DEKI_RANGE(0, 48)
    int32_t d3Pin = 13;

    DEKI_EXPORT
    DEKI_RANGE(0, 48)
    int32_t clkPin = 18;

    DEKI_EXPORT
    DEKI_RANGE(-1, 48)
    int32_t cdPin = -1;

    SDCardComponent();
    virtual ~SDCardComponent();

    void Setup(SetupCallback onComplete) override;
    const char* GetSetupName() const override { return "SD Card"; }

    bool Mount();
    void Unmount();
    bool IsMounted() const { return m_Mounted; }

    static IDekiSDCard* GetSDCardPackage();

private:
    bool m_Mounted = false;

    void LoadAssetLookupTable();
};

#include "generated/SDCardComponent.gen.h"
