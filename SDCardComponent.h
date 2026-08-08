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
 * integration module is loaded; it registers itself via DekiSDCard::SetFactory().
 *
 * Once mounted, all asset loading will automatically use the SD card
 * via the DekiFileSystem with the S:/ prefix.
 */
class SDCardComponent : public SetupComponent
{
public:
    DEKI_COMPONENT(SDCardComponent, SetupComponent, "System", "3836a84e-5cec-4849-bcc7-b524ad0baa5d", "")
    DEKI_DESCRIPTION("Mounts an SD card at boot so assets can load from it.")

    /** @brief Interface mode: SPI or SDMMC 1-bit */
    DEKI_EXPORT
    SDCardMode mode = SDCardMode::SPI;

    DEKI_VISIBLE_WHEN(mode, SPI)
    DEKI_EXPORT
    DEKI_RANGE(0, 48)
    int32_t mosi_pin = 23;

    DEKI_VISIBLE_WHEN(mode, SPI)
    DEKI_EXPORT
    DEKI_RANGE(0, 48)
    int32_t miso_pin = 19;

    DEKI_VISIBLE_WHEN(mode, SPI)
    DEKI_EXPORT
    DEKI_RANGE(0, 48)
    int32_t cs_pin = 5;

    DEKI_VISIBLE_WHEN(mode, SPI)
    DEKI_EXPORT
    DEKI_RANGE(1, 40)
    int32_t spi_mhz = 20;

    DEKI_VISIBLE_WHEN(mode, SDMMC_1BIT, SDMMC_4BIT)
    DEKI_EXPORT
    DEKI_RANGE(1, 40)
    int32_t sdmmc_mhz = 20;

    DEKI_VISIBLE_WHEN(mode, SDMMC_1BIT, SDMMC_4BIT)
    DEKI_EXPORT
    DEKI_RANGE(0, 48)
    int32_t cmd_pin = 15;

    DEKI_VISIBLE_WHEN(mode, SDMMC_1BIT, SDMMC_4BIT)
    DEKI_EXPORT
    DEKI_RANGE(0, 48)
    int32_t d0_pin = 2;

    DEKI_VISIBLE_WHEN(mode, SDMMC_4BIT)
    DEKI_EXPORT
    DEKI_RANGE(0, 48)
    int32_t d1_pin = 4;

    DEKI_VISIBLE_WHEN(mode, SDMMC_4BIT)
    DEKI_EXPORT
    DEKI_RANGE(0, 48)
    int32_t d2_pin = 12;

    DEKI_VISIBLE_WHEN(mode, SDMMC_4BIT)
    DEKI_EXPORT
    DEKI_RANGE(0, 48)
    int32_t d3_pin = 13;

    DEKI_EXPORT
    DEKI_RANGE(0, 48)
    int32_t clk_pin = 18;

    DEKI_EXPORT
    DEKI_RANGE(-1, 48)
    int32_t cd_pin = -1;

    SDCardComponent();
    virtual ~SDCardComponent();

    void Setup(SetupCallback onComplete) override;
    const char* GetSetupName() const override { return "SD Card"; }

    bool Mount();
    void Unmount();
    bool IsMounted() const { return m_Mounted; }

    static IDekiSDCard* GetSDCardModule();

private:
    bool m_Mounted = false;

    void LoadAssetLookupTable();
};

#include "generated/SDCardComponent.gen.h"
