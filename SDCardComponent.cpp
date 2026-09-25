#include "SDCardComponent.h"
#include <deki/providers/Memory.h>
#include "DekiSDCard.h"
#include <deki/providers/FileSystem.h>
#include "IDekiSDCard.h"
#include <deki/PackageConfig.h>
#include <deki/assets/AssetManager.h>
#include <deki/ProjectSettings.h>
#include <deki/Storage.h>
#include <deki/LogSystem.h>
#include <deki/Engine.h>
#include <deki/SceneSystem.h>

namespace DekiSdCard
{

// Static SD card instance
static IDekiSDCard* s_SDCardPackage = nullptr;

SDCardComponent::SDCardComponent()
{
}

SDCardComponent::~SDCardComponent()
{
    if (m_Mounted)
    {
        Unmount();
    }
}

void SDCardComponent::Setup(SetupCallback onComplete)
{
    bool success = Mount();

    if (success && GetOwner())
    {
        Deki::Engine::GetInstance().GetSceneSystem().MarkPersistent(GetOwner());
    }

    // A failed step stops the boot. A board whose card is a slot on the side
    // may prefer to come up without one.
    if (!success && !required)
    {
        DEKI_LOG_WARNING("SDCardComponent: no card mounted; carrying on without one. Assets on S:/ will not load.");
        success = true;
    }

    if (onComplete)
    {
        onComplete(success);
    }
}

bool SDCardComponent::Mount()
{
    if (m_Mounted)
        return true;

    if (!s_SDCardPackage)
    {
        s_SDCardPackage = DekiSDCard::Create();
        if (!s_SDCardPackage)
        {
            DEKI_LOG_ERROR("SDCardComponent: No SD card backend registered");
            return false;
        }
    }

    Deki::PackageConfig config;
    config.packageId = "sd_card";
    config.enabled = true;
    config.settings["auto_mount"] = "false";

    if (mode == SDCardMode::SDMMC_1BIT || mode == SDCardMode::SDMMC_4BIT)
    {
        config.settings["mode"] = (mode == SDCardMode::SDMMC_4BIT) ? "SDMMC_4BIT" : "SDMMC_1BIT";
        config.settings["sdmmcHz"] = std::to_string(sdmmcHz);
        config.pins["CLK"] = clkPin;
        config.pins["CMD"] = cmdPin;
        config.pins["D0"] = d0Pin;
        if (mode == SDCardMode::SDMMC_4BIT)
        {
            config.pins["D1"] = d1Pin;
            config.pins["D2"] = d2Pin;
            config.pins["D3"] = d3Pin;
        }
        if (cdPin >= 0)
        {
            config.pins["CD"] = cdPin;
        }
    }
    else
    {
        config.settings["mode"] = "SPI";
        config.pins["MOSI"] = mosiPin;
        config.pins["MISO"] = misoPin;
        config.pins["CLK"] = clkPin;
        config.pins["CS"] = csPin;
        if (cdPin >= 0)
        {
            config.pins["CD"] = cdPin;
        }
        config.settings["spiHz"] = std::to_string(spiHz);
    }

    s_SDCardPackage->Configure(config);

    if (!s_SDCardPackage->Initialize())
    {
        DEKI_LOG_ERROR("SDCardComponent: Failed to initialize SD card backend");
        return false;
    }

    if (!s_SDCardPackage->Mount())
    {
        DEKI_LOG_ERROR("SDCardComponent: Failed to mount SD card");
        return false;
    }

    Deki::IFileSystem* sdFs = s_SDCardPackage->GetFileSystem();
    if (sdFs)
    {
        Deki::FileSystem::RegisterFileSystem("S:/", sdFs);
        Deki::FileSystem::SetDefaultFileSystem(sdFs);
    }

    m_Mounted = true;

    // A build that keeps its assets on the card finds them now; one that keeps
    // them inside had them loaded at startup, and the card is just storage.
    if (Deki::ProjectSettings::GetAssetStorage() == Deki::Storage::External)
        Deki::AssetManager::Get()->LoadAssetRoot(Deki::Storage::AssetRoot(Deki::Storage::External));

    return true;
}

void SDCardComponent::Unmount()
{
    if (!m_Mounted)
        return;

    Deki::FileSystem::UnregisterFileSystem("S:/");

    if (s_SDCardPackage)
    {
        s_SDCardPackage->Unmount();
        s_SDCardPackage->Shutdown();
    }

    m_Mounted = false;
}

IDekiSDCard* SDCardComponent::GetSDCardPackage()
{
    return s_SDCardPackage;
}

}  // namespace DekiSdCard
