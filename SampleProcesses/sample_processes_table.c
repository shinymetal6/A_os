/*
 * sample_processes_table.c
 *
 *  Created on: Sep 13, 2023
 *      Author: fil
 */


#include "main.h"
#include "sample_A_os_includes.h"

#ifdef SAMPLE_PROCESSES_ENABLED
#include "sample_processes_includes.h"
#include "sample_user_config.h"

#ifdef	SAMPLEPROCESS_1_HC05
extern	void sample_process_1_hc05(uint32_t process_id);	//This is process1
#endif // #define	SAMPLEPROCESS_1_HC05
#ifdef	SAMPLEPROCESS_1_BASIC
extern	void sample_process_1_basic(uint32_t process_id);	//This is process1
#endif // #define	SAMPLEPROCESS_1_BASIC
#ifdef	SAMPLEPROCESS_1_GPIO
extern	void sample_process_1_gpio(uint32_t process_id);	//This is process1
#endif // #define	SAMPLEPROCESS_1_GPIO
#ifdef	SAMPLEPROCESS_1_XMODEM_RX_UART
extern	void sample_process_1_xmodem_rx_UART(uint32_t process_id);	//This is process1
#endif // #define	SAMPLEPROCESS_1_XMODEM_RX_UART
#ifdef	SAMPLEPROCESS_1_PING_NRF24L01
extern	void sample_process_1_ping_nrf24l01(uint32_t process_id);	//This is process1
extern	void sample_process_1_ping_init(uint32_t process_id);
#endif // #define	SAMPLEPROCESS_1_PING_NRF24L01
#ifdef	SAMPLEPROCESS_1_PONG_NRF24L01
extern	void sample_process_1_pong_nrf24l01(uint32_t process_id);	//This is process1
extern	void sample_process_1_pong_init(uint32_t process_id);
#endif // #define	SAMPLEPROCESS_1_PONG_NRF24L01
#ifdef	SAMPLEPROCESS_1_PN5180
extern	void sample_process_1_pn5180(uint32_t process_id);	//This is process1
extern	void sample_process_1_init(uint32_t process_id);
#endif // #define	SAMPLEPROCESS_1_PN5180
#ifdef	SAMPLEPROCESS_1_DMA_UART
extern	void sample_process_1_xmodem_rx_UART(uint32_t process_id);	//This is process1
#endif // #define	SAMPLEPROCESS_1_DMA_UART
#ifdef	SAMPLEPROCESS_1_XMODEM_RX_USB
extern	void sample_process_1_init(uint32_t process_id);
extern	void sample_process_1_xmodem_rx_USB(uint32_t process_id);	//This is process1
#endif // #define	SAMPLEPROCESS_1_XMODEM
#ifdef	SAMPLEPROCESS_1_SWITCH_MIDI_CDC
extern	void sample_process_1_init(uint32_t process_id);
extern	void sample_process_1_switch_midi_cdc(uint32_t process_id);	//This is process1
#endif // #define	SAMPLEPROCESS_1_SWITCH_MIDI_CDC
#ifdef	SAMPLEPROCESS_1_USB_ECHO
extern	void sample_process_1_init(uint32_t process_id);
extern	void sample_process_1_usbecho(uint32_t process_id);	//This is process1
#endif // #define	SAMPLEPROCESS_1_USB_ECHO
#ifdef	SAMPLEPROCESS_1_DCCPWM
extern	void sample_process_1_dccpwm(uint32_t process_id);	//This is process1
#endif // #define	SAMPLEPROCESS_1_DCCPWM
#ifdef	SAMPLEPROCESS_1_DHTxx
extern	void sample_process_1_Dhtxx_am230x(uint32_t process_id);	//This is process1
#endif // #define	SAMPLEPROCESS_1_DHTxx
#ifdef	SAMPLEPROCESS_1_QSPI
extern	void sample_process_1_init(uint32_t process_id);
extern	void sample_process_1_qspi(uint32_t process_id);	//This is process1
#endif // #define	SAMPLEPROCESS_1_QSPI
#ifdef	SAMPLEPROCESS_1_SERVO
extern	void sample_process_1_servo(uint32_t process_id);	//This is process1
#endif // #define	SAMPLEPROCESS_1_SERVO
#ifdef	SAMPLEPROCESS_1_I2CMEM
extern	void sample_process_1_i2cmem(uint32_t process_id);	//This is process1
#endif // #define	SAMPLEPROCESS_1_I2CMEM
#ifdef	SAMPLEPROCESS_1_ADC
extern	void sample_process_1_adc(uint32_t process_id);	//This is process1
#endif // #define	SAMPLEPROCESS_1_ADC
#ifdef	SAMPLEPROCESS_UARTQUEUES
extern	void sample_process_1_queuetest(uint32_t process_id);	//This is process1
#endif // #define	SAMPLEPROCESS_1_ADC
#ifdef	SAMPLEPROCESS_INTFLASH
extern	void sample_process_1_intflash(uint32_t process_id);	//This is process1
#endif // #define	SAMPLEPROCESS_INTFLASH
#ifdef	SAMPLEPROCESS_I2CSENSORS
extern	void sample_process_1_i2csensors(uint32_t process_id);	//This is process1
#endif // #define	SAMPLEPROCESS_I2CSENSORS
#ifdef	SAMPLEPROCESS_1_CAN
extern	void sample_process_1_can(uint32_t process_id);	//This is process1
#endif // #define	SAMPLEPROCESS_1_CAN
#ifdef	SAMPLEPROCESS_MBX
extern	void sample_process_1_mbxToPrc2(uint32_t process_id);	//This is process1
extern	void sample_process_2_mbxFromPrc1(uint32_t process_id);	//This is process1
#endif // #define	SAMPLEPROCESS_TIMDELAY_DEBUG
#ifdef	SAMPLEPROCESS_1_LCD7735
extern	void sample_process_1_lcd7735(uint32_t process_id);	//This is process1
extern	void sample_process_1_init(uint32_t process_id);
#endif // #define	SAMPLEPROCESS_1_LCD7735
#ifdef	SAMPLEPROCESS_1_MIDI
extern	void sample_process_1_midi(uint32_t process_id);	//This is process1
extern	void sample_process_1_init(uint32_t process_id);
#endif // #define	SAMPLEPROCESS_1_MIDI
#ifdef	SAMPLEPROCESS_1_ENCODER
extern	void sample_process_1_encoder(uint32_t process_id);
#endif // #define	SAMPLEPROCESS_1_ENCODER
#ifdef	SAMPLEPROCESS_1_AUDIO_DAC_MIDI
extern	void sample_process_1_audio_DAC_midi(uint32_t process_id);	//This is process1
extern	void sample_process_1_init(uint32_t process_id);
#endif // #define	SAMPLEPROCESS_1_MIDI
#ifdef	SAMPLEPROCESS_1_AUDIO_INSERT_REMOVE
extern	void sample_process_1_audio_insert_remove(uint32_t process_id);	//This is process1
extern	void sample_process_1_init(uint32_t process_id);
#endif // #define	SAMPLEPROCESS_1_AUDIO_INSERT_REMOVE
#ifdef	SAMPLEPROCESS_1_AUDIO_I2S_OSCILLATOR
extern	void sample_process_1_audio_I2S_oscillator(uint32_t process_id);	//This is process1
extern	void sample_process_1_init(uint32_t process_id);
#endif // #define	SAMPLEPROCESS_1_AUDIO_I2S_OSCILLATOR
#ifdef	SAMPLEPROCESS_1_AUDIO_I2S_DUAL_OSCILLATOR
extern	void sample_process_1_audio_I2S_dual_oscillator(uint32_t process_id);	//This is process1
extern	void sample_process_1_init(uint32_t process_id);
#endif // #define	SAMPLEPROCESS_1_AUDIO_I2S_DUAL_OSCILLATOR
#ifdef	SAMPLEPROCESS_1_PID
extern	void sample_process_1_pid(uint32_t process_id);	//This is process1
extern	void sample_process_1_init(uint32_t process_id);
#endif // #define	SAMPLEPROCESS_1_AUDIO_I2S_DUAL_OSCILLATOR
#ifdef	SAMPLEPROCESS_1_AUDIO_I2S_MIDI
extern	void sample_process_1_audio_I2S_midi(uint32_t process_id);	//This is process1
extern	void sample_process_1_init(uint32_t process_id);
#endif // #define	SAMPLEPROCESS_1_AUDIO_I2S_MIDI
#ifdef	SAMPLEPROCESS_1_AUDIO_I2S_IN2OUT
extern	void sample_process_1_audio_I2S_in2out(uint32_t process_id);	//This is process1
extern	void sample_process_1_init(uint32_t process_id);
#endif // #define	SAMPLEPROCESS_1_AUDIO_I2S_IN2OUT
#ifdef	SAMPLEPROCESS_1_AUDIO_I2S_IN_EFFECT_OUT
extern	void sample_process_1_audio_I2S_in_effect_out(uint32_t process_id);	//This is process1
extern	void sample_process_1_init(uint32_t process_id);
#endif // #define	SAMPLEPROCESS_1_AUDIO_I2S_IN_EFFECT_OUT
#ifdef	SAMPLEPROCESS_1_AUDIO_I2S_IN_SYNTH_EFFECT_OUT
extern	void sample_process_1_audio_I2S_in_synth_effect_out(uint32_t process_id);	//This is process1
extern	void sample_process_1_init(uint32_t process_id);
#endif // #define	SAMPLEPROCESS_1_AUDIO_I2S_IN_SYNTH_EFFECT_OUT
#ifdef	SAMPLEPROCESS_1_DAC_WAVEFORMS
extern	void sample_process_1_dac_waveforms(uint32_t process_id);	//This is process1
extern	void sample_process_1_init(uint32_t process_id);
#endif // #define	SAMPLEPROCESS_1_DAC_WAVEFORMS
#ifdef	SAMPLEPROCESS_1_AUDIO_DAC_OSCILLATOR
extern	void sample_process_1_audio_dac_oscillator(uint32_t process_id);	//This is process1
extern	void sample_process_1_init(uint32_t process_id);
#endif // #define	SAMPLEPROCESS_1_AUDIO_DAC_OSCILLATOR
#ifdef	SAMPLEPROCESS_1_AUDIO_I2S_DRUM
extern	void sample_process_1_audio_I2S_drum(uint32_t process_id);	//This is process1
extern	void sample_process_1_init(uint32_t process_id);
#endif // #define	SAMPLEPROCESS_1_AUDIO_I2S_DRUM
#ifdef	SAMPLEPROCESS_1_SDCARD
extern	void sample_process_1_sdcard(uint32_t process_id);	//This is process1
extern	void sample_process_1_init(uint32_t process_id);
#endif // #define	SAMPLEPROCESS_1_SDCARD
#ifdef	SAMPLEPROCESS_1_USBAUDIO
extern	void sample_process_1_usbaudio(uint32_t process_id);	//This is process1
extern	void sample_process_1_init(uint32_t process_id);
#endif // #define	SAMPLEPROCESS_1_USBAUDIO
#ifdef	SAMPLEPROCESS_1_G431RB_PN5180
extern	void sample_process_1_g431rb_pn5180(uint32_t process_id);	//This is process1
extern	void sample_process_1_init(uint32_t process_id);
#endif // #define	SAMPLEPROCESS_1_G431RB_PN5180
#ifdef	SAMPLEPROCESS_1_WS2812
extern	void sample_process_1_ws2812(uint32_t process_id);	//This is process1
extern	void sample_process_1_init(uint32_t process_id);
#endif // #define	SAMPLEPROCESS_1_WS2812
#ifdef	SAMPLEPROCESS_1_MLX90614
extern	void sample_process_1_mlx90614(uint32_t process_id);	//This is process1
extern	void sample_process_1_init(uint32_t process_id);
#endif // #define	SAMPLEPROCESS_1_MLX90614
#ifdef	SAMPLEPROCESS_1_MLX90640
extern	void sample_process_1_mlx90640(uint32_t process_id);	//This is process1
extern	void sample_process_1_init(uint32_t process_id);
#endif // #define	SAMPLEPROCESS_1_MLX90640
#ifdef	SAMPLEPROCESS_1_UVCDEVICE
extern	void sample_process_1_uvcdevice(uint32_t process_id);	//This is process1
extern	void sample_process_1_init(uint32_t process_id);
#endif // #define	SAMPLEPROCESS_1_UVCDEVICE
#ifdef	SAMPLEPROCESS_1_IMX335
extern	void sample_process_1_imx335(uint32_t process_id);	//This is process1
extern	void sample_process_1_init(uint32_t process_id);
#endif // #define	SAMPLEPROCESS_1_IMX335
#ifdef	SAMPLEPROCESS_1_TOF_VL53L5CX
extern	void sample_process_1_tof_vl53l5cx(uint32_t process_id);	//This is process1
extern	void sample_process_1_init(uint32_t process_id);
#endif // #define	SAMPLEPROCESS_1_TOF_VL53L5CX
#ifdef	SAMPLEPROCESS_1_STEPPER
extern	void sample_process_1_stepper(uint32_t process_id);	//This is process1
extern	void sample_process_1_init(uint32_t process_id);
#endif // #define	SAMPLEPROCESS_1_STEPPER


extern	void sample_process_2(uint32_t process_id);	//This is process3
extern	void sample_process_3(uint32_t process_id);	//This is process3
extern	void sample_process_4(uint32_t process_id);	//This is process4 of the application

VERSIONING	uint8_t	app_name[32] 		= "Sample Processes";
VERSIONING	uint8_t	app_version[32] 	= "Sample Version";

USRprcs_t	UserProcesses[USR_PROCESS_NUMBER] =
{
		{
#ifdef	SAMPLEPROCESS_1_HC05
				.user_process = sample_process_1_hc05,
#endif // #define	SAMPLEPROCESS_1_HC05

#ifdef	SAMPLEPROCESS_1_BASIC
				.user_process = sample_process_1_basic,
#endif // #define	SAMPLEPROCESS_1_BASIC
#ifdef	SAMPLEPROCESS_1_GPIO
				.user_process = sample_process_1_gpio,
#endif // #define	SAMPLEPROCESS_1_GPIO
#ifdef	SAMPLEPROCESS_1_XMODEM_RX_UART
				.user_process = sample_process_1_xmodem_rx_UART,
#endif // #define	SAMPLEPROCESS_1_XMODEM_RX_UART
#ifdef	SAMPLEPROCESS_1_PING_NRF24L01
				.user_process = sample_process_1_ping_nrf24l01,
				.user_init = sample_process_1_ping_init,
#endif // #define	SAMPLEPROCESS_1_NRF24L01
#ifdef	SAMPLEPROCESS_1_PONG_NRF24L01
				.user_process = sample_process_1_pong_nrf24l01,
				.user_init = sample_process_1_pong_init,
#endif // #define	SAMPLEPROCESS_1_PONG_NRF24L01
#ifdef	SAMPLEPROCESS_1_PN5180
				.user_process = sample_process_1_pn5180,
				.user_init = sample_process_1_init,
#endif // #define	SAMPLEPROCESS_1_PN5180
#ifdef	SAMPLEPROCESS_1_DMA_UART
				.user_process = sample_process_1_xmodem_rx_UART,
#endif // #define	SAMPLEPROCESS_1_DMA_UART
#ifdef	SAMPLEPROCESS_1_XMODEM_RX_USB
				.user_process = sample_process_1_xmodem_rx_USB,
				.user_init = sample_process_1_init,
#endif // #define	SAMPLEPROCESS_1_XMODEM_RX_USB
#ifdef	SAMPLEPROCESS_1_SWITCH_MIDI_CDC
				.user_process = sample_process_1_switch_midi_cdc,
				.user_init = sample_process_1_init,
#endif // #define	SAMPLEPROCESS_1_SWITCH_MIDI_CDC
#ifdef	SAMPLEPROCESS_1_USB_ECHO
				.user_process = sample_process_1_usbecho,
				.user_init = sample_process_1_init,
#endif // #define	SAMPLEPROCESS_1_USB_ECHO
#ifdef	SAMPLEPROCESS_1_DCCPWM
				.user_process = sample_process_1_dccpwm,
#endif // #define	SAMPLEPROCESS_1_DCCPWM
#ifdef	SAMPLEPROCESS_1_DHTxx
				.user_process = sample_process_1_Dhtxx_am230x,
#endif // #define	SAMPLEPROCESS_1_DHTxx
#ifdef	SAMPLEPROCESS_1_QSPI
				.user_process = sample_process_1_qspi,
				.user_init = sample_process_1_init,
#endif // #define	SAMPLEPROCESS_1_QSPI
#ifdef	SAMPLEPROCESS_1_SERVO
				.user_process = sample_process_1_servo,
#endif // #define	SAMPLEPROCESS_1_SERVO
#ifdef	SAMPLEPROCESS_1_I2CMEM
				.user_process = sample_process_1_i2cmem,
#endif // #define	SAMPLEPROCESS_1_I2CMEM
#ifdef	SAMPLEPROCESS_1_ADC
				.user_process = sample_process_1_adc,
#endif // #define	SAMPLEPROCESS_1_ADC
#ifdef	SAMPLEPROCESS_UARTQUEUES
				.user_process = sample_process_1_queuetest,
#endif // #define	SAMPLEPROCESS_UARTQUEUES
#ifdef	SAMPLEPROCESS_INTFLASH
				.user_process = sample_process_1_intflash,
#endif // #define	SAMPLEPROCESS_INTFLASH
#ifdef	SAMPLEPROCESS_I2CSENSORS
				.user_process = sample_process_1_i2csensors,
#endif // #define	SAMPLEPROCESS_I2CSENSORS
#ifdef	SAMPLEPROCESS_1_CAN
				.user_process = sample_process_1_can,
#endif // #define	SAMPLEPROCESS_1_CAN
#ifdef	SAMPLEPROCESS_MBX
				.user_process = sample_process_1_mbxToPrc2,
#endif // #define	SAMPLEPROCESS_MBX
#ifdef	SAMPLEPROCESS_1_LCD7735
				.user_process = sample_process_1_lcd7735,
				.user_init = sample_process_1_init,
#endif // #define	SAMPLEPROCESS_1_LCD7735
#ifdef	SAMPLEPROCESS_1_MIDI
				.user_process = sample_process_1_midi,
				.user_init = sample_process_1_init,
#endif // #define	SAMPLEPROCESS_1_MIDI
#ifdef	SAMPLEPROCESS_1_ENCODER
				.user_process = sample_process_1_encoder,
#endif // #define	SAMPLEPROCESS_1_ENCODER
#ifdef	SAMPLEPROCESS_1_AUDIO_DAC_MIDI
				.user_process = sample_process_1_audio_DAC_midi,
				.user_init = sample_process_1_init,
#endif
#ifdef	SAMPLEPROCESS_1_AUDIO_INSERT_REMOVE
				.user_process = sample_process_1_audio_insert_remove,
				.user_init = sample_process_1_init,
#endif
#ifdef	SAMPLEPROCESS_1_AUDIO_I2S_OSCILLATOR
				.user_process = sample_process_1_audio_I2S_oscillator,
				.user_init = sample_process_1_init,
#endif
#ifdef	SAMPLEPROCESS_1_AUDIO_I2S_DUAL_OSCILLATOR
				.user_process = sample_process_1_audio_I2S_dual_oscillator,
				.user_init = sample_process_1_init,
#endif
#ifdef	SAMPLEPROCESS_1_PID
				.user_process = sample_process_1_pid,
				.user_init = sample_process_1_init,
#endif
#ifdef	SAMPLEPROCESS_1_AUDIO_I2S_MIDI
				.user_process = sample_process_1_audio_I2S_midi,
				.user_init = sample_process_1_init,
#endif // #define	SAMPLEPROCESS_1_AUDIO_I2S_MIDI
#ifdef	SAMPLEPROCESS_1_AUDIO_I2S_IN2OUT
				.user_process = sample_process_1_audio_I2S_in2out,
				.user_init = sample_process_1_init,
#endif // #define	SAMPLEPROCESS_1_AUDIO_I2S_MIDI
#ifdef	SAMPLEPROCESS_1_AUDIO_I2S_IN_EFFECT_OUT
				.user_process = sample_process_1_audio_I2S_in_effect_out,
				.user_init = sample_process_1_init,
#endif // #define	SAMPLEPROCESS_1_AUDIO_I2S_IN_EFFECT_OUT
#ifdef	SAMPLEPROCESS_1_AUDIO_I2S_IN_SYNTH_EFFECT_OUT
				.user_process = sample_process_1_audio_I2S_in_synth_effect_out,
				.user_init = sample_process_1_init,
#endif // #define	SAMPLEPROCESS_1_AUDIO_I2S_IN_EFFECT_OUT
#ifdef	SAMPLEPROCESS_1_AUDIO_DAC_OSCILLATOR
				.user_process = sample_process_1_audio_dac_oscillator,
				.user_init = sample_process_1_init,
#endif // #define	SAMPLEPROCESS_1_AUDIO_DAC_OSCILLATOR
#ifdef	SAMPLEPROCESS_1_AUDIO_I2S_DRUM
				.user_process = sample_process_1_audio_I2S_drum,
				.user_init = sample_process_1_init,
#endif // #define	SAMPLEPROCESS_1_AUDIO_I2S_DRUM
#ifdef	SAMPLEPROCESS_1_SDCARD
				.user_process = sample_process_1_sdcard,
				.user_init = sample_process_1_init,
#endif // #define	SAMPLEPROCESS_1_SDCARD
#ifdef	SAMPLEPROCESS_1_USBAUDIO
				.user_process = sample_process_1_usbaudio,
				.user_init = sample_process_1_init,
#endif // #define	SAMPLEPROCESS_1_USBAUDIO
#ifdef	SAMPLEPROCESS_1_WS2812
				.user_process = sample_process_1_ws2812,
				.user_init = sample_process_1_init,
#endif // #define	SAMPLEPROCESS_1_WS2812
#ifdef	SAMPLEPROCESS_1_MLX90614
				.user_process = sample_process_1_mlx90614,
				.user_init = sample_process_1_init,
#endif // #define	SAMPLEPROCESS_1_MLX90614
#ifdef	SAMPLEPROCESS_1_MLX90640
				.user_process = sample_process_1_mlx90640,
				.user_init = sample_process_1_init,
#endif // #define	SAMPLEPROCESS_1_MLX90640
#ifdef	SAMPLEPROCESS_1_UVCDEVICE
				.user_process = sample_process_1_uvcdevice,
				.user_init = sample_process_1_init,
#endif // #define	SAMPLEPROCESS_1_UVCDEVICE
#ifdef	SAMPLEPROCESS_1_IMX335
				.user_process = sample_process_1_imx335,
				.user_init = sample_process_1_init,
#endif // #define	SAMPLEPROCESS_1_IMX335
#ifdef	SAMPLEPROCESS_1_TOF_VL53L5CX
				.user_process = sample_process_1_tof_vl53l5cx,
				.user_init = sample_process_1_init,
#endif // #define	SAMPLEPROCESS_1_TOF_VL53L5CX
#ifdef	SAMPLEPROCESS_1_STEPPER
				.user_process = sample_process_1_stepper,
				.user_init = sample_process_1_init,
#endif // #define	SAMPLEPROCESS_1_STEPPER
				.stack_size = 1024,
		},
		{
#ifdef	SAMPLEPROCESS_MBX
				.user_process = sample_process_2_mbxFromPrc1,
#else
				.user_process = sample_process_2,
#endif // #define	SAMPLEPROCESS_MBX
				.stack_size = 1024,
		},
		{
				.user_process = sample_process_3,
				.stack_size = 1024,
		},
		{
				.user_process = sample_process_4,
				.stack_size = 1024,
		}
};

#endif // #ifdef SAMPLE_PROCESSES_ENABLED
