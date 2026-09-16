#pragma once

#include <cstdint>
#include <deki/SetupComponent.h>
#include <deki/reflection/Property.h>
#include "IDekiSDCard.h"

namespace DekiSdCard
{

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
DEKI_CATEGORY("System")
DEKI_DESCRIPTION("Mounts an SD card at boot so assets can load from it.")
DEKI_FORMER_NAME("SDCardComponent")
class SDCardComponent : public Deki::SetupComponent
{
public:

    /** @brief Interface mode: SPI or SDMMC 1-bit */
    DEKI_EXPORT
    DEKI_TOOLTIP("How the card is wired. SPI needs four pins and works on any board; SDMMC is faster but needs dedicated pins, and 4-bit is faster than 1-bit.")
    SDCardMode mode = SDCardMode::SPI;

    DEKI_VISIBLE_WHEN(mode, SPI)
    DEKI_EXPORT
    DEKI_TOOLTIP("SPI mode: GPIO the board sends on, wired to the card's DI or CMD pad.")
    DEKI_RANGE(0, 48)
    int32_t mosiPin = 23;

    DEKI_VISIBLE_WHEN(mode, SPI)
    DEKI_EXPORT
    DEKI_TOOLTIP("SPI mode: GPIO the board receives on, wired to the card's DO or DAT0 pad.")
    DEKI_RANGE(0, 48)
    int32_t misoPin = 19;

    DEKI_VISIBLE_WHEN(mode, SPI)
    DEKI_EXPORT
    DEKI_TOOLTIP("SPI mode: chip select. Held low while this card is being talked to, which is what lets other devices share the bus.")
    DEKI_RANGE(0, 48)
    int32_t csPin = 5;

    DEKI_VISIBLE_WHEN(mode, SPI)
    DEKI_EXPORT
    DEKI_TOOLTIP("SPI clock speed. 20 MHz suits most cards and short wiring; a card that mounts but then reads badly usually wants a lower figure.")
    DEKI_UNIT(Frequency)
    DEKI_RANGE(1, 40)
    int32_t spiHz = 20000000;

    DEKI_VISIBLE_WHEN(mode, SDMMC_1BIT, SDMMC_4BIT)
    DEKI_EXPORT
    DEKI_TOOLTIP("SDMMC clock speed. 20 MHz is a safe start; 40 MHz works on boards with short, well-matched traces.")
    DEKI_UNIT(Frequency)
    DEKI_RANGE(1, 40)
    int32_t sdmmcHz = 20000000;

    DEKI_VISIBLE_WHEN(mode, SDMMC_1BIT, SDMMC_4BIT)
    DEKI_EXPORT
    DEKI_TOOLTIP("SDMMC mode: the command line the board uses to talk to the card.")
    DEKI_RANGE(0, 48)
    int32_t cmdPin = 15;

    DEKI_VISIBLE_WHEN(mode, SDMMC_1BIT, SDMMC_4BIT)
    DEKI_EXPORT
    DEKI_TOOLTIP("SDMMC mode: data line 0. The only data line 1-bit mode uses.")
    DEKI_RANGE(0, 48)
    int32_t d0Pin = 2;

    DEKI_VISIBLE_WHEN(mode, SDMMC_4BIT)
    DEKI_EXPORT
    DEKI_TOOLTIP("SDMMC 4-bit mode: data line 1. Ignored in 1-bit mode.")
    DEKI_RANGE(0, 48)
    int32_t d1Pin = 4;

    DEKI_VISIBLE_WHEN(mode, SDMMC_4BIT)
    DEKI_EXPORT
    DEKI_TOOLTIP("SDMMC 4-bit mode: data line 2. Ignored in 1-bit mode.")
    DEKI_RANGE(0, 48)
    int32_t d2Pin = 12;

    DEKI_VISIBLE_WHEN(mode, SDMMC_4BIT)
    DEKI_EXPORT
    DEKI_TOOLTIP("SDMMC 4-bit mode: data line 3. Ignored in 1-bit mode.")
    DEKI_RANGE(0, 48)
    int32_t d3Pin = 13;

    DEKI_EXPORT
    DEKI_TOOLTIP("SDMMC mode: the clock line driving the card.")
    DEKI_RANGE(0, 48)
    int32_t clkPin = 18;

    DEKI_EXPORT
    DEKI_TOOLTIP("Card-detect pin, if the socket has the switch for it. -1 when it does not, and the card is then assumed present.")
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

}  // namespace DekiSdCard

