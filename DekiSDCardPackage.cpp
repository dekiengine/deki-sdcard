/**
 * @file DekiSDCardPackage.cpp
 * @brief Package entry point for deki-sdcard
 */
#include "DekiSDCardPackage.h"
#include "interop/DekiPlugin.h"
#include "DekiLogSystem.h"

#ifdef DEKI_EDITOR
extern void DekiSDCard_RegisterComponents();
extern int  DekiSDCard_GetAutoComponentCount();
extern const Deki::ComponentMeta* DekiSDCard_GetAutoComponentMeta(int index);
#endif

static bool s_SDCardRegistered = false;

extern "C" {

DEKI_SDCARD_API int DekiSDCard_EnsureRegistered(void)
{
#ifdef DEKI_EDITOR
    if (s_SDCardRegistered) return DekiSDCard_GetAutoComponentCount();
    s_SDCardRegistered = true;
    DekiSDCard_RegisterComponents();
    return DekiSDCard_GetAutoComponentCount();
#else
    return 0;
#endif
}

DEKI_PLUGIN_API const char* DekiPlugin_GetName(void)    { return "Deki SD Card Package"; }
DEKI_PLUGIN_API const char* DekiPlugin_GetVersion(void)
{
#ifdef DEKI_PACKAGE_VERSION
    return DEKI_PACKAGE_VERSION;
#else
    return "0.0.0-dev";
#endif
}
DEKI_PLUGIN_API int  DekiPlugin_Init(void)     { DEKI_LOG_INFO("[deki-sdcard] DekiPlugin_Init"); return 0; }
DEKI_PLUGIN_API void DekiPlugin_Shutdown(void) { s_SDCardRegistered = false; }

#ifdef DEKI_EDITOR
DEKI_PLUGIN_API int  DekiPlugin_GetComponentCount(void) { return DekiSDCard_GetAutoComponentCount(); }
DEKI_PLUGIN_API const Deki::ComponentMeta* DekiPlugin_GetComponentMeta(int index)
{
    return DekiSDCard_GetAutoComponentMeta(index);
}
#else
DEKI_PLUGIN_API int  DekiPlugin_GetComponentCount(void) { return 0; }
DEKI_PLUGIN_API const Deki::ComponentMeta* DekiPlugin_GetComponentMeta(int) { return nullptr; }
#endif

DEKI_PLUGIN_API void DekiPlugin_RegisterComponents(void)
{
#ifdef DEKI_EDITOR
    int n = DekiSDCard_EnsureRegistered();
    DEKI_LOG_INFO("[deki-sdcard] DekiPlugin_RegisterComponents -> %d component(s)", n);
#endif
}


}  // extern "C"
