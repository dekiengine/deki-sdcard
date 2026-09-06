#pragma once

#include "IDekiSDCard.h"
#include "DekiSDCardPackage.h"

/**
 * @brief SD Card Provider for injectable platform-specific SD card implementations
 *
 * A platform integration package registers a factory via SetFactory() at boot.
 * SDCardComponent calls Create() to obtain a fresh IDekiSDCard instance
 * without knowing the concrete type.
 */
class DEKI_SDCARD_API DekiSDCard
{
public:
    using Factory = IDekiSDCard* (*)();

    static void SetFactory(Factory factory);
    static IDekiSDCard* Create();
    static bool HasFactory();

private:
    static Factory s_Factory;
};
