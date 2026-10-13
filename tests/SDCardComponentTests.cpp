// SDCardComponent mounts the card from the boot scene. The pins and speed it
// passes on depend on how the card is wired (SPI, or SDMMC with one or four
// data lines); a mounted card becomes S:/; and whether a missing card stops
// the boot is the scene's choice ("required"). The card is a fake made by the
// factory a platform package would register.

#include <gtest/gtest.h>

#include "DekiSDCard.h"
#include "SDCardComponent.h"

#include <deki/PackageConfig.h>
#include <deki/providers/FileSystem.h>
#include <deki/providers/IFileSystem.h>

using namespace DekiSdCard;

namespace
{

// A file system that is only ever compared by address.
class NullFileSystem : public Deki::IFileSystem
{
public:
    bool Initialize() override { return true; }
    void Shutdown() override {}
    FileHandle OpenFile(const char*, OpenMode) override { return nullptr; }
    void CloseFile(FileHandle) override {}
    size_t ReadFile(FileHandle, void*, size_t) override { return 0; }
    size_t WriteFile(FileHandle, const void*, size_t) override { return 0; }
    long SeekFile(FileHandle, long, SeekOrigin) override { return -1; }
    long TellFile(FileHandle) override { return -1; }
    long GetFileSize(FileHandle) override { return -1; }
    bool FileExists(const char*) override { return false; }
    bool ConvertPath(const char*, char*, size_t) override { return false; }
};

class FakeCard : public IDekiSDCard
{
public:
    Deki::PackageConfig config;
    bool failInitialize = false;
    bool failMount = false;
    int mounts = 0;
    int unmounts = 0;
    NullFileSystem fs;

    const char* GetPackageId() const override { return "sd_card"; }
    const char* GetPackageName() const override { return "Fake SD"; }
    void Configure(const Deki::PackageConfig& c) override { config = c; }
    bool Initialize() override { return !failInitialize; }
    void Shutdown() override {}
    void Update(float) override {}
    Deki::PackageState GetState() const override { return Deki::PackageState::Running; }
    const char* GetLastError() const override { return ""; }

    bool Mount() override
    {
        ++mounts;
        return !failMount;
    }
    void Unmount() override { ++unmounts; }
    SDCardState GetCardState() const override { return SDCardState::Mounted; }
    bool IsCardInserted() const override { return true; }
    uint64_t GetTotalBytes() const override { return 0; }
    uint64_t GetFreeBytes() const override { return 0; }
    Deki::IFileSystem* GetFileSystem() override { return &fs; }
    const char* GetMountPoint() const override { return "/sdcard"; }
    SDCardMode GetMode() const override { return SDCardMode::SPI; }
};

// The component keeps its driver for the life of the program, as on a
// device, so every test sees this one card.
FakeCard g_Card;

IDekiSDCard* CreateCard()
{
    return &g_Card;
}

class SDCardTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        g_Card.config = Deki::PackageConfig{};
        g_Card.failInitialize = false;
        g_Card.failMount = false;
        g_Card.mounts = 0;
        g_Card.unmounts = 0;
        DekiSDCard::SetFactory(&CreateCard);
    }
    void TearDown() override { DekiSDCard::SetFactory(nullptr); }

    static bool RunSetup(SDCardComponent& c)
    {
        bool reported = false;
        c.Setup([&](bool ok) { reported = ok; });
        return reported;
    }
};

}  // namespace

TEST_F(SDCardTest, SpiPassesItsFourPinsAndSpeed)
{
    SDCardComponent c;
    c.mode = SDCardMode::SPI;
    c.mosiPin = 11;
    c.misoPin = 13;
    c.clkPin = 12;
    c.csPin = 10;
    c.spiHz = 10000000;
    ASSERT_TRUE(RunSetup(c));
    EXPECT_EQ(g_Card.config.GetString("mode"), "SPI");
    EXPECT_EQ(g_Card.config.GetPin("MOSI"), 11);
    EXPECT_EQ(g_Card.config.GetPin("MISO"), 13);
    EXPECT_EQ(g_Card.config.GetPin("CLK"), 12);
    EXPECT_EQ(g_Card.config.GetPin("CS"), 10);
    EXPECT_EQ(g_Card.config.GetInt("spiHz"), 10000000);
    EXPECT_EQ(g_Card.config.GetPin("CD"), -1);  // no card-detect pin
    EXPECT_EQ(g_Card.config.GetPin("D0"), -1);  // nothing from SDMMC
    c.Unmount();
}

TEST_F(SDCardTest, SdmmcOneBitUsesOneDataLine)
{
    SDCardComponent c;
    c.mode = SDCardMode::SDMMC1Bit;
    c.cdPin = 7;
    ASSERT_TRUE(RunSetup(c));
    EXPECT_EQ(g_Card.config.GetString("mode"), "SDMMC1Bit");
    EXPECT_EQ(g_Card.config.GetPin("D0"), c.d0Pin);
    EXPECT_EQ(g_Card.config.GetPin("D1"), -1);
    EXPECT_EQ(g_Card.config.GetPin("CD"), 7);
    EXPECT_EQ(g_Card.config.GetPin("MOSI"), -1);
    c.Unmount();
}

TEST_F(SDCardTest, SdmmcFourBitUsesFour)
{
    SDCardComponent c;
    c.mode = SDCardMode::SDMMC4Bit;
    c.sdmmcHz = 40000000;
    ASSERT_TRUE(RunSetup(c));
    EXPECT_EQ(g_Card.config.GetString("mode"), "SDMMC4Bit");
    EXPECT_EQ(g_Card.config.GetPin("D3"), c.d3Pin);
    EXPECT_EQ(g_Card.config.GetInt("sdmmcHz"), 40000000);
    c.Unmount();
}

TEST_F(SDCardTest, AMountedCardIsSDrive)
{
    SDCardComponent c;
    ASSERT_TRUE(RunSetup(c));
    EXPECT_EQ(Deki::FileSystem::GetFileSystemForPath("S:/assets/x.bin"), &g_Card.fs);

    c.Unmount();
    EXPECT_NE(Deki::FileSystem::GetFileSystemForPath("S:/assets/x.bin"), &g_Card.fs);
    EXPECT_EQ(g_Card.unmounts, 1);
}

TEST_F(SDCardTest, UnmountingPutsTheDefaultFileSystemBack)
{
    // Left as the default, the shut-down card would take every path that
    // has no mount prefix.
    NullFileSystem before;
    Deki::FileSystem::SetDefaultFileSystem(&before);
    SDCardComponent c;
    ASSERT_TRUE(RunSetup(c));
    EXPECT_EQ(Deki::FileSystem::GetFileSystemForPath("plain.txt"), &g_Card.fs);
    c.Unmount();
    EXPECT_EQ(Deki::FileSystem::GetFileSystemForPath("plain.txt"), &before);
    Deki::FileSystem::SetDefaultFileSystem(nullptr);
}

TEST_F(SDCardTest, MountingTwiceMountsOnce)
{
    SDCardComponent c;
    ASSERT_TRUE(c.Mount());
    ASSERT_TRUE(c.Mount());
    EXPECT_EQ(g_Card.mounts, 1);
    c.Unmount();
}

TEST_F(SDCardTest, ARequiredCardThatWillNotMountStopsTheBoot)
{
    g_Card.failMount = true;
    SDCardComponent c;  // required by default
    EXPECT_FALSE(RunSetup(c));
}

TEST_F(SDCardTest, AnOptionalCardThatWillNotMountDoesNot)
{
    g_Card.failMount = true;
    SDCardComponent c;
    c.required = false;
    EXPECT_TRUE(RunSetup(c));
    EXPECT_NE(Deki::FileSystem::GetFileSystemForPath("S:/x"), &g_Card.fs);
}

TEST_F(SDCardTest, ABackendThatWillNotStartIsAFailedMount)
{
    g_Card.failInitialize = true;
    SDCardComponent c;
    EXPECT_FALSE(c.Mount());
    EXPECT_EQ(g_Card.mounts, 0);
}

TEST_F(SDCardTest, DestroyingAMountedComponentUnmounts)
{
    {
        SDCardComponent c;
        ASSERT_TRUE(RunSetup(c));
    }
    EXPECT_EQ(g_Card.unmounts, 1);
}
