#pragma once

namespace vx {
namespace app {
namespace usb {

// https://github.com/libsdl-org/SDL/blob/main/src/joystick/usb_ids.h

// Definitions of useful USB VID/PID values

enum
{
    // ------------------------------------------------------------
    // USB Vendor IDs
    // ------------------------------------------------------------

    VENDOR_8BITDO                                     = 0x2DC8,
    VENDOR_AMAZON                                     = 0x1949,
    VENDOR_APPLE                                      = 0x05AC,
    VENDOR_ASTRO                                      = 0x9886,
    VENDOR_ASUS                                       = 0x0B05,
    VENDOR_BACKBONE                                   = 0x358A,
    VENDOR_GAMESIR                                    = 0x3537,
    VENDOR_DRAGONRISE                                 = 0x0079,
    VENDOR_FLYDIGI                                    = 0x04B4,
    VENDOR_GOOGLE                                     = 0x18D1,
    VENDOR_HORI                                       = 0x0F0D,
    VENDOR_HP                                         = 0x03F0,
    VENDOR_HYPERKIN                                   = 0x2E24,
    VENDOR_LOGITECH                                   = 0x046D,
    VENDOR_MADCATZ                                    = 0x0738,
    VENDOR_MAYFLASH                                   = 0x33DF,
    VENDOR_MICROSOFT                                  = 0x045E,
    VENDOR_NACON                                      = 0x146B,
    VENDOR_NACON_ALT                                  = 0x3285,
    VENDOR_NINTENDO                                   = 0x057E,
    VENDOR_NVIDIA                                     = 0x0955,
    VENDOR_PDP                                        = 0x0E6F,
    VENDOR_POWERA                                     = 0x24C6,
    VENDOR_POWERA_ALT                                 = 0x20D6,
    VENDOR_QANBA                                      = 0x2C22,
    VENDOR_RAZER                                      = 0x1532,
    VENDOR_SAITEK                                     = 0x06A3,
    VENDOR_SHANWAN                                    = 0x2563,
    VENDOR_SHANWAN_ALT                                = 0x20BC,
    VENDOR_SONY                                       = 0x054C,
    VENDOR_THRUSTMASTER                               = 0x044F,
    VENDOR_TURTLE_BEACH                               = 0x10F5,
    VENDOR_SWITCH                                     = 0x2563,
    VENDOR_VALVE                                      = 0x28DE,
    VENDOR_ZEROPLUS                                   = 0x0C12,
    VENDOR_RASPBERRYPI                                = 0x2E8A, // Commercial hardware from various companies are registered under this VID
    VENDOR_ZUIKI                                      = 0x33DD,

    // ------------------------------------------------------------
    // USB Product IDs
    // ------------------------------------------------------------

    PRODUCT_8BITDO_SF30_PRO                           = 0x6000,    // B + START
    PRODUCT_8BITDO_SF30_PRO_BT                        = 0x6100,    // B + START
    PRODUCT_8BITDO_SN30_PRO                           = 0x6001,    // B + START
    PRODUCT_8BITDO_SN30_PRO_BT                        = 0x6101,    // B + START
    PRODUCT_8BITDO_PRO_2                              = 0x6003,    // mode switch to D
    PRODUCT_8BITDO_PRO_2_BT                           = 0x6006,    // mode switch to D
    PRODUCT_8BITDO_PRO_3                              = 0x6009,    // mode switch to D
    PRODUCT_8BITDO_ULTIMATE2_WIRELESS                 = 0x6012,    // mode switch to BT
    PRODUCT_AMAZON_LUNA_CONTROLLER                    = 0x0419,
    PRODUCT_ASTRO_C40_XBOX360                         = 0x0024,
    PRODUCT_BACKBONE_ONE_IOS                          = 0x0103,
    PRODUCT_BACKBONE_ONE_IOS_PS5                      = 0x0104,
    PRODUCT_BDA_XB1_CLASSIC                           = 0x581A,
    PRODUCT_BDA_XB1_FIGHTPAD                          = 0x791A,
    PRODUCT_BDA_XB1_SPECTRA_PRO                       = 0x592A,
    PRODUCT_GOOGLE_STADIA_CONTROLLER                  = 0x9400,
    PRODUCT_EVORETRO_GAMECUBE_ADAPTER1                = 0x1843,
    PRODUCT_EVORETRO_GAMECUBE_ADAPTER2                = 0x1844,
    PRODUCT_EVORETRO_GAMECUBE_ADAPTER3                = 0x1846,
    PRODUCT_FLYDIGI_GAMEPAD                           = 0x2412,
    PRODUCT_HORI_FIGHTING_STICK_ALPHA_PS4             = 0x011C,
    PRODUCT_HORI_FIGHTING_STICK_ALPHA_PS5             = 0x0184,
    PRODUCT_HORI_STEAM_CONTROLLER                     = 0x01AB,
    PRODUCT_HORI_STEAM_CONTROLLER_BT                  = 0x0196,
    PRODUCT_LOGITECH_F310                             = 0xC216,
    PRODUCT_LOGITECH_CHILLSTREAM                      = 0xCAD1,
    PRODUCT_MADCATZ_SAITEK_SIDE_PANEL_CONTROL_DECK    = 0x2218,
    PRODUCT_NACON_REVOLUTION_5_PRO_PS4_WIRELESS       = 0x0D16,
    PRODUCT_NACON_REVOLUTION_5_PRO_PS4_WIRED          = 0x0D17,
    PRODUCT_NACON_REVOLUTION_5_PRO_PS5_WIRELESS       = 0x0D18,
    PRODUCT_NACON_REVOLUTION_5_PRO_PS5_WIRED          = 0x0D19,
    PRODUCT_NACON_REVOLUTION_X_UNLIMITED_BT           = 0x0689,
    PRODUCT_NINTENDO_GAMECUBE_ADAPTER                 = 0x0337,
    PRODUCT_NINTENDO_N64_CONTROLLER                   = 0x2019,
    PRODUCT_NINTENDO_SEGA_GENESIS_CONTROLLER          = 0x201E,
    PRODUCT_NINTENDO_SNES_CONTROLLER                  = 0x2017,
    PRODUCT_NINTENDO_SWITCH_JOYCON_GRIP               = 0x200E,
    PRODUCT_NINTENDO_SWITCH_JOYCON_LEFT               = 0x2006,
    PRODUCT_NINTENDO_SWITCH_JOYCON_PAIR               = 0x2008, // Used by joycond
    PRODUCT_NINTENDO_SWITCH_JOYCON_RIGHT              = 0x2007,
    PRODUCT_NINTENDO_SWITCH_PRO                       = 0x2009,
    PRODUCT_NINTENDO_SWITCH2_GAMECUBE_CONTROLLER      = 0x2073,
    PRODUCT_NINTENDO_SWITCH2_JOYCON_LEFT              = 0x2067,
    PRODUCT_NINTENDO_SWITCH2_JOYCON_PAIR              = 0x2068,
    PRODUCT_NINTENDO_SWITCH2_JOYCON_RIGHT             = 0x2066,
    PRODUCT_NINTENDO_SWITCH2_PRO                      = 0x2069,
    PRODUCT_NINTENDO_WII_REMOTE                       = 0x0306,
    PRODUCT_NINTENDO_WII_REMOTE2                      = 0x0330,
    PRODUCT_NVIDIA_SHIELD_CONTROLLER_V103             = 0x7210,
    PRODUCT_NVIDIA_SHIELD_CONTROLLER_V104             = 0x7214,
    PRODUCT_PDP_ROCK_CANDY                            = 0x0246,
    PRODUCT_PDP_REALMZ_WIRELESS                       = 0x018C,
    PRODUCT_POWERA_MINI                               = 0x541A,
    PRODUCT_RAZER_ATROX                               = 0x0A00,
    PRODUCT_RAZER_KITSUNE                             = 0x1012,
    PRODUCT_RAZER_PANTHERA                            = 0x0401,
    PRODUCT_RAZER_PANTHERA_EVO                        = 0x1008,
    PRODUCT_RAZER_RAIJU                               = 0x1000,
    PRODUCT_RAZER_TOURNAMENT_EDITION_USB              = 0x1007,
    PRODUCT_RAZER_TOURNAMENT_EDITION_BLUETOOTH        = 0x100A,
    PRODUCT_RAZER_ULTIMATE_EDITION_USB                = 0x1004,
    PRODUCT_RAZER_ULTIMATE_EDITION_BLUETOOTH          = 0x1009,
    PRODUCT_RAZER_WOLVERINE_V2                        = 0x0A29,
    PRODUCT_RAZER_WOLVERINE_V2_CHROMA                 = 0x0A2E,
    PRODUCT_RAZER_WOLVERINE_V2_PRO_PS5_WIRED          = 0x100B,
    PRODUCT_RAZER_WOLVERINE_V2_PRO_PS5_WIRELESS       = 0x100C,
    PRODUCT_RAZER_WOLVERINE_V2_PRO_XBOX_WIRED         = 0x1010,
    PRODUCT_RAZER_WOLVERINE_V2_PRO_XBOX_WIRELESS      = 0x1011,
    PRODUCT_RAZER_WOLVERINE_V3_PRO                    = 0x0A3F,
    PRODUCT_SAITEK_CYBORG_V3                          = 0xF622,
    PRODUCT_SHANWAN_DS3                               = 0x0523,
    PRODUCT_SONY_DS3                                  = 0x0268,
    PRODUCT_SONY_DS4                                  = 0x05C4,
    PRODUCT_SONY_DS4_DONGLE                           = 0x0BA0,
    PRODUCT_SONY_DS4_SLIM                             = 0x09CC,
    PRODUCT_SONY_DS4_STRIKEPAD                        = 0x05C5,
    PRODUCT_SONY_DS5                                  = 0x0CE6,
    PRODUCT_SONY_DS5_EDGE                             = 0x0DF2,
    PRODUCT_STEALTH_ULTRA_WIRED                       = 0x7073,
    PRODUCT_SWITCH_RETROBIT_CONTROLLER                = 0x0575,
    PRODUCT_THRUSTMASTER_ESWAPX_PRO_PS4               = 0xD00E,
    PRODUCT_THRUSTMASTER_T_FLIGHT_HOTAS_ONE           = 0xB68C,
    PRODUCT_VALVE_STEAM_CONTROLLER_DONGLE             = 0x1142,
    PRODUCT_VICTRIX_FS_PRO                            = 0x0203,
    PRODUCT_VICTRIX_FS_PRO_V2                         = 0x0207,
    PRODUCT_XBOX360_XUSB_CONTROLLER                   = 0x02A1, // XUSB driver software PID
    PRODUCT_XBOX360_WIRED_CONTROLLER                  = 0x028E,
    PRODUCT_XBOX360_WIRELESS_RECEIVER                 = 0x0719,
    PRODUCT_XBOX360_WIRELESS_RECEIVER_THIRDPARTY1     = 0x02A9,
    PRODUCT_XBOX360_WIRELESS_RECEIVER_THIRDPARTY2     = 0x0291,
    PRODUCT_XBOX_ONE_ADAPTIVE                         = 0x0B0A,
    PRODUCT_XBOX_ONE_ADAPTIVE_BLUETOOTH               = 0x0B0C,
    PRODUCT_XBOX_ONE_ADAPTIVE_BLE                     = 0x0B21,
    PRODUCT_XBOX_ONE_ELITE_SERIES_1                   = 0x02E3,
    PRODUCT_XBOX_ONE_ELITE_SERIES_2                   = 0x0B00,
    PRODUCT_XBOX_ONE_ELITE_SERIES_2_BLUETOOTH         = 0x0B05,
    PRODUCT_XBOX_ONE_ELITE_SERIES_2_BLE               = 0x0B22,
    PRODUCT_XBOX_ONE_S                                = 0x02EA,
    PRODUCT_XBOX_ONE_S_REV1_BLUETOOTH                 = 0x02E0,
    PRODUCT_XBOX_ONE_S_REV2_BLUETOOTH                 = 0x02FD,
    PRODUCT_XBOX_ONE_S_REV2_BLE                       = 0x0B20,
    PRODUCT_XBOX_SERIES_X                             = 0x0B12,
    PRODUCT_XBOX_SERIES_X_BLE                         = 0x0B13,
    PRODUCT_XBOX_ONE_XBOXGIP_CONTROLLER               = 0x02FF, // XBOXGIP driver software PID
    PRODUCT_STEAM_VIRTUAL_GAMEPAD                     = 0x11FF,
    PRODUCT_HANDHELDLEGEND_SINPUT_GENERIC             = 0x10C6,
    PRODUCT_HANDHELDLEGEND_PROGCC                     = 0x10DF,
    PRODUCT_HANDHELDLEGEND_GCULTIMATE                 = 0x10DD,
    PRODUCT_BONZIRICHANNEL_FIREBIRD                   = 0x10E0, 
    PRODUCT_ZUIKI_MASCON_PRO                          = 0x0006,
    PRODUCT_VOIDGAMING_PS4FIREBIRD                    = 0x10E5,

    // ------------------------------------------------------------
    // USB Usage Pages
    // ------------------------------------------------------------

    USAGEPAGE_GENERIC_DESKTOP                         = 0x0001,
    USAGEPAGE_BUTTON                                  = 0x0009,
    USAGEPAGE_VENDOR_FLYDIGI                          = 0xFFA0,

    // ------------------------------------------------------------
    // USB Usages (Generic Desktop)
    // ------------------------------------------------------------

    USAGE_GENERIC_POINTER                             = 0x0001,
    USAGE_GENERIC_MOUSE                               = 0x0002,
    USAGE_GENERIC_JOYSTICK                            = 0x0004,
    USAGE_GENERIC_GAMEPAD                             = 0x0005,
    USAGE_GENERIC_KEYBOARD                            = 0x0006,
    USAGE_GENERIC_KEYPAD                              = 0x0007,
    USAGE_GENERIC_MULTIAXISCONTROLLER                 = 0x0008,
    USAGE_GENERIC_X                                   = 0x0030,
    USAGE_GENERIC_Y                                   = 0x0031,
    USAGE_GENERIC_Z                                   = 0x0032,
    USAGE_GENERIC_RX                                  = 0x0033,
    USAGE_GENERIC_RY                                  = 0x0034,
    USAGE_GENERIC_RZ                                  = 0x0035,
    USAGE_GENERIC_SLIDER                              = 0x0036,
    USAGE_GENERIC_DIAL                                = 0x0037,
    USAGE_GENERIC_WHEEL                               = 0x0038,
    USAGE_GENERIC_HAT                                 = 0x0039,

    // ------------------------------------------------------------
    // Bluetooth Assigned Company Identifiers
    // ------------------------------------------------------------

    BLUETOOTH_VENDOR_AMAZON                           = 0x0171,
    BLUETOOTH_PRODUCT_LUNA_CONTROLLER                 = 0x0419
};

} // namespace usb
} // namespace app
} // namespace vx