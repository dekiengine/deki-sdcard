// Package entry point for deki-sdcard.
#include "DekiSDCardPackage.h"
#include <deki/interop/Plugin.h>
#include <deki/LogSystem.h>

extern void DekiSDCardRegisterComponents();
extern int DekiSDCardGetAutoComponentCount();
extern const Deki::ComponentMeta* DekiSDCardGetAutoComponentMeta(int index);

namespace DekiSdCard
{

#ifdef DEKI_EDITOR
#endif

static bool s_SDCardRegistered = false;

}  // namespace DekiSdCard
// The exports below are C symbols at global scope; the package's own
// registration helpers and statics live in its namespace.
using namespace DekiSdCard;

extern "C"
{
    DEKI_SDCARD_API int DekiSDCardEnsureRegistered(void)
    {
#ifdef DEKI_EDITOR
        if (s_SDCardRegistered)
        {
            return ::DekiSDCardGetAutoComponentCount();
        }
        s_SDCardRegistered = true;
        ::DekiSDCardRegisterComponents();
        return ::DekiSDCardGetAutoComponentCount();
#else
        return 0;
#endif
    }

    DEKI_PLUGIN_API const char* DekiPluginGetName(void)
    {
        return "Deki SD Card Package";
    }
    DEKI_PLUGIN_API const char* DekiPluginGetVersion(void)
    {
#ifdef DEKI_PACKAGE_VERSION
        return DEKI_PACKAGE_VERSION;
#else
        return "0.0.0-dev";
#endif
    }
    DEKI_PLUGIN_API int DekiPluginInit(void)
    {
        return 0;
    }
    DEKI_PLUGIN_API void DekiPluginShutdown(void)
    {
        s_SDCardRegistered = false;
    }

#ifdef DEKI_EDITOR
    DEKI_PLUGIN_API int DekiPluginGetComponentCount(void)
    {
        return ::DekiSDCardGetAutoComponentCount();
    }
    DEKI_PLUGIN_API const Deki::ComponentMeta* DekiPluginGetComponentMeta(int index)
    {
        return ::DekiSDCardGetAutoComponentMeta(index);
    }
#else
    DEKI_PLUGIN_API int DekiPluginGetComponentCount(void)
    {
        return 0;
    }
    DEKI_PLUGIN_API const Deki::ComponentMeta* DekiPluginGetComponentMeta(int)
    {
        return nullptr;
    }
#endif

    DEKI_PLUGIN_API void DekiPluginRegisterComponents(void)
    {
#ifdef DEKI_EDITOR
        DekiSDCardEnsureRegistered();
#endif
    }

}  // extern "C"
