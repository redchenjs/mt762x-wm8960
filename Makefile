include $(TOPDIR)/rules.mk

PKG_NAME:=mt762x-wm8960
PKG_RELEASE:=1

PKG_LICENSE:=GPL-2.0
PKG_LICENSE_FILES:=LICENSE

PKG_MAINTAINER:=Jack Chen <redchenjs@live.com>
PKG_BUILD_PARALLEL:=1

PKG_CONFIG_DEPENDS += \
	CONFIG_PACKAGE_kmod-sound-mt762x-wm8960

include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/package.mk

define KernelPackage/sound-mt762x-wm8960
	SUBMENU:=Sound Support
	TITLE:=MT762X WM8960 ALSA SoC Machine Driver
	DEPENDS:=@TARGET_ramips +kmod-sound-mt7620 \
		+(TARGET_ramips_mt7621||TARGET_ramips_mt76x8):kmod-i2c-mt7628 \
		+!(TARGET_ramips_mt7621||TARGET_ramips_mt76x8):kmod-i2c-ralink \
		@!TARGET_ramips_rt288x
	FILES:=$(PKG_BUILD_DIR)/snd-soc-mt762x-wm8960.ko
	AUTOLOAD:=$(call AutoLoad,91,snd-soc-mt762x-wm8960)
endef

NOSTDINC_FLAGS = \
	-I$(PKG_BUILD_DIR) \
	-I$(LINUX_DIR)/sound/soc/ralink

ifdef CONFIG_PACKAGE_kmod-sound-mt762x-wm8960
	PKG_MAKE_FLAGS += CONFIG_SND_SOC_MT762X_WM8960=m
endif

define Build/Compile
	+$(MAKE) $(PKG_JOBS) -C "$(LINUX_DIR)" \
		$(KERNEL_MAKE_FLAGS) \
		$(PKG_MAKE_FLAGS) \
		M="$(PKG_BUILD_DIR)" \
		NOSTDINC_FLAGS="$(NOSTDINC_FLAGS)" \
		modules
endef

define Package/kmod-sound-mt762x-wm8960/install
	true
endef

$(eval $(call KernelPackage,sound-mt762x-wm8960))
