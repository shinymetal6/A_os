/* 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Project : A_os
*/
/*
 * usbd_midi.c
 *
 *  Created on: Jan 24, 2024
 *      Author: fil
 */

#include "main.h"

#ifdef	STM32H743xx

#include "../../../../../kernel/system_default.h"

#ifdef	USB_MIDI

#include "usbd_midi.h"
#include "../../Core/usbd_ctlreq.h"

static uint8_t USBD_MIDI_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t USBD_MIDI_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t USBD_MIDI_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
static uint8_t USBD_MIDI_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t USBD_MIDI_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t USBD_MIDI_EP0_RxReady(USBD_HandleTypeDef *pdev);
#ifndef USE_USBD_COMPOSITE
static uint8_t *USBD_MIDI_GetFSCfgDesc(uint16_t *length);
static uint8_t *USBD_MIDI_GetHSCfgDesc(uint16_t *length);
static uint8_t *USBD_MIDI_GetOtherSpeedCfgDesc(uint16_t *length);
uint8_t *USBD_MIDI_GetDeviceQualifierDescriptor(uint16_t *length);
#endif /* USE_USBD_COMPOSITE  */

#ifndef USE_USBD_COMPOSITE
/* USB Standard Device Descriptor */
__ALIGN_BEGIN static uint8_t USBD_MIDI_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] __ALIGN_END =
{
  USB_LEN_DEV_QUALIFIER_DESC,
  USB_DESC_TYPE_DEVICE_QUALIFIER,
  0x00,
  0x02,
  0x00,
  0x00,
  0x00,
  0x40,
  0x01,
  0x00,
};
#endif /* USE_USBD_COMPOSITE  */
/**
  * @}
  */

/** @defgroup USBD_MIDI_Private_Variables
  * @{
  */


/* MIDI interface class callbacks structure */
USBD_ClassTypeDef  USBD_MIDI =
{
  USBD_MIDI_Init,
  USBD_MIDI_DeInit,
  USBD_MIDI_Setup,
  NULL,                 /* EP0_TxSent */
  USBD_MIDI_EP0_RxReady,
  USBD_MIDI_DataIn,
  USBD_MIDI_DataOut,
  NULL,
  NULL,
  NULL,
#ifdef USE_USBD_COMPOSITE
  NULL,
  NULL,
  NULL,
  NULL,
#else
  USBD_MIDI_GetHSCfgDesc,
  USBD_MIDI_GetFSCfgDesc,
  USBD_MIDI_GetOtherSpeedCfgDesc,
  USBD_MIDI_GetDeviceQualifierDescriptor,
#endif /* USE_USBD_COMPOSITE  */
};

#ifndef USE_USBD_COMPOSITE
__ALIGN_BEGIN static uint8_t USBD_MIDI_CfgDesc[USB_MIDI_CONFIG_DESC_SIZE]  __ALIGN_END =
{
		  0x09,                       /* bLength: Configuration Descriptor size */
		  USB_DESC_TYPE_CONFIGURATION,/* bDescriptorType: Configuration */
		  USB_MIDI_CONFIG_DESC_SIZE,
		  0x00,                       /*Length of the total configuration block, including this descriptor, in bytes.*/
		  0x01,                       /*bNumInterfaces: 1 interface*/
		  0x01,                       /*bConfigurationValue: ID of this configuration. */
		  0x00,                       /*iConfiguration: Index of string descriptor describing the configuration (Unused.)*/
		  0x80,                       /*bmAttributes: Bus Powered device, not Self Powered, no Remote wakeup capability. */
		  0xFA,                       /*MaxPower 500 mA: this current is used for detecting Vbus*/

		  /************** MIDI Adapter Standard MS Interface Descriptor ****************/
		  0x09,                   /*bLength: Interface Descriptor size*/
		  USB_DESC_TYPE_INTERFACE,/*bDescriptorType: Interface descriptor type*/
		  0x00,                   /*bInterfaceNumber: Index of this interface.*/
		  0x00,                   /*bAlternateSetting: Alternate setting*/
		  0x02,                   /*bNumEndpoints*/
		  0x01,                   /*bInterfaceClass: AUDIO*/
		  0x03,                   /*bInterfaceSubClass : MIDISTREAMING*/
		  0x00,                   /*nInterfaceProtocol : Unused*/
		  0x00,                   /*iInterface: Unused*/

		  /******************** MIDI Adapter Class-specific MS Interface Descriptor ********************/
		  /* USB_MIDI_CLASS_DESC_SHIFT */
		  0x07,                 /*bLength: Descriptor size*/
		  0x24,                 /*bDescriptorType: CS_INTERFACE descriptor*/
		  0x01,                 /*bDescriptorSubtype: MS_HEADER subtype*/
		  0x00,
		  0x01,                 /*BcdADC: Revision of this class specification*/
		  USB_MIDI_INTERFACE_DESC_SIZE,
		  0x00,                  /*wTotalLength: Total size of class-specific descriptors*/

		#if MIDI_IN_PORTS_NUM >= 1
		  /******************** MIDI Adapter MIDI IN Jack Descriptor (External) ********************/
		  0x06,                   /*bLength: Size of this descriptor, in bytes*/
		  0x24,                   /*bDescriptorType: CS_INTERFACE descriptor.*/
		  0x02,                   /*bDescriptorSubtype: MIDI_IN_JACK subtype*/
		  0x02,                   /*bJackType: EXTERNAL.*/
		  MIDI_JACK_1,            /*bJackID: ID of this Jack.*/
		  0x00,                   /*iJack: Unused.*/

		  /******************** MIDI Adapter MIDI OUT Jack Descriptor (Embedded) ********************/
		  0x09,                   /*bLength: Size of this descriptor, in bytes*/
		  0x24,                   /*bDescriptorType: CS_INTERFACE descriptor.*/
		  0x03,                   /*bDescriptorSubtype: MIDI_OUT_JACK subtype*/
		  0x01,                   /*bJackType: EMBEDDED*/
		  MIDI_JACK_2,            /*bJackID: ID of this Jack.*/
		  0x01,                   /*bNrInputPins: Number of Input Pins of this Jack.*/
		  MIDI_JACK_1,            /*BaSourceID(1): ID of the Entity to which this Pin is connected.*/
		  0x01,                   /*BaSourcePin(1): Output Pin number of the Entity to which this Input Pin is connected.*/
		  0x00,                   /*iJack: Unused.*/
		#endif

		#if MIDI_IN_PORTS_NUM >= 2
		  /******************** MIDI Adapter MIDI IN Jack Descriptor (External) ********************/
		  0x06,                   /*bLength: Size of this descriptor, in bytes*/
		  0x24,                   /*bDescriptorType: CS_INTERFACE descriptor.*/
		  0x02,                   /*bDescriptorSubtype: MIDI_IN_JACK subtype*/
		  0x02,                   /*bJackType: EXTERNAL.*/
		  MIDI_JACK_3,            /*bJackID: ID of this Jack.*/
		  0x00,                   /*iJack: Unused.*/

		  /******************** MIDI Adapter MIDI OUT Jack Descriptor (Embedded) ********************/
		  0x09,                   /*bLength: Size of this descriptor, in bytes*/
		  0x24,                   /*bDescriptorType: CS_INTERFACE descriptor.*/
		  0x03,                   /*bDescriptorSubtype: MIDI_OUT_JACK subtype*/
		  0x01,                   /*bJackType: EMBEDDED*/
		  MIDI_JACK_4,            /*bJackID: ID of this Jack.*/
		  0x01,                   /*bNrInputPins: Number of Input Pins of this Jack.*/
		  MIDI_JACK_3,            /*BaSourceID(1): ID of the Entity to which this Pin is connected.*/
		  0x01,                   /*BaSourcePin(1): Output Pin number of the Entity to which this Input Pin is connected.*/
		  0x00,                   /*iJack: Unused.*/
		#endif

		#if MIDI_IN_PORTS_NUM >= 3
		  /******************** MIDI Adapter MIDI IN Jack Descriptor (External) ********************/
		  0x06,                   /*bLength: Size of this descriptor, in bytes*/
		  0x24,                   /*bDescriptorType: CS_INTERFACE descriptor.*/
		  0x02,                   /*bDescriptorSubtype: MIDI_IN_JACK subtype*/
		  0x02,                   /*bJackType: EXTERNAL.*/
		  MIDI_JACK_5,            /*bJackID: ID of this Jack.*/
		  0x00,                   /*iJack: Unused.*/

		  /******************** MIDI Adapter MIDI OUT Jack Descriptor (Embedded) ********************/
		  0x09,                   /*bLength: Size of this descriptor, in bytes*/
		  0x24,                   /*bDescriptorType: CS_INTERFACE descriptor.*/
		  0x03,                   /*bDescriptorSubtype: MIDI_OUT_JACK subtype*/
		  0x01,                   /*bJackType: EMBEDDED*/
		  MIDI_JACK_6,            /*bJackID: ID of this Jack.*/
		  0x01,                   /*bNrInputPins: Number of Input Pins of this Jack.*/
		  MIDI_JACK_5,            /*BaSourceID(1): ID of the Entity to which this Pin is connected.*/
		  0x01,                   /*BaSourcePin(1): Output Pin number of the Entity to which this Input Pin is connected.*/
		  0x00,                   /*iJack: Unused.*/
		#endif

		#if MIDI_IN_PORTS_NUM >= 4
		  /******************** MIDI Adapter MIDI IN Jack Descriptor (External) ********************/
		  0x06,                   /*bLength: Size of this descriptor, in bytes*/
		  0x24,                   /*bDescriptorType: CS_INTERFACE descriptor.*/
		  0x02,                   /*bDescriptorSubtype: MIDI_IN_JACK subtype*/
		  0x02,                   /*bJackType: EXTERNAL.*/
		  MIDI_JACK_7,            /*bJackID: ID of this Jack.*/
		  0x00,                   /*iJack: Unused.*/

		  /******************** MIDI Adapter MIDI OUT Jack Descriptor (Embedded) ********************/
		  0x09,                   /*bLength: Size of this descriptor, in bytes*/
		  0x24,                   /*bDescriptorType: CS_INTERFACE descriptor.*/
		  0x03,                   /*bDescriptorSubtype: MIDI_OUT_JACK subtype*/
		  0x01,                   /*bJackType: EMBEDDED*/
		  MIDI_JACK_8,            /*bJackID: ID of this Jack.*/
		  0x01,                   /*bNrInputPins: Number of Input Pins of this Jack.*/
		  MIDI_JACK_7,            /*BaSourceID(1): ID of the Entity to which this Pin is connected.*/
		  0x01,                   /*BaSourcePin(1): Output Pin number of the Entity to which this Input Pin is connected.*/
		  0x00,                   /*iJack: Unused.*/
		#endif

		#if MIDI_IN_PORTS_NUM >= 5
		  /******************** MIDI Adapter MIDI IN Jack Descriptor (External) ********************/
		  0x06,                   /*bLength: Size of this descriptor, in bytes*/
		  0x24,                   /*bDescriptorType: CS_INTERFACE descriptor.*/
		  0x02,                   /*bDescriptorSubtype: MIDI_IN_JACK subtype*/
		  0x02,                   /*bJackType: EXTERNAL.*/
		  MIDI_JACK_9,            /*bJackID: ID of this Jack.*/
		  0x00,                   /*iJack: Unused.*/

		  /******************** MIDI Adapter MIDI OUT Jack Descriptor (Embedded) ********************/
		  0x09,                   /*bLength: Size of this descriptor, in bytes*/
		  0x24,                   /*bDescriptorType: CS_INTERFACE descriptor.*/
		  0x03,                   /*bDescriptorSubtype: MIDI_OUT_JACK subtype*/
		  0x01,                   /*bJackType: EMBEDDED*/
		  MIDI_JACK_10,           /*bJackID: ID of this Jack.*/
		  0x01,                   /*bNrInputPins: Number of Input Pins of this Jack.*/
		  MIDI_JACK_9,            /*BaSourceID(1): ID of the Entity to which this Pin is connected.*/
		  0x01,                   /*BaSourcePin(1): Output Pin number of the Entity to which this Input Pin is connected.*/
		  0x00,                   /*iJack: Unused.*/
		#endif

		#if MIDI_IN_PORTS_NUM >= 6
		  /******************** MIDI Adapter MIDI IN Jack Descriptor (External) ********************/
		  0x06,                   /*bLength: Size of this descriptor, in bytes*/
		  0x24,                   /*bDescriptorType: CS_INTERFACE descriptor.*/
		  0x02,                   /*bDescriptorSubtype: MIDI_IN_JACK subtype*/
		  0x02,                   /*bJackType: EXTERNAL.*/
		  MIDI_JACK_11,           /*bJackID: ID of this Jack.*/
		  0x00,                   /*iJack: Unused.*/

		  /******************** MIDI Adapter MIDI OUT Jack Descriptor (Embedded) ********************/
		  0x09,                   /*bLength: Size of this descriptor, in bytes*/
		  0x24,                   /*bDescriptorType: CS_INTERFACE descriptor.*/
		  0x03,                   /*bDescriptorSubtype: MIDI_OUT_JACK subtype*/
		  0x01,                   /*bJackType: EMBEDDED*/
		  MIDI_JACK_12,           /*bJackID: ID of this Jack.*/
		  0x01,                   /*bNrInputPins: Number of Input Pins of this Jack.*/
		  MIDI_JACK_11,           /*BaSourceID(1): ID of the Entity to which this Pin is connected.*/
		  0x01,                   /*BaSourcePin(1): Output Pin number of the Entity to which this Input Pin is connected.*/
		  0x00,                   /*iJack: Unused.*/
		#endif

		#if MIDI_IN_PORTS_NUM >= 7
		  /******************** MIDI Adapter MIDI IN Jack Descriptor (External) ********************/
		  0x06,                   /*bLength: Size of this descriptor, in bytes*/
		  0x24,                   /*bDescriptorType: CS_INTERFACE descriptor.*/
		  0x02,                   /*bDescriptorSubtype: MIDI_IN_JACK subtype*/
		  0x02,                   /*bJackType: EXTERNAL.*/
		  MIDI_JACK_13,           /*bJackID: ID of this Jack.*/
		  0x00,                   /*iJack: Unused.*/

		  /******************** MIDI Adapter MIDI OUT Jack Descriptor (Embedded) ********************/
		  0x09,                   /*bLength: Size of this descriptor, in bytes*/
		  0x24,                   /*bDescriptorType: CS_INTERFACE descriptor.*/
		  0x03,                   /*bDescriptorSubtype: MIDI_OUT_JACK subtype*/
		  0x01,                   /*bJackType: EMBEDDED*/
		  MIDI_JACK_14,           /*bJackID: ID of this Jack.*/
		  0x01,                   /*bNrInputPins: Number of Input Pins of this Jack.*/
		  MIDI_JACK_13,           /*BaSourceID(1): ID of the Entity to which this Pin is connected.*/
		  0x01,                   /*BaSourcePin(1): Output Pin number of the Entity to which this Input Pin is connected.*/
		  0x00,                   /*iJack: Unused.*/
		#endif

		#if MIDI_IN_PORTS_NUM >= 8
		  /******************** MIDI Adapter MIDI IN Jack Descriptor (External) ********************/
		  0x06,                   /*bLength: Size of this descriptor, in bytes*/
		  0x24,                   /*bDescriptorType: CS_INTERFACE descriptor.*/
		  0x02,                   /*bDescriptorSubtype: MIDI_IN_JACK subtype*/
		  0x02,                   /*bJackType: EXTERNAL.*/
		  MIDI_JACK_15,           /*bJackID: ID of this Jack.*/
		  0x00,                   /*iJack: Unused.*/

		  /******************** MIDI Adapter MIDI OUT Jack Descriptor (Embedded) ********************/
		  0x09,                   /*bLength: Size of this descriptor, in bytes*/
		  0x24,                   /*bDescriptorType: CS_INTERFACE descriptor.*/
		  0x03,                   /*bDescriptorSubtype: MIDI_OUT_JACK subtype*/
		  0x01,                   /*bJackType: EMBEDDED*/
		  MIDI_JACK_16,           /*bJackID: ID of this Jack.*/
		  0x01,                   /*bNrInputPins: Number of Input Pins of this Jack.*/
		  MIDI_JACK_15,           /*BaSourceID(1): ID of the Entity to which this Pin is connected.*/
		  0x01,                   /*BaSourcePin(1): Output Pin number of the Entity to which this Input Pin is connected.*/
		  0x00,                   /*iJack: Unused.*/
		#endif

		#if MIDI_OUT_PORTS_NUM >= 1
		  /******************** MIDI Adapter MIDI IN Jack Descriptor (Embedded) ********************/
		  0x06,                   /*bLength: Size of this descriptor, in bytes*/
		  0x24,                   /*bDescriptorType: CS_INTERFACE descriptor.*/
		  0x02,                   /*bDescriptorSubtype: MIDI_IN_JACK subtype*/
		  0x01,                   /*bJackType: EMBEDDED*/
		  MIDI_JACK_17,           /*bJackID: ID of this Jack.*/
		  0x00,                   /*iJack: Unused.*/

		  /******************** MIDI Adapter MIDI OUT Jack Descriptor (External) ********************/
		  0x09,                   /*bLength: Size of this descriptor, in bytes*/
		  0x24,                   /*bDescriptorType: CS_INTERFACE descriptor.*/
		  0x03,                   /*bDescriptorSubtype: MIDI_OUT_JACK subtype*/
		  0x02,                   /*bJackType: EXTERNAL.*/
		  MIDI_JACK_18,           /*bJackID: ID of this Jack.*/
		  0x01,                   /*bNrInputPins: Number of Input Pins of this Jack.*/
		  MIDI_JACK_17,           /*BaSourceID(1): ID of the Entity to which this Pin is connected.*/
		  0x01,                   /*BaSourcePin(1): Output Pin number of the Entity to which this Input Pin is connected.*/
		  0x00,                   /*iJack: Unused.*/
		#endif

		#if MIDI_OUT_PORTS_NUM >= 2
		  /******************** MIDI Adapter MIDI IN Jack Descriptor (Embedded) ********************/
		  0x06,                   /*bLength: Size of this descriptor, in bytes*/
		  0x24,                   /*bDescriptorType: CS_INTERFACE descriptor.*/
		  0x02,                   /*bDescriptorSubtype: MIDI_IN_JACK subtype*/
		  0x01,                   /*bJackType: EMBEDDED*/
		  MIDI_JACK_19,           /*bJackID: ID of this Jack.*/
		  0x00,                   /*iJack: Unused.*/

		  /******************** MIDI Adapter MIDI OUT Jack Descriptor (External) ********************/
		  0x09,                   /*bLength: Size of this descriptor, in bytes*/
		  0x24,                   /*bDescriptorType: CS_INTERFACE descriptor.*/
		  0x03,                   /*bDescriptorSubtype: MIDI_OUT_JACK subtype*/
		  0x02,                   /*bJackType: EXTERNAL.*/
		  MIDI_JACK_20,           /*bJackID: ID of this Jack.*/
		  0x01,                   /*bNrInputPins: Number of Input Pins of this Jack.*/
		  MIDI_JACK_19,           /*BaSourceID(1): ID of the Entity to which this Pin is connected.*/
		  0x01,                   /*BaSourcePin(1): Output Pin number of the Entity to which this Input Pin is connected.*/
		  0x00,                   /*iJack: Unused.*/
		#endif

		#if MIDI_OUT_PORTS_NUM >= 3
		  /******************** MIDI Adapter MIDI IN Jack Descriptor (Embedded) ********************/
		  0x06,                   /*bLength: Size of this descriptor, in bytes*/
		  0x24,                   /*bDescriptorType: CS_INTERFACE descriptor.*/
		  0x02,                   /*bDescriptorSubtype: MIDI_IN_JACK subtype*/
		  0x01,                   /*bJackType: EMBEDDED*/
		  MIDI_JACK_21,           /*bJackID: ID of this Jack.*/
		  0x00,                   /*iJack: Unused.*/

		  /******************** MIDI Adapter MIDI OUT Jack Descriptor (External) ********************/
		  0x09,                   /*bLength: Size of this descriptor, in bytes*/
		  0x24,                   /*bDescriptorType: CS_INTERFACE descriptor.*/
		  0x03,                   /*bDescriptorSubtype: MIDI_OUT_JACK subtype*/
		  0x02,                   /*bJackType: EXTERNAL.*/
		  MIDI_JACK_22,           /*bJackID: ID of this Jack.*/
		  0x01,                   /*bNrInputPins: Number of Input Pins of this Jack.*/
		  MIDI_JACK_21,           /*BaSourceID(1): ID of the Entity to which this Pin is connected.*/
		  0x01,                   /*BaSourcePin(1): Output Pin number of the Entity to which this Input Pin is connected.*/
		  0x00,                   /*iJack: Unused.*/
		#endif

		#if MIDI_OUT_PORTS_NUM >= 4
		  /******************** MIDI Adapter MIDI IN Jack Descriptor (Embedded) ********************/
		  0x06,                   /*bLength: Size of this descriptor, in bytes*/
		  0x24,                   /*bDescriptorType: CS_INTERFACE descriptor.*/
		  0x02,                   /*bDescriptorSubtype: MIDI_IN_JACK subtype*/
		  0x01,                   /*bJackType: EMBEDDED*/
		  MIDI_JACK_23,           /*bJackID: ID of this Jack.*/
		  0x00,                   /*iJack: Unused.*/

		  /******************** MIDI Adapter MIDI OUT Jack Descriptor (External) ********************/
		  0x09,                   /*bLength: Size of this descriptor, in bytes*/
		  0x24,                   /*bDescriptorType: CS_INTERFACE descriptor.*/
		  0x03,                   /*bDescriptorSubtype: MIDI_OUT_JACK subtype*/
		  0x02,                   /*bJackType: EXTERNAL.*/
		  MIDI_JACK_24,           /*bJackID: ID of this Jack.*/
		  0x01,                   /*bNrInputPins: Number of Input Pins of this Jack.*/
		  MIDI_JACK_23,           /*BaSourceID(1): ID of the Entity to which this Pin is connected.*/
		  0x01,                   /*BaSourcePin(1): Output Pin number of the Entity to which this Input Pin is connected.*/
		  0x00,                   /*iJack: Unused.*/
		#endif

		#if MIDI_OUT_PORTS_NUM >= 5
		  /******************** MIDI Adapter MIDI IN Jack Descriptor (Embedded) ********************/
		  0x06,                   /*bLength: Size of this descriptor, in bytes*/
		  0x24,                   /*bDescriptorType: CS_INTERFACE descriptor.*/
		  0x02,                   /*bDescriptorSubtype: MIDI_IN_JACK subtype*/
		  0x01,                   /*bJackType: EMBEDDED*/
		  MIDI_JACK_25,           /*bJackID: ID of this Jack.*/
		  0x00,                   /*iJack: Unused.*/

		  /******************** MIDI Adapter MIDI OUT Jack Descriptor (External) ********************/
		  0x09,                   /*bLength: Size of this descriptor, in bytes*/
		  0x24,                   /*bDescriptorType: CS_INTERFACE descriptor.*/
		  0x03,                   /*bDescriptorSubtype: MIDI_OUT_JACK subtype*/
		  0x02,                   /*bJackType: EXTERNAL.*/
		  MIDI_JACK_26,           /*bJackID: ID of this Jack.*/
		  0x01,                   /*bNrInputPins: Number of Input Pins of this Jack.*/
		  MIDI_JACK_25,           /*BaSourceID(1): ID of the Entity to which this Pin is connected.*/
		  0x01,                   /*BaSourcePin(1): Output Pin number of the Entity to which this Input Pin is connected.*/
		  0x00,                   /*iJack: Unused.*/
		#endif

		#if MIDI_OUT_PORTS_NUM >= 6
		  /******************** MIDI Adapter MIDI IN Jack Descriptor (Embedded) ********************/
		  0x06,                   /*bLength: Size of this descriptor, in bytes*/
		  0x24,                   /*bDescriptorType: CS_INTERFACE descriptor.*/
		  0x02,                   /*bDescriptorSubtype: MIDI_IN_JACK subtype*/
		  0x01,                   /*bJackType: EMBEDDED*/
		  MIDI_JACK_27,           /*bJackID: ID of this Jack.*/
		  0x00,                   /*iJack: Unused.*/

		  /******************** MIDI Adapter MIDI OUT Jack Descriptor (External) ********************/
		  0x09,                   /*bLength: Size of this descriptor, in bytes*/
		  0x24,                   /*bDescriptorType: CS_INTERFACE descriptor.*/
		  0x03,                   /*bDescriptorSubtype: MIDI_OUT_JACK subtype*/
		  0x02,                   /*bJackType: EXTERNAL.*/
		  MIDI_JACK_28,           /*bJackID: ID of this Jack.*/
		  0x01,                   /*bNrInputPins: Number of Input Pins of this Jack.*/
		  MIDI_JACK_27,           /*BaSourceID(1): ID of the Entity to which this Pin is connected.*/
		  0x01,                   /*BaSourcePin(1): Output Pin number of the Entity to which this Input Pin is connected.*/
		  0x00,                   /*iJack: Unused.*/
		#endif

		#if MIDI_OUT_PORTS_NUM >= 7
		  /******************** MIDI Adapter MIDI IN Jack Descriptor (Embedded) ********************/
		  0x06,                   /*bLength: Size of this descriptor, in bytes*/
		  0x24,                   /*bDescriptorType: CS_INTERFACE descriptor.*/
		  0x02,                   /*bDescriptorSubtype: MIDI_IN_JACK subtype*/
		  0x01,                   /*bJackType: EMBEDDED*/
		  MIDI_JACK_29,           /*bJackID: ID of this Jack.*/
		  0x00,                   /*iJack: Unused.*/

		  /******************** MIDI Adapter MIDI OUT Jack Descriptor (External) ********************/
		  0x09,                   /*bLength: Size of this descriptor, in bytes*/
		  0x24,                   /*bDescriptorType: CS_INTERFACE descriptor.*/
		  0x03,                   /*bDescriptorSubtype: MIDI_OUT_JACK subtype*/
		  0x02,                   /*bJackType: EXTERNAL.*/
		  MIDI_JACK_30,           /*bJackID: ID of this Jack.*/
		  0x01,                   /*bNrInputPins: Number of Input Pins of this Jack.*/
		  MIDI_JACK_29,           /*BaSourceID(1): ID of the Entity to which this Pin is connected.*/
		  0x01,                   /*BaSourcePin(1): Output Pin number of the Entity to which this Input Pin is connected.*/
		  0x00,                   /*iJack: Unused.*/
		#endif

		#if MIDI_OUT_PORTS_NUM >= 8
		  /******************** MIDI Adapter MIDI IN Jack Descriptor (Embedded) ********************/
		  0x06,                   /*bLength: Size of this descriptor, in bytes*/
		  0x24,                   /*bDescriptorType: CS_INTERFACE descriptor.*/
		  0x02,                   /*bDescriptorSubtype: MIDI_IN_JACK subtype*/
		  0x01,                   /*bJackType: EMBEDDED*/
		  MIDI_JACK_31,           /*bJackID: ID of this Jack.*/
		  0x00,                   /*iJack: Unused.*/

		  /******************** MIDI Adapter MIDI OUT Jack Descriptor (External) ********************/
		  0x09,                   /*bLength: Size of this descriptor, in bytes*/
		  0x24,                   /*bDescriptorType: CS_INTERFACE descriptor.*/
		  0x03,                   /*bDescriptorSubtype: MIDI_OUT_JACK subtype*/
		  0x02,                   /*bJackType: EXTERNAL.*/
		  MIDI_JACK_32,           /*bJackID: ID of this Jack.*/
		  0x01,                   /*bNrInputPins: Number of Input Pins of this Jack.*/
		  MIDI_JACK_31,           /*BaSourceID(1): ID of the Entity to which this Pin is connected.*/
		  0x01,                   /*BaSourcePin(1): Output Pin number of the Entity to which this Input Pin is connected.*/
		  0x00,                   /*iJack: Unused.*/
		#endif

		  /******************** MIDI Adapter Standard Bulk OUT Endpoint Descriptor ********************/
		  0x09,                   /*bLength: Size of this descriptor, in bytes*/
		  USB_DESC_TYPE_ENDPOINT, /*bDescriptorType: ENDPOINT descriptor.*/
		  MIDI_OUT_EP,        /*bEndpointAddress: OUT Endpoint 1.*/
		  0x02,                   /*bmAttributes: Bulk, not shared.*/
		  MIDI_OUT_EP_SIZE,
		  0x00,                   /*wMaxPacketSize*/
		  0x00,                   /*bInterval: Ignored for Bulk. Set to zero.*/
		  0x00,                   /*bRefresh: Unused.*/
		  0x00,                   /*bSynchAddress: Unused.*/

		  /******************** MIDI Adapter Class-specific Bulk OUT Endpoint Descriptor ********************/
		  (4 + MIDI_OUT_PORTS_NUM), /*bLength: Size of this descriptor, in bytes*/
		  0x25,                     /*bDescriptorType: CS_ENDPOINT descriptor*/
		  0x01,                     /*bDescriptorSubtype: MS_GENERAL subtype.*/
		  MIDI_OUT_PORTS_NUM,       /*bNumEmbMIDIJack: Number of embedded MIDI IN Jacks.*/
		#if MIDI_OUT_PORTS_NUM >= 1
		  MIDI_JACK_17,             /*BaAssocJackID(1): ID of the Embedded MIDI IN Jack.*/
		#endif
		#if MIDI_OUT_PORTS_NUM >= 2
		  MIDI_JACK_19,             /*BaAssocJackID(2): ID of the Embedded MIDI IN Jack.*/
		#endif
		#if MIDI_OUT_PORTS_NUM >= 3
		  MIDI_JACK_21,             /*BaAssocJackID(3): ID of the Embedded MIDI IN Jack.*/
		#endif
		#if MIDI_OUT_PORTS_NUM >= 4
		  MIDI_JACK_23,             /*BaAssocJackID(4): ID of the Embedded MIDI IN Jack.*/
		#endif
		#if MIDI_OUT_PORTS_NUM >= 5
		  MIDI_JACK_25,             /*BaAssocJackID(5): ID of the Embedded MIDI IN Jack.*/
		#endif
		#if MIDI_OUT_PORTS_NUM >= 6
		  MIDI_JACK_27,             /*BaAssocJackID(6): ID of the Embedded MIDI IN Jack.*/
		#endif
		#if MIDI_OUT_PORTS_NUM >= 7
		  MIDI_JACK_29,             /*BaAssocJackID(7): ID of the Embedded MIDI IN Jack.*/
		#endif
		#if MIDI_OUT_PORTS_NUM >= 8
		  MIDI_JACK_31,             /*BaAssocJackID(8): ID of the Embedded MIDI IN Jack.*/
		#endif

		  /******************** MIDI Adapter Standard Bulk IN Endpoint Descriptor ********************/
		  0x09,                    /*bLength: Size of this descriptor, in bytes*/
		  USB_DESC_TYPE_ENDPOINT,  /*bDescriptorType: ENDPOINT descriptor.*/
		  MIDI_IN_EP,          /*bEndpointAddress: IN Endpoint 1.*/
		  0x02,                    /*bmAttributes: Bulk, not shared.*/
		  MIDI_IN_EPSIZE,
		  0x00,                    /*wMaxPacketSize*/
		  0x00,                    /*bInterval: Ignored for Bulk. Set to zero.*/
		  0x00,                    /*bRefresh: Unused.*/
		  0x00,                    /*bSynchAddress: Unused.*/

		  /******************** MIDI Adapter Class-specific Bulk IN Endpoint Descriptor ********************/
		  (4 + MIDI_IN_PORTS_NUM), /*bLength: Size of this descriptor, in bytes*/
		  0x25,                    /*bDescriptorType: CS_ENDPOINT descriptor*/
		  0x01,                    /*bDescriptorSubtype: MS_GENERAL subtype.*/
		  MIDI_IN_PORTS_NUM,       /*bNumEmbMIDIJack: Number of embedded MIDI OUT Jacks.*/
		#if MIDI_IN_PORTS_NUM >= 1
		  MIDI_JACK_2,             /*BaAssocJackID(1): ID of the Embedded MIDI OUT Jack.*/
		#endif
		#if MIDI_IN_PORTS_NUM >= 2
		  MIDI_JACK_4,             /*BaAssocJackID(2): ID of the Embedded MIDI OUT Jack.*/
		#endif
		#if MIDI_IN_PORTS_NUM >= 3
		  MIDI_JACK_6,             /*BaAssocJackID(3): ID of the Embedded MIDI OUT Jack.*/
		#endif
		#if MIDI_IN_PORTS_NUM >= 4
		  MIDI_JACK_8,             /*BaAssocJackID(4): ID of the Embedded MIDI OUT Jack.*/
		#endif
		#if MIDI_IN_PORTS_NUM >= 5
		  MIDI_JACK_10,            /*BaAssocJackID(5): ID of the Embedded MIDI OUT Jack.*/
		#endif
		#if MIDI_IN_PORTS_NUM >= 6
		  MIDI_JACK_12,            /*BaAssocJackID(6): ID of the Embedded MIDI OUT Jack.*/
		#endif
		#if MIDI_IN_PORTS_NUM >= 7
		  MIDI_JACK_14,            /*BaAssocJackID(7): ID of the Embedded MIDI OUT Jack.*/
		#endif
		#if MIDI_IN_PORTS_NUM >= 8
		  MIDI_JACK_16,            /*BaAssocJackID(8): ID of the Embedded MIDI OUT Jack.*/
		#endif
};

#endif /* USE_USBD_COMPOSITE  */

static uint8_t MIDIInEpAdd = MIDI_IN_EP;
static uint8_t MIDIOutEpAdd = MIDI_OUT_EP;
static uint8_t MIDICmdEpAdd = MIDI_CMD_EP;

static uint8_t USBD_MIDI_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  UNUSED(cfgidx);
  USBD_MIDI_HandleTypeDef *hcdc;

  hcdc = (USBD_MIDI_HandleTypeDef *)USBD_malloc(sizeof(USBD_MIDI_HandleTypeDef));

  if (hcdc == NULL)
  {
    pdev->pClassDataCmsit[pdev->classId] = NULL;
    return (uint8_t)USBD_EMEM;
  }

  (void)USBD_memset(hcdc, 0, sizeof(USBD_MIDI_HandleTypeDef));

  pdev->pClassDataCmsit[pdev->classId] = (void *)hcdc;
  pdev->pClassData = pdev->pClassDataCmsit[pdev->classId];

#ifdef USE_USBD_COMPOSITE
  /* Get the Endpoints addresses allocated for this class instance */
  MIDIInEpAdd  = USBD_CoreGetEPAdd(pdev, USBD_EP_IN, USBD_EP_TYPE_BULK, (uint8_t)pdev->classId);
  MIDIOutEpAdd = USBD_CoreGetEPAdd(pdev, USBD_EP_OUT, USBD_EP_TYPE_BULK, (uint8_t)pdev->classId);
  MIDICmdEpAdd = USBD_CoreGetEPAdd(pdev, USBD_EP_IN, USBD_EP_TYPE_INTR, (uint8_t)pdev->classId);
#endif /* USE_USBD_COMPOSITE */

  if (pdev->dev_speed == USBD_SPEED_HIGH)
  {
    /* Open EP IN */
    (void)USBD_LL_OpenEP(pdev, MIDIInEpAdd, USBD_EP_TYPE_BULK,
                         MIDI_DATA_HS_IN_PACKET_SIZE);

    pdev->ep_in[MIDIInEpAdd & 0xFU].is_used = 1U;

    /* Open EP OUT */
    (void)USBD_LL_OpenEP(pdev, MIDIOutEpAdd, USBD_EP_TYPE_BULK,
                         MIDI_DATA_HS_OUT_PACKET_SIZE);

    pdev->ep_out[MIDIOutEpAdd & 0xFU].is_used = 1U;

    /* Set bInterval for MIDI CMD Endpoint */
    pdev->ep_in[MIDICmdEpAdd & 0xFU].bInterval = MIDI_HS_BINTERVAL;
  }
  else
  {
    /* Open EP IN */
    (void)USBD_LL_OpenEP(pdev, MIDIInEpAdd, USBD_EP_TYPE_BULK,
                         MIDI_DATA_FS_IN_PACKET_SIZE);

    pdev->ep_in[MIDIInEpAdd & 0xFU].is_used = 1U;

    /* Open EP OUT */
    (void)USBD_LL_OpenEP(pdev, MIDIOutEpAdd, USBD_EP_TYPE_BULK,
                         MIDI_DATA_FS_OUT_PACKET_SIZE);

    pdev->ep_out[MIDIOutEpAdd & 0xFU].is_used = 1U;

    /* Set bInterval for CMD Endpoint */
    pdev->ep_in[MIDICmdEpAdd & 0xFU].bInterval = MIDI_FS_BINTERVAL;
  }

  /* Open Command IN EP */
  (void)USBD_LL_OpenEP(pdev, MIDICmdEpAdd, USBD_EP_TYPE_INTR, MIDI_CMD_PACKET_SIZE);
  pdev->ep_in[MIDICmdEpAdd & 0xFU].is_used = 1U;

  hcdc->RxBuffer = NULL;

  /* Init  physical Interface components */
  ((USBD_MIDI_ItfTypeDef *)pdev->pUserData[pdev->classId])->Init();

  /* Init Xfer states */
  hcdc->TxState = 0U;
  hcdc->RxState = 0U;

  if (hcdc->RxBuffer == NULL)
  {
    return (uint8_t)USBD_EMEM;
  }

  if (pdev->dev_speed == USBD_SPEED_HIGH)
  {
    /* Prepare Out endpoint to receive next packet */
    (void)USBD_LL_PrepareReceive(pdev, MIDIOutEpAdd, hcdc->RxBuffer,
                                 MIDI_DATA_HS_OUT_PACKET_SIZE);
  }
  else
  {
    /* Prepare Out endpoint to receive next packet */
    (void)USBD_LL_PrepareReceive(pdev, MIDIOutEpAdd, hcdc->RxBuffer,
                                 MIDI_DATA_FS_OUT_PACKET_SIZE);
  }

  return (uint8_t)USBD_OK;
}

static uint8_t USBD_MIDI_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  UNUSED(cfgidx);


#ifdef USE_USBD_COMPOSITE
  /* Get the Endpoints addresses allocated for this MIDI class instance */
  MIDIInEpAdd  = USBD_CoreGetEPAdd(pdev, USBD_EP_IN, USBD_EP_TYPE_BULK, (uint8_t)pdev->classId);
  MIDIOutEpAdd = USBD_CoreGetEPAdd(pdev, USBD_EP_OUT, USBD_EP_TYPE_BULK, (uint8_t)pdev->classId);
  MIDICmdEpAdd = USBD_CoreGetEPAdd(pdev, USBD_EP_IN, USBD_EP_TYPE_INTR, (uint8_t)pdev->classId);
#endif /* USE_USBD_COMPOSITE */

  /* Close EP IN */
  (void)USBD_LL_CloseEP(pdev, MIDIInEpAdd);
  pdev->ep_in[MIDIInEpAdd & 0xFU].is_used = 0U;

  /* Close EP OUT */
  (void)USBD_LL_CloseEP(pdev, MIDIOutEpAdd);
  pdev->ep_out[MIDIOutEpAdd & 0xFU].is_used = 0U;

  /* Close Command IN EP */
  (void)USBD_LL_CloseEP(pdev, MIDICmdEpAdd);
  pdev->ep_in[MIDICmdEpAdd & 0xFU].is_used = 0U;
  pdev->ep_in[MIDICmdEpAdd & 0xFU].bInterval = 0U;

  /* DeInit  physical Interface components */
  if (pdev->pClassDataCmsit[pdev->classId] != NULL)
  {
    ((USBD_MIDI_ItfTypeDef *)pdev->pUserData[pdev->classId])->DeInit();
    (void)USBD_free(pdev->pClassDataCmsit[pdev->classId]);
    pdev->pClassDataCmsit[pdev->classId] = NULL;
    pdev->pClassData = NULL;
  }

  return (uint8_t)USBD_OK;
}

static uint8_t USBD_MIDI_Setup(USBD_HandleTypeDef *pdev,
                              USBD_SetupReqTypedef *req)
{
  USBD_MIDI_HandleTypeDef *hcdc = (USBD_MIDI_HandleTypeDef *)pdev->pClassDataCmsit[pdev->classId];
  uint16_t len;
  uint8_t ifalt = 0U;
  uint16_t status_info = 0U;
  USBD_StatusTypeDef ret = USBD_OK;

  if (hcdc == NULL)
  {
    return (uint8_t)USBD_FAIL;
  }

  switch (req->bmRequest & USB_REQ_TYPE_MASK)
  {
    case USB_REQ_TYPE_CLASS:
      if (req->wLength != 0U)
      {
        if ((req->bmRequest & 0x80U) != 0U)
        {
          ((USBD_MIDI_ItfTypeDef *)pdev->pUserData[pdev->classId])->Control(req->bRequest,
                                                                           (uint8_t *)hcdc->data,
                                                                           req->wLength);

          len = MIN(MIDI_REQ_MAX_DATA_SIZE, req->wLength);
          (void)USBD_CtlSendData(pdev, (uint8_t *)hcdc->data, len);
        }
        else
        {
          hcdc->CmdOpCode = req->bRequest;
          hcdc->CmdLength = (uint8_t)MIN(req->wLength, USB_MAX_EP0_SIZE);

          (void)USBD_CtlPrepareRx(pdev, (uint8_t *)hcdc->data, hcdc->CmdLength);
        }
      }
      else
      {
        ((USBD_MIDI_ItfTypeDef *)pdev->pUserData[pdev->classId])->Control(req->bRequest,
                                                                         (uint8_t *)req, 0U);
      }
      break;

    case USB_REQ_TYPE_STANDARD:
      switch (req->bRequest)
      {
        case USB_REQ_GET_STATUS:
          if (pdev->dev_state == USBD_STATE_CONFIGURED)
          {
            (void)USBD_CtlSendData(pdev, (uint8_t *)&status_info, 2U);
          }
          else
          {
            USBD_CtlError(pdev, req);
            ret = USBD_FAIL;
          }
          break;

        case USB_REQ_GET_INTERFACE:
          if (pdev->dev_state == USBD_STATE_CONFIGURED)
          {
            (void)USBD_CtlSendData(pdev, &ifalt, 1U);
          }
          else
          {
            USBD_CtlError(pdev, req);
            ret = USBD_FAIL;
          }
          break;

        case USB_REQ_SET_INTERFACE:
          if (pdev->dev_state != USBD_STATE_CONFIGURED)
          {
            USBD_CtlError(pdev, req);
            ret = USBD_FAIL;
          }
          break;

        case USB_REQ_CLEAR_FEATURE:
          break;

        default:
          USBD_CtlError(pdev, req);
          ret = USBD_FAIL;
          break;
      }
      break;

    default:
      USBD_CtlError(pdev, req);
      ret = USBD_FAIL;
      break;
  }

  return (uint8_t)ret;
}

static uint8_t USBD_MIDI_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  USBD_MIDI_HandleTypeDef *hcdc;
  PCD_HandleTypeDef *hpcd = (PCD_HandleTypeDef *)pdev->pData;

  if (pdev->pClassDataCmsit[pdev->classId] == NULL)
  {
    return (uint8_t)USBD_FAIL;
  }

  hcdc = (USBD_MIDI_HandleTypeDef *)pdev->pClassDataCmsit[pdev->classId];

  if ((pdev->ep_in[epnum & 0xFU].total_length > 0U) &&
      ((pdev->ep_in[epnum & 0xFU].total_length % hpcd->IN_ep[epnum & 0xFU].maxpacket) == 0U))
  {
    /* Update the packet total length */
    pdev->ep_in[epnum & 0xFU].total_length = 0U;

    /* Send ZLP */
    (void)USBD_LL_Transmit(pdev, epnum, NULL, 0U);
  }
  else
  {
    hcdc->TxState = 0U;

    if (((USBD_MIDI_ItfTypeDef *)pdev->pUserData[pdev->classId])->TransmitCplt != NULL)
    {
      ((USBD_MIDI_ItfTypeDef *)pdev->pUserData[pdev->classId])->TransmitCplt(hcdc->TxBuffer, &hcdc->TxLength, epnum);
    }
  }

  return (uint8_t)USBD_OK;
}

static uint8_t USBD_MIDI_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  USBD_MIDI_HandleTypeDef *hcdc = (USBD_MIDI_HandleTypeDef *)pdev->pClassDataCmsit[pdev->classId];

  if (pdev->pClassDataCmsit[pdev->classId] == NULL)
  {
    return (uint8_t)USBD_FAIL;
  }

  /* Get the received data length */
  hcdc->RxLength = USBD_LL_GetRxDataSize(pdev, epnum);

  /* USB data will be immediately processed, this allow next USB traffic being
  NAKed till the end of the application Xfer */

  ((USBD_MIDI_ItfTypeDef *)pdev->pUserData[pdev->classId])->Receive(hcdc->RxBuffer, &hcdc->RxLength);

  return (uint8_t)USBD_OK;
}

static uint8_t USBD_MIDI_EP0_RxReady(USBD_HandleTypeDef *pdev)
{
  USBD_MIDI_HandleTypeDef *hcdc = (USBD_MIDI_HandleTypeDef *)pdev->pClassDataCmsit[pdev->classId];

  if (hcdc == NULL)
  {
    return (uint8_t)USBD_FAIL;
  }

  if ((pdev->pUserData[pdev->classId] != NULL) && (hcdc->CmdOpCode != 0xFFU))
  {
    ((USBD_MIDI_ItfTypeDef *)pdev->pUserData[pdev->classId])->Control(hcdc->CmdOpCode,
                                                                     (uint8_t *)hcdc->data,
                                                                     (uint16_t)hcdc->CmdLength);
    hcdc->CmdOpCode = 0xFFU;
  }

  return (uint8_t)USBD_OK;
}
#ifndef USE_USBD_COMPOSITE

static uint8_t *USBD_MIDI_GetFSCfgDesc(uint16_t *length)
{
  USBD_EpDescTypeDef *pEpCmdDesc = USBD_GetEpDesc(USBD_MIDI_CfgDesc, MIDI_CMD_EP);
  USBD_EpDescTypeDef *pEpOutDesc = USBD_GetEpDesc(USBD_MIDI_CfgDesc, MIDI_OUT_EP);
  USBD_EpDescTypeDef *pEpInDesc = USBD_GetEpDesc(USBD_MIDI_CfgDesc, MIDI_IN_EP);

  if (pEpCmdDesc != NULL)
  {
    pEpCmdDesc->bInterval = MIDI_FS_BINTERVAL;
  }

  if (pEpOutDesc != NULL)
  {
    pEpOutDesc->wMaxPacketSize = MIDI_DATA_FS_MAX_PACKET_SIZE;
  }

  if (pEpInDesc != NULL)
  {
    pEpInDesc->wMaxPacketSize = MIDI_DATA_FS_MAX_PACKET_SIZE;
  }

  *length = (uint16_t)sizeof(USBD_MIDI_CfgDesc);
  return USBD_MIDI_CfgDesc;
}

static uint8_t *USBD_MIDI_GetHSCfgDesc(uint16_t *length)
{
  USBD_EpDescTypeDef *pEpCmdDesc = USBD_GetEpDesc(USBD_MIDI_CfgDesc, MIDI_CMD_EP);
  USBD_EpDescTypeDef *pEpOutDesc = USBD_GetEpDesc(USBD_MIDI_CfgDesc, MIDI_OUT_EP);
  USBD_EpDescTypeDef *pEpInDesc = USBD_GetEpDesc(USBD_MIDI_CfgDesc, MIDI_IN_EP);

  if (pEpCmdDesc != NULL)
  {
    pEpCmdDesc->bInterval = MIDI_HS_BINTERVAL;
  }

  if (pEpOutDesc != NULL)
  {
    pEpOutDesc->wMaxPacketSize = MIDI_DATA_HS_MAX_PACKET_SIZE;
  }

  if (pEpInDesc != NULL)
  {
    pEpInDesc->wMaxPacketSize = MIDI_DATA_HS_MAX_PACKET_SIZE;
  }

  *length = (uint16_t)sizeof(USBD_MIDI_CfgDesc);
  return USBD_MIDI_CfgDesc;
}

static uint8_t *USBD_MIDI_GetOtherSpeedCfgDesc(uint16_t *length)
{
  USBD_EpDescTypeDef *pEpCmdDesc = USBD_GetEpDesc(USBD_MIDI_CfgDesc, MIDI_CMD_EP);
  USBD_EpDescTypeDef *pEpOutDesc = USBD_GetEpDesc(USBD_MIDI_CfgDesc, MIDI_OUT_EP);
  USBD_EpDescTypeDef *pEpInDesc = USBD_GetEpDesc(USBD_MIDI_CfgDesc, MIDI_IN_EP);

  if (pEpCmdDesc != NULL)
  {
    pEpCmdDesc->bInterval = MIDI_FS_BINTERVAL;
  }

  if (pEpOutDesc != NULL)
  {
    pEpOutDesc->wMaxPacketSize = MIDI_DATA_FS_MAX_PACKET_SIZE;
  }

  if (pEpInDesc != NULL)
  {
    pEpInDesc->wMaxPacketSize = MIDI_DATA_FS_MAX_PACKET_SIZE;
  }

  *length = (uint16_t)sizeof(USBD_MIDI_CfgDesc);
  return USBD_MIDI_CfgDesc;
}

uint8_t *USBD_MIDI_GetDeviceQualifierDescriptor(uint16_t *length)
{
  *length = (uint16_t)sizeof(USBD_MIDI_DeviceQualifierDesc);

  return USBD_MIDI_DeviceQualifierDesc;
}
#endif /* USE_USBD_COMPOSITE  */

uint8_t USBD_MIDI_RegisterInterface(USBD_HandleTypeDef *pdev,
                                   USBD_MIDI_ItfTypeDef *fops)
{
  if (fops == NULL)
  {
    return (uint8_t)USBD_FAIL;
  }

  pdev->pUserData[pdev->classId] = fops;

  return (uint8_t)USBD_OK;
}

#ifdef USE_USBD_COMPOSITE
uint8_t USBD_MIDI_SetTxBuffer(USBD_HandleTypeDef *pdev,
                             uint8_t *pbuff, uint32_t length, uint8_t ClassId)
{
  USBD_MIDI_HandleTypeDef *hcdc = (USBD_MIDI_HandleTypeDef *)pdev->pClassDataCmsit[ClassId];
#else
uint8_t USBD_MIDI_SetTxBuffer(USBD_HandleTypeDef *pdev,
                             uint8_t *pbuff, uint32_t length)
{
  USBD_MIDI_HandleTypeDef *hcdc = (USBD_MIDI_HandleTypeDef *)pdev->pClassDataCmsit[pdev->classId];
#endif /* USE_USBD_COMPOSITE */

  if (hcdc == NULL)
  {
    return (uint8_t)USBD_FAIL;
  }

  hcdc->TxBuffer = pbuff;
  hcdc->TxLength = length;

  return (uint8_t)USBD_OK;
}

uint8_t USBD_MIDI_SetRxBuffer(USBD_HandleTypeDef *pdev, uint8_t *pbuff)
{
  USBD_MIDI_HandleTypeDef *hcdc = (USBD_MIDI_HandleTypeDef *)pdev->pClassDataCmsit[pdev->classId];

  if (hcdc == NULL)
  {
    return (uint8_t)USBD_FAIL;
  }

  hcdc->RxBuffer = pbuff;

  return (uint8_t)USBD_OK;
}

#ifdef USE_USBD_COMPOSITE
uint8_t USBD_MIDI_TransmitPacket(USBD_HandleTypeDef *pdev, uint8_t ClassId)
{
  USBD_MIDI_HandleTypeDef *hcdc = (USBD_MIDI_HandleTypeDef *)pdev->pClassDataCmsit[ClassId];
#else
uint8_t USBD_MIDI_TransmitPacket(USBD_HandleTypeDef *pdev)
{
  USBD_MIDI_HandleTypeDef *hcdc = (USBD_MIDI_HandleTypeDef *)pdev->pClassDataCmsit[pdev->classId];
#endif  /* USE_USBD_COMPOSITE */

  USBD_StatusTypeDef ret = USBD_BUSY;

#ifdef USE_USBD_COMPOSITE
  /* Get the Endpoints addresses allocated for this class instance */
  MIDIInEpAdd  = USBD_CoreGetEPAdd(pdev, USBD_EP_IN, USBD_EP_TYPE_BULK, ClassId);
#endif  /* USE_USBD_COMPOSITE */

  if (hcdc == NULL)
  {
    return (uint8_t)USBD_FAIL;
  }

  if (hcdc->TxState == 0U)
  {
    /* Tx Transfer in progress */
    hcdc->TxState = 1U;

    /* Update the packet total length */
    pdev->ep_in[MIDIInEpAdd & 0xFU].total_length = hcdc->TxLength;

    /* Transmit next packet */
    (void)USBD_LL_Transmit(pdev, MIDIInEpAdd, hcdc->TxBuffer, hcdc->TxLength);

    ret = USBD_OK;
  }

  return (uint8_t)ret;
}

uint8_t USBD_MIDI_ReceivePacket(USBD_HandleTypeDef *pdev)
{
  USBD_MIDI_HandleTypeDef *hcdc = (USBD_MIDI_HandleTypeDef *)pdev->pClassDataCmsit[pdev->classId];

#ifdef USE_USBD_COMPOSITE
  /* Get the Endpoints addresses allocated for this class instance */
  MIDIOutEpAdd = USBD_CoreGetEPAdd(pdev, USBD_EP_OUT, USBD_EP_TYPE_BULK, (uint8_t)pdev->classId);
#endif /* USE_USBD_COMPOSITE */

  if (pdev->pClassDataCmsit[pdev->classId] == NULL)
  {
    return (uint8_t)USBD_FAIL;
  }

  if (pdev->dev_speed == USBD_SPEED_HIGH)
  {
    /* Prepare Out endpoint to receive next packet */
    (void)USBD_LL_PrepareReceive(pdev, MIDIOutEpAdd, hcdc->RxBuffer,
                                 MIDI_DATA_HS_OUT_PACKET_SIZE);
  }
  else
  {
    /* Prepare Out endpoint to receive next packet */
    (void)USBD_LL_PrepareReceive(pdev, MIDIOutEpAdd, hcdc->RxBuffer,
                                 MIDI_DATA_FS_OUT_PACKET_SIZE);
  }

  return (uint8_t)USBD_OK;
}
#endif // #ifdef	USB_MIDI

#endif // #ifdef	STM32H743xx

