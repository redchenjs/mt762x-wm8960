MT76x8 WM8960 ALSA SoC machine driver
=====================================

ALSA SoC machine driver for MT7628/88 SoC with WM8960 CODEC chip.

## Requirements

* OpenWrt with kernel 4.9 or later.

## Preparing for build

* Add `mt76x8-wm8960` folder to OpenWrt folder `package/kernel`.
* Modify your DTS file according to `example.dts`.

## Configuring OpenWrt

`make menuconfig`

* Navigate to `> Kernel modules > Sound Support`.
* Enable `kmod-sound-core` and `kmod-sound-mt76x8-wm8960`.

<img src="docs/menuconfig.png">

## Known issues

1. Capture does not work.
2. Need to turn on `"Left Output Mixer PCM"` and `"Right Output Mixer PCM"` via `alsamixer` or `amixer` before playback.

## About WM8960 MCLK

WM8960 could get MCLK from either an externel clock source or MT7628/88's REFCLK pin.

Modify your dts file as following can enable MT7628/88's REFCLK pin:

```
refclk {
    ralink,group = "refclk";
    ralink,function = "refclk";
};
```

## WM8960 block diagram

<img src="docs/wm8960blkdiag.png">
