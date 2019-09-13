include $(TOPDIR)/rules.mk

PKG_NAME:=mt76x8-wm8960
PKG_RELEASE=1

PKG_LICENSE:=GPLv2
PKG_LICENSE_FILES:=

PKG_MAINTAINER:=Jack Chen <redchenjs@live.com>
PKG_BUILD_PARALLEL:=1

PKG_CONFIG_DEPENDS += \
	CONFIG_PACKAGE_kmod-sound-mt76x8-wm8960

include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/package.mk

define KernelPackage/sound-mt76x8-wm8960
	SUBMENU:=Sound Support
	TITLE:=MT76x8 WM8960 ALSA SoC machine driver
	DEPENDS:=@TARGET_ramips +kmod-sound-mt7620
	FILES:= \
		$(PKG_BUILD_DIR)/snd-soc-mt76x8-wm8960.ko
	AUTOLOAD:=$(call AutoLoad,99,snd-soc-mt76x8-wm8960)
endef

NOSTDINC_FLAGS = \
	-I$(PKG_BUILD_DIR) \
	-I$(LINUX_DIR)/sound/soc/ralink

ifdef CONFIG_PACKAGE_kmod-sound-mt76x8-wm8960
	PKG_MAKE_FLAGS += CONFIG_SND_SOC_MT76X8_WM8960=m
endif

define Build/Compile
	+$(MAKE) $(PKG_JOBS) -C "$(LINUX_DIR)" \
		$(KERNEL_MAKE_FLAGS) \
		$(PKG_MAKE_FLAGS) \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		NOSTDINC_FLAGS="$(NOSTDINC_FLAGS)" \
		modules
endef

define Package/kmod-sound-mt76x8-wm8960/install
	true
endef

$(eval $(call KernelPackage,sound-mt76x8-wm8960))
