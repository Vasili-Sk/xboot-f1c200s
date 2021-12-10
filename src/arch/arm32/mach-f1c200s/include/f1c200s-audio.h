/*
 * f1c200s-audio.h
 *
 *  Created on: 6 дек. 2021 г.
 *      Author: VasiliSk
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef union
{
	struct
	{
		uint32_t HPVL_CTRL_OUT :6; //Internal status of headphone PA volume control, total 64 level
		uint32_t HPVL_STEP_CTRL :6; //Headphone volume soft tuning step control 0: 10us, 1: 20us, ..., 7: 80us, 8: 90us, 9: 100us, 10: 200us, ..., 16: 800us, 17: 900us, 18: 1ms, 19: 2ms, ..., 25: 8ms, 26: 9ms, 27: 10ms, 28: 20ms, ..., 34: 80ms, 35: 90ms, 36: 100ms, 37: 200ms, ..., 43: 800ms, 44: 900ms, 45: 1s, ..., 52: 8s, 53: 9s, 54-63: 9s
		uint32_t DVOL :6; //Digital volume control: dvc, ATT=DVC[5:0]*(-1.16dB)
		uint32_t HPF_EN :1; //High pass filter enable
		uint32_t HPPA_MXRD_ENA :1; //Status of headphone PA mixer all-input mute control
		uint32_t DACA_CHND_ENA :1; //Status of DAC Analog channel enable control
		uint32_t HPVL_SOFT_MOD :2; //Headphone volume soft tuning mode 0X: Normal mode, 10: Soft volume mode, 11: DAC Soft disable or enable mode
		uint32_t reserved :1; //bit 23
		uint32_t DWA :1; //DWA Function Disable
		uint32_t MODQU :4; //Internal DAC Quantization Levels Levels=[7*(21+MODQU[3:0])]/128, Default levels=7*21/128=1.15
		uint32_t reserved2 :2; //bit 29-30
		uint32_t EN_DAC :1;  //DAC Digital Part Enable 1: Enable
	};
	uint32_t REG32;
} DAC_DPC_t; //DAC Digital Part Control Register

typedef union
{
	struct
	{
		uint32_t FIFO_FLUSH :1; //Write С1Т to flush TX FIFO, self clear to С0Т
		uint32_t FIFO_OVR_IRQ_EN :1; //DAC FIFO Over Run IRQ Enable
		uint32_t FIFO_UDR_IRQ_EN :1; //DAC FIFO Under Run IRQ Enable
		uint32_t DAC_IRQ_EN :1; //DAC FIFO Empty IRQ Enable
		uint32_t DAC_DRQ_EN :1; //DAC FIFO Empty DRQ Enable
		uint32_t TX_SAMP_BITS :1; //Transmitting Audio Sample Resolution 0:16b, 1:24b
		uint32_t DAC_MONO_EN :1; //0:stereo 64 levels FIFO, 1:mono 128 levels FIFO
		uint32_t ADDA_LOOP_EN :1; //0 disable 1 enable
		uint32_t TX_TRIG_LEVEL :12; //Number of valid samples for interrupt and DMA request trigger, only 6:0b valid when TXMODE=0
		uint32_t DAC_DRQ_CLR_CNT :2; //When  TX  FIFO  available  room  less  than  or  equal  N,  DRQ  Request  will  be De-asserted, 00b:IRQ/DRQ Deasserted when WLEVEL > TXTL, 01b:4, 10b:8,	11b:16
		uint32_t reserved :1; //
		uint32_t FIFO_MODE :2; //24-bits FIFO_I[23:0]={TXDATA[31:8]}, 16-bits 0 = FIFO_I[23:0]={TXDATA[31:16], 8Тb0}, 16-bits 1 = FIFO_I[23:0]={TXDATA[15:0], 8Тb0}
		uint32_t SEND_LASAT :1; //Audio sample select when TX FIFO under run 0: Sending zero, 1: Sending last audio sample
		uint32_t reserved2 :1; //bit 27
		uint32_t FIR_VER :1; //FIR Version 0: 64-Tap FIR; 1: 32-Tap FIR
		uint32_t DAC_FS :3; //Sample rate of DAC
	};
	uint32_t REG32;
} DAC_FIFOC_t; //DAC FIFO Control Register

enum
{
	DAC_FS_48K = 0, DAC_FS_32K = 1, DAC_FS_24K = 2, DAC_FS_16K = 3, DAC_FS_12K = 4, DAC_FS_8K = 5, DAC_FS_192K = 6, DAC_FS_96K = 7,
};

typedef union
{
	struct
	{
		uint32_t reserved :1; //bit 0
		uint32_t TXO_INT :1; //TX FIFO Overrun Pending Interrupt, w1-clear
		uint32_t TXU_INT :1; //TX FIFO Underrun Pending Interrupt, w1-clear
		uint32_t TXE_INT :1; //TX FIFO Empty Pending Interrupt, w1-clear
		uint32_t reserved2 :4; //bit 4-7
		uint32_t TXE_CNT :15; //TX FIFO Empty space word counter
		uint32_t TX_EMPTY :1; //TX FIFO Empty 0: No room for new sample, 1: More than one room
	};
	uint32_t REG32;
} DAC_FIFOS_t; //DAC FIFO Status Register

typedef union
{
	struct
	{
		uint32_t HPVOL :6; //Headphone  Volume  Control,  (HPVOL):  Total  64  level,  from  0dB  to  -62dB
		uint32_t RTLNMUTE :1; //Right HPOUT Negative To Left HPOUT Mute 0: Mute
		uint32_t LTRNMUTE :1; //Left HPOUT Negative To Right HPOUT Mute 0: Mute
		uint32_t LMIXMUTE_R_DAC :1; //Left output Mixer Mute Control Right DAC 0: Mute
		uint32_t LMIXMUTE_L_DAC :1; //Left output Mixer Mute Control Left DAC 0: Mute
		uint32_t LMIXMUTE_FMINL :1; //Left output Mixer Mute Control FM IN L 0: Mute
		uint32_t LMIXMUTE_LINEIN :1; //Left output Mixer Mute Control LINE IN 0: Mute
		uint32_t LMIXMUTE_MICIN :1; //Left output Mixer Mute Control MIC IN Boost stage  0: Mute
		uint32_t reserved2 :2; //bit 13-14
		uint32_t HPPAEN :1; //Right Left Headphone power amplifier enable 1:Enable
		uint32_t RMIXMUTE_L_DAC :1; //Left output Mixer Mute Control Left DAC 0: Mute
		uint32_t RMIXMUTE_R_DAC :1; //Left output Mixer Mute Control Right DAC 0: Mute
		uint32_t RMIXMUTE_FMINR :1; //Left output Mixer Mute Control FM IN R 0: Mute
		uint32_t RMIXMUTE_LINEIN :1; //Left output Mixer Mute Control LINE IN 0: Mute
		uint32_t RMIXMUTE_MICIN :1; //Left output Mixer Mute Control MIC IN Boost stage  0: Mute
		uint32_t COMPTEN :1; //HPCOM output protection enable when it is set as Direct driver for HPL/R 0: protection disable
		uint32_t HPCOM_FC :2; //HPCOM function control 00: HPCOM off & output is floating, 01: HPL inverting output, 10: HPR inverting output, 11: Direct driver for HPL & HPR
		uint32_t LHPIS :1; //Left Headphone Power Amplifier (PA) Input Source Select  0: Left channel DAC, 1: Left Analog Mixer
		uint32_t RHPIS :1; //Right Headphone Power Amplifier (PA) Input Source Select 0: Right channel DAC, 1: Right Analog Mixer
		uint32_t LHPPAMUTE :1; //All input source to Left Headphone PA mute, including Left Output mixer and Internal Left channel DAC 0: Mute
		uint32_t RHPPAMUTE :1; //All input source to Right Headphone PA mute, including Right Output mixer and Internal Right channel DAC 0: Mute
		uint32_t LMIXEN :1; //Left Analog Output Mixer Enable 1:Enable
		uint32_t RMIXEN :1; //Right analog Output Mixer Enable 1:Enable
		uint32_t DACALEN :1; //Internal Analog Left channel DAC enable
		uint32_t DACAREN :1; //Internal Analog Right channel DAC enable
	};
	uint32_t REG32;
} DAC_MIXER_CTRL_t; //DAC FIFO Status Register

typedef union
{
	struct
	{
		uint32_t BIHE_CTRL :2; //BIHE control 0:no, 1: BIHE=7.5 HOSC, 2:BIHE=11.5 HOSC, 3:BIHE=15.5 HOSC
		uint32_t DITHER_CLK_SEL :2; //ADC dither clock select 0: ADC FS * (8/9), 1: ADC FS * (16/15), 2: ADC FS * (4/3), 3: ADC FS * (16/9)
		uint32_t DITHER :1; //ADC dither on/off control 1: dither on
		uint32_t USB_BIAS_CUR :3; //USB bias current tuning, From 23uA to 30uA, Default is 25uA
		uint32_t OPAAF_BIAS_CUR :2; //OPAAF in ADC Bias Current Select
		uint32_t OPADC2_BIAS_CUR :2; //OPADC2 Bias Current Select
		uint32_t OPADC1_BIAS_CUR :2; //OPADC1 Bias Current Select
		uint32_t OPDRV_OPCOM_CUR :2; //OPDRV/OPCOM output stage current setting
		uint32_t OPMIX_BIAS_CUR :2; //OPMIX/OPLPF/OPDRV/OPCOM Bias Current Control
		uint32_t OPDAC_BIAS_CUR :2; //OPDAC Bias Current Control
		uint32_t OPVR_BIAS_CUR :2; //OPVR Bias Current Control
		uint32_t OPMIC_BIAS_CUR :2; //OPMIC Bias Current Control
		uint32_t PA_ANTI_POP_CTRL :3; //PA Anti-pop time control 0:131ms, 4:655ms, 7:1048ms
		uint32_t PA_SLOPE_SEL :1; //PA slope select 0: cosine, 1: ramp
		uint32_t PTDBS :2; //HPCOM protect debounce time setting 0:2-3ms, 1:4-6ms, 2:8-12ms, 3:16-24ms
		uint32_t ZERO_CROSS_TIME_SEL :1; //Timeout control for master volume change at zero cross over 0:32ms, 1:64ms
		uint32_t ZERO_CROSS_EN :1; //Function enable for master volume change at zero cross over 1: enable
	};
	uint32_t REG32;
} ADCDAC_TUNE_t; //ADDA_TUNE_REG ADC&DAC performance tuning Register

typedef struct
{
	volatile uint32_t DAC_DPC; //DAC Digital Part Control Register
	volatile uint32_t DAC_FIFOC; //DAC FIFO Control Register
	volatile uint32_t DAC_FIFOS; //DAC FIFO Status Register
	volatile uint32_t DAC_TXDATA; //DAC TX Data Register
	volatile uint32_t ADC_FIFOC; //ADC FIFO Control Register
	volatile uint32_t ADC_FIFOS; //ADC FIFO Status Register
	volatile uint32_t ADC_RXDATA; //ADC RX Data Register
	volatile uint32_t reserved; //
	volatile uint32_t DAC_MIXER_CTRL; //DAC & MIXER Control Register
	volatile uint32_t ADC_MIXER_CTRL; //ADC Analog and Input mixer Control Register
	volatile uint32_t ADDA_TUNE; //ADC & DAC performance tuning Register
	volatile uint32_t BIAS_DA16_CAL_CTRL0; //Bias & DA16 Calibration Control Register 0
	volatile uint32_t reserved2[1]; //
	volatile uint32_t BIAS_DA16_CAL_CTRL1; //Bias & DA16 Calibration Control Register 1
	volatile uint32_t reserved3[2]; //
	volatile uint32_t DAC_CNT; //DAC TX FIFO Counter Register
	volatile uint32_t ADC_CNT; //ADC RX FIFO Counter Register
	volatile uint32_t DAC_DG; //DAC Debug Register
	volatile uint32_t ADC_DG; //ADC Debug Register
	volatile uint32_t reserved4[8];
	volatile uint32_t ADC_DAP_CTR; //ADC DAP Control Register
	volatile uint32_t ADC_DAP_LCTR; //ADC DAP Left Control Register
	volatile uint32_t ADC_DAP_RCTR; //ADC DAP Right Control Register
	volatile uint32_t ADC_DAP_PARA; //ADC DAP Parameter Register
	volatile uint32_t ADC_DAP_LAC; //ADC DAP Left Average Coef Register
	volatile uint32_t ADC_DAP_LDAT; //ADC DAP Left Decay and Attack Time Register
	volatile uint32_t ADC_DAP_RAC; //ADC DAP Right Average Coef Register
	volatile uint32_t ADC_DAP_RDAT; //ADC DAP Right Decay and Attack Time Register
	volatile uint32_t ADC_DAP_HPFC; //ADC DAP HPF Coef Register
	volatile uint32_t ADC_DAP_LINAC; //ADC DAP Left Input Signal Low Average Coef Register
	volatile uint32_t ADC_DAP_RINAC; //ADC DAP Right Input Signal Low Average Coef Register
	volatile uint32_t ADC_DAP_ORT; //ADC DAP Optimum Register
} AudioCodec_t;
#ifdef __cplusplus
}
#endif
