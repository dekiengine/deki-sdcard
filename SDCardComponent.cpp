#include "SDCardComponent.h"
#include "DekiSDCard.h"
#include "providers/DekiFileSystem.h"
#include "IDekiSDCard.h"
#include "PackageConfig.h"
#include "assets/AssetManager.h"
#include "assets/AssetLookupTable.h"
#include "assets/AssetPackReader.h"
#include "DekiLogSystem.h"
#include "DekiEngine.h"
#include "PrefabSystem.h"

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
        DekiEngine::GetInstance().GetPrefabSystem().MarkPersistent(GetOwner());
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

    PackageConfig config;
    config.packageId = "sd_card";
    config.enabled = true;
    config.settings["auto_mount"] = "false";

    if (mode == SDCardMode::SDMMC_1BIT || mode == SDCardMode::SDMMC_4BIT)
    {
        config.settings["mode"] = (mode == SDCardMode::SDMMC_4BIT) ? "SDMMC_4BIT" : "SDMMC_1BIT";
        config.settings["sdmmc_mhz"] = std::to_string(sdmmc_mhz);
        config.pins["CLK"] = clk_pin;
        config.pins["CMD"] = cmd_pin;
        config.pins["D0"] = d0_pin;
        if (mode == SDCardMode::SDMMC_4BIT)
        {
            config.pins["D1"] = d1_pin;
            config.pins["D2"] = d2_pin;
            config.pins["D3"] = d3_pin;
        }
        if (cd_pin >= 0)
        {
            config.pins["CD"] = cd_pin;
        }
    }
    else
    {
        config.settings["mode"] = "SPI";
        config.pins["MOSI"] = mosi_pin;
        config.pins["MISO"] = miso_pin;
        config.pins["CLK"] = clk_pin;
        config.pins["CS"] = cs_pin;
        if (cd_pin >= 0)
        {
            config.pins["CD"] = cd_pin;
        }
        config.settings["spi_mhz"] = std::to_string(spi_mhz);
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

    IDekiFileSystem* sdFs = s_SDCardPackage->GetFileSystem();
    if (sdFs)
    {
        DekiFileSystem::RegisterFileSystem("S:/", sdFs);
        DekiFileSystem::SetDefaultFileSystem(sdFs);
    }

    m_Mounted = true;

    Deki::AssetManager::Get()->SetCacheDirectory("S:/");

    LoadAssetLookupTable();

    return true;
}

void SDCardComponent::Unmount()
{
    if (!m_Mounted)
        return;

    DekiFileSystem::UnregisterFileSystem("S:/");

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

void SDCardComponent::LoadAssetLookupTable()
{
    const char* tablePath = "S:/asset_table.bin";

    IDekiFileSystem* fs = DekiFileSystem::GetFileSystemForPath(tablePath);
    if (!fs)
    {
        DEKI_LOG_WARNING("SDCardComponent: No filesystem for asset table");
        return;
    }

    auto handle = fs->OpenFile(tablePath, IDekiFileSystem::OpenMode::READ_BINARY);
    if (!handle)
    {
        DEKI_LOG_WARNING("SDCardComponent: asset_table.bin not found at %s", tablePath);
        return;
    }

    long size = fs->GetFileSize(handle);
    if (size <= 0)
    {
        fs->CloseFile(handle);
        DEKI_LOG_WARNING("SDCardComponent: asset_table.bin is empty");
        return;
    }

    static uint8_t* s_AssetTableData = nullptr;
    static size_t s_AssetTableSize = 0;

    if (s_AssetTableData)
    {
        delete[] s_AssetTableData;
    }

    s_AssetTableSize = static_cast<size_t>(size);
    s_AssetTableData = new uint8_t[s_AssetTableSize];

    size_t bytesRead = fs->ReadFile(handle, s_AssetTableData, s_AssetTableSize);
    fs->CloseFile(handle);

    if (bytesRead != s_AssetTableSize)
    {
        DEKI_LOG_ERROR("SDCardComponent: Failed to read asset_table.bin (read %zu of %zu)",
                       bytesRead, s_AssetTableSize);
        delete[] s_AssetTableData;
        s_AssetTableData = nullptr;
        return;
    }

    if (Deki::AssetManager::Get()->LoadAssetLookupTable(s_AssetTableData, s_AssetTableSize))
    {
        DEKI_LOG_INTERNAL("SDCardComponent: Loaded asset_table.bin (%u entries)",
                      Deki::AssetLookupTable::GetEntryCount());

        Deki::AssetPackReader::Instance().LoadPackIndex("S:/pack_index.bin");
    }
    else
    {
        DEKI_LOG_ERROR("SDCardComponent: Failed to parse asset_table.bin");
    }
}
