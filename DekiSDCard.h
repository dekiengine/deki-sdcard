#pragma once

#include "IDekiSDCard.h"
#include "DekiSDCardPackage.h"

namespace DekiSdCard
{

/// Makes the platform's SD card implementation. A platform integration package
/// registers a factory with SetFactory() at boot; SDCardComponent calls
/// Create() to get a new IDekiSDCard without knowing the concrete type.
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

}  // namespace DekiSdCard
