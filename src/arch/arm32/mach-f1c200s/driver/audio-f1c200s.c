/*
 * driver/audio-f1c200s.c
 *
 * Copyright(c) 2007-2021 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <xboot.h>
#include <clk/clk.h>
#include <dma/dma.h>
#include <gpio/gpio.h>
#include <reset/reset.h>
#include <interrupt/interrupt.h>
#include <audio/audio.h>
#include <f1c200s-dma.h>
#include <f1c200s-audio.h>

#define snd_update_bits(reg, shift, mask, set) \
	write32(reg, (read32(reg) & (~((u32_t)(mask) << shift))) | ((u32_t)(set) << shift))

struct audio_f1c200s_pdata_t
{
	AudioCodec_t *codec;
	char *clk;
	int irq;
	int reset;
	int dma_playback;
	int dma_capture;

	struct
	{
		int16_t buffer[4096];
		audio_callback_t cb;
		void *data;
		int flag;
		int running;
	} playback;
};

static inline u64_t f1c200s_audio_get_clk(enum audio_rate_t rate)
{
	u64_t clk;

	switch(rate)
	{
	case AUDIO_RATE_7350:
	case AUDIO_RATE_11025:
	case AUDIO_RATE_14700:
	case AUDIO_RATE_22050:
	case AUDIO_RATE_29400:
	case AUDIO_RATE_44100:
	case AUDIO_RATE_88200:
	case AUDIO_RATE_176400:
		clk = 22579200 * 8;
		break;
	case AUDIO_RATE_8000:
	case AUDIO_RATE_12000:
	case AUDIO_RATE_16000:
	case AUDIO_RATE_24000:
	case AUDIO_RATE_32000:
	case AUDIO_RATE_48000:
	case AUDIO_RATE_96000:
	case AUDIO_RATE_192000:
		clk = 24576000 * 8;
		break;
	default:
		clk = 24576000 * 8;
		break;
	}
	return clk;
}

static inline int f1c200s_audio_get_fs(enum audio_rate_t rate)
{
	int fs;

	switch(rate)
	{
	case AUDIO_RATE_7350:
	case AUDIO_RATE_8000:
		fs = 5;
		break;
	case AUDIO_RATE_11025:
	case AUDIO_RATE_12000:
		fs = 4;
		break;
	case AUDIO_RATE_14700:
	case AUDIO_RATE_16000:
		fs = 3;
		break;
	case AUDIO_RATE_22050:
	case AUDIO_RATE_24000:
		fs = 2;
		break;
	case AUDIO_RATE_29400:
	case AUDIO_RATE_32000:
		fs = 1;
		break;
	case AUDIO_RATE_44100:
	case AUDIO_RATE_48000:
		fs = 0;
		break;
	case AUDIO_RATE_88200:
	case AUDIO_RATE_96000:
		fs = 7;
		break;
	case AUDIO_RATE_176400:
	case AUDIO_RATE_192000:
		fs = 6;
		break;
	default:
		fs = 0;
		break;
	}
	return fs;
}

static inline void f1c200s_audio_set_playback_volume(struct audio_f1c200s_pdata_t *pdat, int vol)
{
	DAC_MIXER_CTRL_t mixer = { .REG32 = pdat->codec->DAC_MIXER_CTRL };
	mixer.HPVOL = vol * 0x3f / 1000;
	pdat->codec->DAC_MIXER_CTRL = mixer.REG32;
}

static inline int f1c200s_audio_get_playback_volume(struct audio_f1c200s_pdata_t *pdat)
{
	DAC_MIXER_CTRL_t mixer = { .REG32 = pdat->codec->DAC_MIXER_CTRL };
	return mixer.HPVOL * 1000 / 0x3f;
}

static inline void f1c200s_audio_init(struct audio_f1c200s_pdata_t *pdat)
{
	DAC_FIFOC_t fifoc = { .REG32 = pdat->codec->DAC_FIFOC };
	DAC_DPC_t dpc = { .REG32 = pdat->codec->DAC_DPC };
	ADCDAC_TUNE_t tune = { .REG32 = pdat->codec->ADDA_TUNE };
	DAC_MIXER_CTRL_t mixer = { .REG32 = pdat->codec->DAC_MIXER_CTRL };

	//first setup antipop
	tune.PA_ANTI_POP_CTRL = 2; //anti pop on enable
	tune.ZERO_CROSS_EN = 1;
	pdat->codec->ADDA_TUNE = tune.REG32;

	//basically disable everything
	fifoc.DAC_DRQ_CLR_CNT = 3;
	fifoc.FIR_VER = 0;
	fifoc.FIFO_FLUSH = 1;
	fifoc.DAC_DRQ_EN = 0;

	dpc.EN_DAC = 0;

	mixer.COMPTEN = 1; // protection
	mixer.HPCOM_FC = 3; // Direct COM driver for HPL & HPR
	mixer.HPPAEN = 1; //amp enable
	mixer.LHPPAMUTE = 1;
	mixer.RHPPAMUTE = 1;
	mixer.DACALEN = 1;
	mixer.DACAREN = 1;

	//Mix all channels? wtf
	//mixerctl.LMIXMUTE_L_DAC = 1;
	//mixerctl.LMIXMUTE_R_DAC = 1;
	//mixerctl.RMIXMUTE_R_DAC = 1;
	//mixerctl.RMIXMUTE_L_DAC = 1;
	//should be still DAC selected so mix is ineffective
	//mixerctl.LHPIS = 0;
	//mixerctl.RHPIS = 0;

	pdat->codec->DAC_FIFOC = fifoc.REG32;
	pdat->codec->DAC_DPC = dpc.REG32;
	pdat->codec->DAC_MIXER_CTRL = mixer.REG32;
	f1c200s_audio_set_playback_volume(pdat, 1000);
}

static void audio_f1c200s_playback_finish(void *data)
{
	struct audio_t *audio = (struct audio_t*)data;
	struct audio_f1c200s_pdata_t *pdat = (struct audio_f1c200s_pdata_t*)audio->priv;
	int len;

	if(pdat->playback.running)
	{
		len = pdat->playback.cb(pdat->playback.data, pdat->playback.buffer, sizeof(pdat->playback.buffer));
		if(len > 0)
			dma_start(pdat->dma_playback, (void*)pdat->playback.buffer, (void*)&pdat->codec->DAC_TXDATA, len, pdat->playback.flag, NULL,
			        audio_f1c200s_playback_finish, audio);
		else
		{
			pdat->playback.running = 0;
			DAC_FIFOC_t fifoc = { .REG32 = pdat->codec->DAC_FIFOC };
			DAC_DPC_t dpc = { .REG32 = pdat->codec->DAC_DPC };
			fifoc.DAC_DRQ_EN = 0;
			dpc.EN_DAC = 0;
			pdat->codec->DAC_FIFOC = fifoc.REG32;
			pdat->codec->DAC_DPC = dpc.REG32;
		}
	}
}

static void audio_f1c200s_playback_start(struct audio_t *audio, enum audio_rate_t rate, enum audio_format_t fmt, int ch, audio_callback_t cb, void *data)
{
	struct audio_f1c200s_pdata_t *pdat = (struct audio_f1c200s_pdata_t*)audio->priv;
	int len;

	if(!pdat->playback.running)
	{

		clk_set_rate("pll-audio", f1c200s_audio_get_clk(rate));

		DAC_FIFOC_t fifoc = { .REG32 = pdat->codec->DAC_FIFOC };
		DAC_DPC_t dpc = { .REG32 = pdat->codec->DAC_DPC };

		fifoc.DAC_FS = f1c200s_audio_get_fs(rate);

		switch(fmt)
		{
		case AUDIO_FORMAT_S16:
			fifoc.TX_SAMP_BITS = 0; //16bit
			fifoc.FIFO_MODE = 3; // FIFO_I[23:0] = {TXDATA[15:0], 8’b0}
			break;
		case AUDIO_FORMAT_S24:
			fifoc.TX_SAMP_BITS = 1; //24bit
			fifoc.FIFO_MODE = 0; //  FIFO_I[23:0] = {TXDATA[31:8]}
			break;
		default:
			return;
		}

		switch(ch)
		{
		case 1:
			fifoc.DAC_MONO_EN = 1;
			break;
		case 2:
			fifoc.DAC_MONO_EN = 0;
			break;
		default:
			return;
		}
		fifoc.FIFO_FLUSH = 0;
		fifoc.TX_TRIG_LEVEL = 0xF;

		if(rate > 320000)
		{
			fifoc.FIR_VER = 0;
		}
		else
		{
			fifoc.FIR_VER = 1;
		}
		fifoc.SEND_LASAT = 0; //send zero
		fifoc.DAC_DRQ_EN = 1; //DMA request enable
		pdat->codec->DAC_FIFOC = fifoc.REG32;

		dpc.EN_DAC = 1;
		pdat->codec->DAC_DPC = dpc.REG32;

		pdat->playback.flag = DMA_S_TYPE(DMA_TYPE_MEMTODEV);
		pdat->playback.flag |= DMA_S_SRC_INC(DMA_INCREASE) | DMA_S_DST_INC(DMA_CONSTANT);
		pdat->playback.flag |= DMA_S_SRC_WIDTH(DMA_WIDTH_16BIT) | DMA_S_DST_WIDTH(DMA_WIDTH_16BIT);
		pdat->playback.flag |= DMA_S_SRC_BURST(DMA_BURST_SIZE_1) | DMA_S_DST_BURST(DMA_BURST_SIZE_1);
		pdat->playback.flag |= DMA_S_SRC_PORT(F1C200S_NDMA_PORT_SDRAM) | DMA_S_DST_PORT(F1C200S_NDMA_PORT_AUDIO);
		pdat->playback.cb = cb;
		pdat->playback.data = data;
		pdat->playback.running = 1;
		len = pdat->playback.cb(pdat->playback.data, pdat->playback.buffer, ch * fmt / 8 * 128);
		dma_start(pdat->dma_playback, (void*)pdat->playback.buffer, (void*)&pdat->codec->DAC_TXDATA, len, pdat->playback.flag, NULL,
		        audio_f1c200s_playback_finish, audio);
	}
}

static void audio_f1c200s_playback_stop(struct audio_t *audio)
{
	struct audio_f1c200s_pdata_t *pdat = (struct audio_f1c200s_pdata_t*)audio->priv;
	pdat->playback.running = 0;

	DAC_FIFOC_t fifoc = { .REG32 = pdat->codec->DAC_FIFOC };
	DAC_DPC_t dpc = { .REG32 = pdat->codec->DAC_DPC };
	fifoc.DAC_DRQ_EN = 0;
	dpc.EN_DAC = 0;
	pdat->codec->DAC_FIFOC = fifoc.REG32;
	pdat->codec->DAC_DPC = dpc.REG32;
}

static void audio_f1c200s_capture_start(struct audio_t *audio, enum audio_rate_t rate, enum audio_format_t fmt, int ch, audio_callback_t cb, void *data)
{
}

static void audio_f1c200s_capture_stop(struct audio_t *audio)
{
}

static int audio_f1c200s_ioctl(struct audio_t *audio, const char *cmd, void *arg)
{
	struct audio_f1c200s_pdata_t *pdat = (struct audio_f1c200s_pdata_t*)audio->priv;
	int *p = arg;

	switch(shash(cmd))
	{
	case 0x892b3889: /* "audio-set-playback-volume" */
		if(p)
		{
			f1c200s_audio_set_playback_volume(pdat, clamp(p[0], 0, 1000));
			return 0;
		}
		break;
	case 0x3eee6d7d: /* "audio-get-playback-volume" */
		if(p)
		{
			p[0] = f1c200s_audio_get_playback_volume(pdat);
			return 0;
		}
		break;
	case 0x6dab0056: /* "audio-set-capture-volume" */
		break;
	case 0x44a166ca: /* "audio-get-capture-volume" */
		break;
	default:
		break;
	}
	return -1;
}

static void f1c200s_audio_interrupt(void *data)
{
	struct audio_t *audio = (struct audio_t*)data;
	struct audio_f1c200s_pdata_t *pdat = (struct audio_f1c200s_pdata_t*)audio->priv;
	write32((virtual_addr_t)&pdat->codec->DAC_FIFOS, read32((virtual_addr_t)&pdat->codec->DAC_FIFOS));
	write32((virtual_addr_t)&pdat->codec->ADC_FIFOS, read32((virtual_addr_t)&pdat->codec->ADC_FIFOS));
}

static struct device_t* audio_f1c200s_probe(struct driver_t *drv, struct dtnode_t *n)
{
	struct audio_f1c200s_pdata_t *pdat;
	struct audio_t *audio;
	struct device_t *dev;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char *clk = dt_read_string(n, "clock-name", NULL);
	int irq = dt_read_int(n, "interrupt", -1);
	int dma_playback = dt_read_int(n, "dma-channel-playback", -1);
	int dma_capture = dt_read_int(n, "dma-channel-capture", -1);

	if(!search_clk(clk))
		return NULL;

	if(!irq_is_valid(irq))
		return NULL;

	if(!dma_is_valid(dma_playback))
		return NULL;

	if(!dma_is_valid(dma_capture))
		return NULL;

	pdat = malloc(sizeof(struct audio_f1c200s_pdata_t));
	if(!pdat)
		return NULL;

	audio = malloc(sizeof(struct audio_t));
	if(!audio)
	{
		free(pdat);
		return NULL;
	}

	pdat->codec = (void*)virt;
	pdat->clk = strdup(clk);
	pdat->irq = irq;
	pdat->reset = dt_read_int(n, "reset", -1);
	pdat->dma_playback = dma_playback;
	pdat->dma_capture = dma_capture;
	pdat->playback.running = 0;

	audio->name = alloc_device_name(dt_read_name(n), -1);
	audio->playback_start = audio_f1c200s_playback_start;
	audio->playback_stop = audio_f1c200s_playback_stop;
	audio->capture_start = audio_f1c200s_capture_start;
	audio->capture_stop = audio_f1c200s_capture_stop;
	audio->ioctl = audio_f1c200s_ioctl;
	audio->priv = pdat;

	clk_enable(pdat->clk);
	if(pdat->reset >= 0)
		reset_deassert(pdat->reset);
	request_irq(pdat->irq, f1c200s_audio_interrupt, IRQ_TYPE_NONE, audio);
	f1c200s_audio_init(pdat);

	if(!(dev = register_audio(audio, drv)))
	{
		free_irq(pdat->irq);
		clk_disable(pdat->clk);
		free(pdat->clk);
		free_device_name(audio->name);
		free(audio->priv);
		free(audio);
		return NULL;
	}
	return dev;
}

static void audio_f1c200s_remove(struct device_t *dev)
{
	struct audio_t *audio = (struct audio_t*)dev->priv;
	struct audio_f1c200s_pdata_t *pdat = (struct audio_f1c200s_pdata_t*)audio->priv;

	if(audio)
	{
		unregister_audio(audio);
		free_irq(pdat->irq);
		clk_disable(pdat->clk);
		free(pdat->clk);
		free_device_name(audio->name);
		free(audio->priv);
		free(audio);
	}
}

static void audio_f1c200s_suspend(struct device_t *dev)
{
}

static void audio_f1c200s_resume(struct device_t *dev)
{
}

static struct driver_t audio_f1c200s = { .name = "audio-f1c200s", .probe = audio_f1c200s_probe, .remove = audio_f1c200s_remove,
        .suspend = audio_f1c200s_suspend, .resume = audio_f1c200s_resume, };

static __init void audio_f1c200s_driver_init(void)
{
	register_driver(&audio_f1c200s);
}

static __exit void audio_f1c200s_driver_exit(void)
{
	unregister_driver(&audio_f1c200s);
}

driver_initcall(audio_f1c200s_driver_init);
driver_exitcall(audio_f1c200s_driver_exit);
