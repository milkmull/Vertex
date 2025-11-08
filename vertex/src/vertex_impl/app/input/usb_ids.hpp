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

    vendor_8bitdo                                     = 0x2DC8,
    vendor_amazon                                     = 0x1949,
    vendor_apple                                      = 0x05AC,
    vendor_astro                                      = 0x9886,
    vendor_asus                                       = 0x0B05,
    vendor_backbone                                   = 0x358A,
    vendor_gamesir                                    = 0x3537,
    vendor_dragonrise                                 = 0x0079,
    vendor_flydigi                                    = 0x04B4,
    vendor_google                                     = 0x18D1,
    vendor_hori                                       = 0x0F0D,
    vendor_hp                                         = 0x03F0,
    vendor_hyperkin                                   = 0x2E24,
    vendor_logitech                                   = 0x046D,
    vendor_madcatz                                    = 0x0738,
    vendor_mayflash                                   = 0x33DF,
    vendor_microsoft                                  = 0x045E,
    vendor_nacon                                      = 0x146B,
    vendor_nacon_alt                                  = 0x3285,
    vendor_nintendo                                   = 0x057E,
    vendor_nvidia                                     = 0x0955,
    vendor_pdp                                        = 0x0E6F,
    vendor_powera                                     = 0x24C6,
    vendor_powera_alt                                 = 0x20D6,
    vendor_qanba                                      = 0x2C22,
    vendor_razer                                      = 0x1532,
    vendor_saitek                                     = 0x06A3,
    vendor_shanwan                                    = 0x2563,
    vendor_shanwan_alt                                = 0x20BC,
    vendor_sony                                       = 0x054C,
    vendor_thrustmaster                               = 0x044F,
    vendor_turtle_beach                               = 0x10F5,
    vendor_switch                                     = 0x2563,
    vendor_valve                                      = 0x28DE,
    vendor_zeroplus                                   = 0x0C12,
    vendor_raspberrypi                                = 0x2E8A, // Commercial hardware from various companies are registered under this VID
    vendor_zuiki                                      = 0x33DD,

    // ------------------------------------------------------------
    // USB Product IDs
    // ------------------------------------------------------------

    product_8bitdo_sf30_pro                           = 0x6000,    // B + START
    product_8bitdo_sf30_pro_bt                        = 0x6100,    // B + START
    product_8bitdo_sn30_pro                           = 0x6001,    // B + START
    product_8bitdo_sn30_pro_bt                        = 0x6101,    // B + START
    product_8bitdo_pro_2                              = 0x6003,    // mode switch to D
    product_8bitdo_pro_2_bt                           = 0x6006,    // mode switch to D
    product_8bitdo_pro_3                              = 0x6009,    // mode switch to D
    product_8bitdo_ultimate2_wireless                 = 0x6012,    // mode switch to BT
    product_amazon_luna_controller                    = 0x0419,
    product_astro_c40_xbox360                         = 0x0024,
    product_backbone_one_ios                          = 0x0103,
    product_backbone_one_ios_ps5                      = 0x0104,
    product_bda_xb1_classic                           = 0x581A,
    product_bda_xb1_fightpad                          = 0x791A,
    product_bda_xb1_spectra_pro                       = 0x592A,
    product_google_stadia_controller                  = 0x9400,
    product_evoretro_gamecube_adapter1                = 0x1843,
    product_evoretro_gamecube_adapter2                = 0x1844,
    product_evoretro_gamecube_adapter3                = 0x1846,
    product_flydigi_gamepad                           = 0x2412,
    product_hori_fighting_stick_alpha_ps4             = 0x011C,
    product_hori_fighting_stick_alpha_ps5             = 0x0184,
    product_hori_steam_controller                     = 0x01AB,
    product_hori_steam_controller_bt                  = 0x0196,
    product_logitech_f310                             = 0xC216,
    product_logitech_chillstream                      = 0xCAD1,
    product_madcatz_saitek_side_panel_control_deck    = 0x2218,
    product_nacon_revolution_5_pro_ps4_wireless       = 0x0D16,
    product_nacon_revolution_5_pro_ps4_wired          = 0x0D17,
    product_nacon_revolution_5_pro_ps5_wireless       = 0x0D18,
    product_nacon_revolution_5_pro_ps5_wired          = 0x0D19,
    product_nacon_revolution_x_unlimited_bt           = 0x0689,
    product_nintendo_gamecube_adapter                 = 0x0337,
    product_nintendo_n64_controller                   = 0x2019,
    product_nintendo_sega_genesis_controller          = 0x201E,
    product_nintendo_snes_controller                  = 0x2017,
    product_nintendo_switch_joycon_grip               = 0x200E,
    product_nintendo_switch_joycon_left               = 0x2006,
    product_nintendo_switch_joycon_pair               = 0x2008, // Used by joycond
    product_nintendo_switch_joycon_right              = 0x2007,
    product_nintendo_switch_pro                       = 0x2009,
    product_nintendo_switch2_gamecube_controller      = 0x2073,
    product_nintendo_switch2_joycon_left              = 0x2067,
    product_nintendo_switch2_joycon_pair              = 0x2068,
    product_nintendo_switch2_joycon_right             = 0x2066,
    product_nintendo_switch2_pro                      = 0x2069,
    product_nintendo_wii_remote                       = 0x0306,
    product_nintendo_wii_remote2                      = 0x0330,
    product_nvidia_shield_controller_v103             = 0x7210,
    product_nvidia_shield_controller_v104             = 0x7214,
    product_pdp_rock_candy                            = 0x0246,
    product_pdp_realmz_wireless                       = 0x018C,
    product_powera_mini                               = 0x541A,
    product_razer_atrox                               = 0x0A00,
    product_razer_kitsune                             = 0x1012,
    product_razer_panthera                            = 0x0401,
    product_razer_panthera_evo                        = 0x1008,
    product_razer_raiju                               = 0x1000,
    product_razer_tournament_edition_usb              = 0x1007,
    product_razer_tournament_edition_bluetooth        = 0x100A,
    product_razer_ultimate_edition_usb                = 0x1004,
    product_razer_ultimate_edition_bluetooth          = 0x1009,
    product_razer_wolverine_v2                        = 0x0A29,
    product_razer_wolverine_v2_chroma                 = 0x0A2E,
    product_razer_wolverine_v2_pro_ps5_wired          = 0x100B,
    product_razer_wolverine_v2_pro_ps5_wireless       = 0x100C,
    product_razer_wolverine_v2_pro_xbox_wired         = 0x1010,
    product_razer_wolverine_v2_pro_xbox_wireless      = 0x1011,
    product_razer_wolverine_v3_pro                    = 0x0A3F,
    product_saitek_cyborg_v3                          = 0xF622,
    product_shanwan_ds3                               = 0x0523,
    product_sony_ds3                                  = 0x0268,
    product_sony_ds4                                  = 0x05C4,
    product_sony_ds4_dongle                           = 0x0BA0,
    product_sony_ds4_slim                             = 0x09CC,
    product_sony_ds4_strikepad                        = 0x05C5,
    product_sony_ds5                                  = 0x0CE6,
    product_sony_ds5_edge                             = 0x0DF2,
    product_stealth_ultra_wired                       = 0x7073,
    product_switch_retrobit_controller                = 0x0575,
    product_thrustmaster_eswapx_pro_ps4               = 0xD00E,
    product_thrustmaster_t_flight_hotas_one           = 0xB68C,
    product_valve_steam_controller_dongle             = 0x1142,
    product_victrix_fs_pro                            = 0x0203,
    product_victrix_fs_pro_v2                         = 0x0207,
    product_xbox360_xusb_controller                   = 0x02A1, // XUSB driver software PID
    product_xbox360_wired_controller                  = 0x028E,
    product_xbox360_wireless_receiver                 = 0x0719,
    product_xbox360_wireless_receiver_thirdparty1     = 0x02A9,
    product_xbox360_wireless_receiver_thirdparty2     = 0x0291,
    product_xbox_one_adaptive                         = 0x0B0A,
    product_xbox_one_adaptive_bluetooth               = 0x0B0C,
    product_xbox_one_adaptive_ble                     = 0x0B21,
    product_xbox_one_elite_series_1                   = 0x02E3,
    product_xbox_one_elite_series_2                   = 0x0B00,
    product_xbox_one_elite_series_2_bluetooth         = 0x0B05,
    product_xbox_one_elite_series_2_ble               = 0x0B22,
    product_xbox_one_s                                = 0x02EA,
    product_xbox_one_s_rev1_bluetooth                 = 0x02E0,
    product_xbox_one_s_rev2_bluetooth                 = 0x02FD,
    product_xbox_one_s_rev2_ble                       = 0x0B20,
    product_xbox_series_x                             = 0x0B12,
    product_xbox_series_x_ble                         = 0x0B13,
    product_xbox_one_xboxgip_controller               = 0x02FF, // XBOXGIP driver software PID
    product_steam_virtual_gamepad                     = 0x11FF,
    product_handheldlegend_sinput_generic             = 0x10C6,
    product_handheldlegend_progcc                     = 0x10DF,
    product_handheldlegend_gcultimate                 = 0x10DD,
    product_bonzirichannel_firebird                   = 0x10E0, 
    product_zuiki_mascon_pro                          = 0x0006,
    product_voidgaming_ps4firebird                    = 0x10E5,

    // ------------------------------------------------------------
    // USB Usage Pages
    // ------------------------------------------------------------

    usagepage_generic_desktop                         = 0x0001,
    usagepage_button                                  = 0x0009,
    usagepage_vendor_flydigi                          = 0xFFA0,

    // ------------------------------------------------------------
    // USB Usages (Generic Desktop)
    // ------------------------------------------------------------

    usage_generic_pointer                             = 0x0001,
    usage_generic_mouse                               = 0x0002,
    usage_generic_joystick                            = 0x0004,
    usage_generic_gamepad                             = 0x0005,
    usage_generic_keyboard                            = 0x0006,
    usage_generic_keypad                              = 0x0007,
    usage_generic_multiaxiscontroller                 = 0x0008,
    usage_generic_x                                   = 0x0030,
    usage_generic_y                                   = 0x0031,
    usage_generic_z                                   = 0x0032,
    usage_generic_rx                                  = 0x0033,
    usage_generic_ry                                  = 0x0034,
    usage_generic_rz                                  = 0x0035,
    usage_generic_slider                              = 0x0036,
    usage_generic_dial                                = 0x0037,
    usage_generic_wheel                               = 0x0038,
    usage_generic_hat                                 = 0x0039,

    // ------------------------------------------------------------
    // Bluetooth Assigned Company Identifiers
    // ------------------------------------------------------------

    bluetooth_vendor_amazon                           = 0x0171,
    bluetooth_product_luna_controller                 = 0x0419
};

} // namespace usb
} // namespace app
} // namespace vx