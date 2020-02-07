// SPDX-License-Identifier: GPL-2.0
/*
 * mt762x-wm8960.c  --  MT762X WM8960 ALSA SoC machine driver
 *
 * Copyright 2018-2020 Jack Chen <redchenjs@live.com>
 */

#include <linux/module.h>
#include <linux/of_platform.h>
#include <linux/i2c.h>
#include <sound/soc.h>

#include "../codecs/wm8960.h"

static const struct snd_soc_dapm_widget mt762x_wm8960_dapm_widgets[] = {
    SND_SOC_DAPM_HP("Headphone", NULL),
    SND_SOC_DAPM_SPK("Ext Spk", NULL),
    SND_SOC_DAPM_LINE("Line In", NULL),
    SND_SOC_DAPM_MIC("Mic", NULL),
};

static int mt762x_wm8960_ops_hw_params(struct snd_pcm_substream *substream,
                                       struct snd_pcm_hw_params *params)
{
    struct snd_soc_pcm_runtime *rtd = substream->private_data;
    struct snd_soc_dai *codec_dai = rtd->codec_dai;
    struct snd_soc_dai *cpu_dai = rtd->cpu_dai;
    unsigned int mclk_rate;
    unsigned int rate = params_rate(params);
    unsigned int div_mclk_over_bck = rate > 192000 ? 2 : 4;
    unsigned int div_bck_over_lrck = 64;

    mclk_rate = rate * div_bck_over_lrck * div_mclk_over_bck;

    snd_soc_dai_set_sysclk(cpu_dai, 0, mclk_rate, SND_SOC_CLOCK_OUT);
    snd_soc_dai_set_sysclk(codec_dai, 0, mclk_rate, SND_SOC_CLOCK_IN);

    return 0;
}

static struct snd_soc_ops mt762x_wm8960_ops = {
    .hw_params = mt762x_wm8960_ops_hw_params,
};

static struct snd_soc_dai_link mt762x_wm8960_dai_links[] = {
    {
        .name = "wm8960-codec",
        .stream_name = "wm8960-hifi",
        .cpu_dai_name = "ralink-i2s",
        .codec_dai_name = "wm8960-hifi",
        .dai_fmt = SND_SOC_DAIFMT_I2S | SND_SOC_DAIFMT_NB_NF | SND_SOC_DAIFMT_CBS_CFS,
        .ops = &mt762x_wm8960_ops,
    },
};

static struct snd_soc_card mt762x_wm8960_card = {
    .name = "MT762X WM8960 ASoC Card",
    .owner = THIS_MODULE,
    .dai_link = mt762x_wm8960_dai_links,
    .num_links = ARRAY_SIZE(mt762x_wm8960_dai_links),
    .dapm_widgets = mt762x_wm8960_dapm_widgets,
    .num_dapm_widgets = ARRAY_SIZE(mt762x_wm8960_dapm_widgets),
};

static int mt762x_wm8960_machine_probe(struct platform_device *pdev)
{
    struct snd_soc_card *card = &mt762x_wm8960_card;
    struct snd_soc_pcm_runtime *rtd;
    struct snd_soc_dai *codec_dai;
    struct platform_device *platform_dev;
    struct device_node *platform_node;
    struct device_node *codec_node;
    struct i2c_client *codec_dev;
    int ret, i;

    platform_node = of_parse_phandle(pdev->dev.of_node, "mediatek,platform", 0);
    if (!platform_node) {
        dev_err(&pdev->dev, "property 'platform' missing or invalid\n");
        return -EINVAL;
    }
    platform_dev = of_find_device_by_node(platform_node);
    if (!platform_dev) {
        dev_err(&pdev->dev, "failed to find platform device\n");
        return -EINVAL;
    }
    for (i=0; i<card->num_links; i++) {
        if (mt762x_wm8960_dai_links[i].platform_name) {
            continue;
        }
        mt762x_wm8960_dai_links[i].platform_of_node = platform_node;
    }

    card->dev = &pdev->dev;

    codec_node = of_parse_phandle(pdev->dev.of_node, "mediatek,audio-codec", 0);
    if (!codec_node) {
        dev_err(&pdev->dev, "property 'audio-codec' missing or invalid\n");
        return -EINVAL;
    }
    codec_dev = of_find_i2c_device_by_node(codec_node);
    if (!codec_dev || !codec_dev->dev.driver) {
        dev_err(&pdev->dev, "failed to find audio codec device\n");
        return -EINVAL;
    }
    for (i=0; i<card->num_links; i++) {
        if (mt762x_wm8960_dai_links[i].codec_name) {
            continue;
        }
        mt762x_wm8960_dai_links[i].codec_of_node = codec_node;
    }

    ret = snd_soc_of_parse_audio_routing(card, "audio-routing");
    if (ret) {
        dev_err(&pdev->dev, "failed to parse audio-routing: %d\n", ret);
        return ret;
    }

    ret = devm_snd_soc_register_card(&pdev->dev, card);
    if (ret) {
        dev_err(&pdev->dev, "%s snd_soc_register_card fail %d\n", __func__, ret);
        return ret;
    }

    list_for_each_entry(rtd, &card->rtd_list, list) {
        if (!strcmp(rtd->codec_dai->name, mt762x_wm8960_dai_links->codec_dai_name)) {
            codec_dai = rtd->codec_dai;
            break;
        }
    }
    if (!codec_dai) {
        dev_err(&pdev->dev, "failed to get codec dai\n");
        return -EINVAL;
    }

    // When ADCLRC is configured as a GPIO, DACLRC is used for the ADCs
    if (of_property_read_bool(codec_node, "wlf,adclrc-as-gpio")) {
        snd_soc_component_update_bits(codec_dai->component, WM8960_IFACE2, 0x40, 0x40);
    }

    return ret;
}

#ifdef CONFIG_OF
static const struct of_device_id mt762x_wm8960_machine_dt_match[] = {
    {.compatible = "mediatek,mt762x-wm8960-machine"},
    {}
};
#endif

static struct platform_driver mt762x_wm8960_machine = {
    .driver = {
        .name = "mt762x-wm8960",
#ifdef CONFIG_OF
        .of_match_table = mt762x_wm8960_machine_dt_match,
#endif
    },
    .probe = mt762x_wm8960_machine_probe,
};

module_platform_driver(mt762x_wm8960_machine);

MODULE_DESCRIPTION("MT762X WM8960 ALSA SoC machine driver");
MODULE_AUTHOR("Jack Chen <redchenjs@live.com>");
MODULE_ALIAS("platform:mt762x-wm8960-asoc-card");
MODULE_LICENSE("GPL v2");
