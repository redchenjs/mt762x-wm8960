MT76x8 WM8960 ALSA SoC machine driver
=====================================

ALSA SoC machine driver for MT7628/88 SoC with WM8960 CODEC chip.

## Requirements

* OpenWrt with kernel 4.9 or later.

## Preparing

* Add the `mt76x8-wm8960` folder to the `package/kernel` folder of OpenWrt.
* Modify the target DTS file in the `target/linux/ramips/dts` folder of OpenWrt according to `example.dts`.

## Configuring the OpenWrt

`make menuconfig`

### Kernel modules:

* Navigate to `> Kernel modules > Sound Support`.
* Select `kmod-sound-core` and `kmod-sound-mt76x8-wm8960`.

<img src="docs/kmod.png">

### Userspace tools:

* Navigate to `> Sound` and select `alsa-utils`.

<img src="docs/alsa-utils.png">

## Building the image

`make -j9 V=s`

## Settings

By default, the WM8960 will be muted after rebooting. Run the following commands before playback/capture.

### Playback:

```
amixer sset "Left Output Mixer PCM" on
amixer sset "Right Output Mixer PCM" on
amixer sset "Headphone" 90%
amixer sset "Speaker" 90%
```

### Capture:

```
amixer sset "Left Input Mixer Boost" on
amixer sset "Right Input Mixer Boost" on
amixer sset "ALC Function" "Stereo"
```

## WM8960 MCLK

The WM8960 can get `MCLK` from an externel clock source or the `refclk` output of MT7628/88(~12MHz).

To enable the `refclk` output, you can modify the dts file as follows:
```
refclk {
	ralink,group = "refclk";
	ralink,function = "refclk";
};
```

## WM8960 ADCLRC

The WM8960 can internally get `ADCLRC` from `DACLRC` and the `ADCLRC` pin can be used as `GPIO1`. It is useful for some boards that only have the `DACLRC` pin connected and the `ADCLRC` pin is left floating.

To enable this feature, add the patch file from the `patches-4.14` folder to the `target/linux/ramips/patches-4.14` folder of OpenWrt and modify the dts file as follows:
```
codec: wm8960@1a {
	compatible = "wlf,wm8960";
	reg = <0x1a>;
	wlf,shared-lrclk;
	wlf,adclrc-gpio;
};
```

## WM8960 block diagram

<img src="docs/wm8960blkdiag.png">
