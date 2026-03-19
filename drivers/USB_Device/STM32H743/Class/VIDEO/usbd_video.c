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
 * usbd_video.c
 *
 *  Created on: Mar 2, 2026
 *      Author: fil
 */
#include "main.h"
#ifdef	STM32H743xx
#include "../../../../../kernel/A.h"
#include "../../../../../kernel/A_exported_functions.h"
#ifdef	USB_DEVICE_ENABLED
#include "usbd_video.h"
#include "../../Core/usbd_ctlreq.h"
#include "../../Core/usbd_core.h"
#include "../../Core/usbd_def.h"
#include "../../App/usbd_video_if.h"

/* VIDEO Device library callbacks */
static uint8_t USBD_VIDEO_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t USBD_VIDEO_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t USBD_VIDEO_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
static uint8_t *USBD_VIDEO_GetFSCfgDesc(uint16_t *length);
static uint8_t *USBD_VIDEO_GetHSCfgDesc(uint16_t *length);
static uint8_t *USBD_VIDEO_GetOtherSpeedCfgDesc(uint16_t *length);
static uint8_t *USBD_VIDEO_GetDeviceQualifierDesc(uint16_t *length);
static uint8_t USBD_VIDEO_EP0_RxReady(USBD_HandleTypeDef *pdev);
static uint8_t USBD_VIDEO_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t USBD_VIDEO_SOF(USBD_HandleTypeDef *pdev);
static uint8_t USBD_VIDEO_IsoINIncomplete(USBD_HandleTypeDef *pdev, uint8_t epnum);

/* VIDEO Requests management functions */
static void VIDEO_REQ_GetCurrent(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
static void VIDEO_REQ_SetCurrent(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);

USBD_ClassTypeDef  USBD_VIDEO =
{
    USBD_VIDEO_Init,
    USBD_VIDEO_DeInit,
    USBD_VIDEO_Setup,
    NULL,
    USBD_VIDEO_EP0_RxReady,
    USBD_VIDEO_DataIn,
    NULL,
    USBD_VIDEO_SOF,
    USBD_VIDEO_IsoINIncomplete,
    NULL,
    USBD_VIDEO_GetHSCfgDesc,
    USBD_VIDEO_GetFSCfgDesc,
    USBD_VIDEO_GetOtherSpeedCfgDesc,
    USBD_VIDEO_GetDeviceQualifierDesc,
};

/* USB VIDEO device Configuration Descriptor */
__ALIGN_BEGIN static uint8_t USBD_VIDEO_CfgHSDesc[] __ALIGN_END =
{
    /* Configuration 1 */
    USB_CONF_DESC_SIZE,                            /* bLength: Configuration Descriptor size */
    USB_DESC_TYPE_CONFIGURATION,                   /* bDescriptorType: Configuration */
    LOBYTE(UVC_CONFIG_DESC_SIZ),                   /* wTotalLength: no of returned bytes */
    HIBYTE(UVC_CONFIG_DESC_SIZ),
    0x02,                                          /* bNumInterfaces: 2 interface */
    0x01,                                          /* bConfigurationValue: Configuration value */
    0x00,                                          /* iConfiguration: Index of string descriptor describing the configuration */
#if (USBD_SELF_POWERED == 1U)
    0xC0,                                          /* bmAttributes: Bus Powered according to user configuration */
#else
    0x80,                                          /* bmAttributes: Bus Powered according to user configuration */
#endif
    USBD_MAX_POWER,                                /* bMaxPower in mA according to user configuration */

    /* Interface Association Descriptor */
    USB_IAD_DESC_SIZE,                             /* bLength: Interface Association Descriptor size */
    USB_DESC_TYPE_IAD,                             /* bDescriptorType: interface association */
    0x00,                                          /* bFirstInterface */
    0x02,                                          /* bInterfaceCount */
    UVC_CC_VIDEO,                                  /* bFunctionClass: Video class */
    SC_VIDEO_INTERFACE_COLLECTION,                 /* bFunctionSubClass: Video Interface Collection */
    PC_PROTOCOL_UNDEFINED,                         /* bInterfaceProtocol: protocol undefined */
    0x00,                                          /* iFunction */

    /* Standard VC (Video Control) Interface Descriptor  = interface 0 */
    USB_IF_DESC_SIZE,                              /* bLength: interface descriptor size */
    USB_DESC_TYPE_INTERFACE,                       /* bDescriptorType: interface */
    UVC_VC_IF_NUM,                                 /* bInterfaceNumber: interface number */
    0x00,                                          /* bAlternateSetting: index of this alternate setting */
    0x00,                                          /* bNumEndpoints: No endpoints used for this interface */
    UVC_CC_VIDEO,                                  /* bInterfaceClass: Video Class */
    SC_VIDEOCONTROL,                               /* bInterfaceSubClass: Video Control */
    PC_PROTOCOL_UNDEFINED,                         /* bInterfaceProtocol: protocol is undefined */
    0x00,                                          /* iFunction: index of string descriptor relative to this item */

    /* Class-specific VC Interface Descriptor */
    VIDEO_VC_IF_HEADER_DESC_SIZE,                  /* bLength */
    CS_INTERFACE,                                  /* bDescriptorType */
    VC_HEADER,                                     /* bDescriptorSubtype */
    LOBYTE(UVC_VERSION),
    HIBYTE(UVC_VERSION),                           /* bcdUVC: UVC1.0 or UVC1.1 revision */
    VC_FRAME_DESC_SIZE,                            /* wTotalLength: total size of class-specific descriptors */
    0x00,
    0x00,                                          /* dwClockFrequency: not used. 48 Mhz value is set, but not used */
    0x6C,
    0xDC,
    0x02,
    0x01,                                          /* bInCollection: number of streaming interfaces */
    0x01,                                          /* baInterfaceNr(1): VideoStreaming interface 1 is part of VC interface */

    /* Input Terminal Descriptor */
    VIDEO_IN_TERMINAL_DESC_SIZE,                   /* bLength: Input terminal descriptor size */
    CS_INTERFACE,                                  /* bDescriptorType: INTERFACE */
    VC_INPUT_TERMINAL,                             /* bDescriptorSubtype: INPUT_TERMINAL */
    0x01,                                          /* bTerminalID: ID of this Terminal */
    LOBYTE(ITT_CAMERA),                            /* wTerminalType: 0x0200 ITT_CAMERA */
    HIBYTE(ITT_CAMERA),
    0x00,                                          /* bAssocTerminal: no Terminal is associated */
    0x00,                                          /* iTerminal: index of string descriptor relative to this item */
    WBVAL(0x0000),                                 /* wObjectiveFocalLengthMin: not supported */
    WBVAL(0x0000),                                 /* wObjectiveFocalLengthMax: not supported */
    WBVAL(0x0000),                                 /* wOcularFocalLength: not supported */
    0x03,                                          /* bControlSize */
    0x00,                                          /* bmControls */
    0x00,
    0x00,

    /* Output Terminal Descriptor */
    VIDEO_OUT_TERMINAL_DESC_SIZE,                  /* bLength: output terminal descriptor size */
    CS_INTERFACE,                                  /* bDescriptorType */
    VC_OUTPUT_TERMINAL,                            /* bDescriptorSubtype */
    0x02,                                          /* bTerminalID */
    LOBYTE(TT_STREAMING),                          /* wTerminalType: USB streaming terminal */
    HIBYTE(TT_STREAMING),
    0x00,                                          /* bAssocTerminal: no Terminal is associated */
    0x01,                                          /* bSourceID: input is connected to output unit ID 1 */
    0x00,                                          /* iTerminal: index of string descriptor relative to this item */

    /* Standard VS (Video Streaming) Interface Descriptor = interface 1, alternate setting 0 = Zero Bandwidth */
    USB_IF_DESC_SIZE,                              /* bLength: interface descriptor size */
    USB_DESC_TYPE_INTERFACE,                       /* bDescriptorType */
    UVC_VS_IF_NUM,                                 /* bInterfaceNumber */
    0x00,                                          /* bAlternateSetting */
    0x00,                                          /* bNumEndpoints: no endpoints used for alternate setting 0 */
    UVC_CC_VIDEO,                                  /* bInterfaceClass */
    SC_VIDEOSTREAMING,                             /* bInterfaceSubClass */
    PC_PROTOCOL_UNDEFINED,                         /* bInterfaceProtocol */
    0x00,                                          /* iInterface: index of string descriptor relative to this item */

    /* Class-specific VS Header Descriptor (Input) */
    VIDEO_VS_IF_IN_HEADER_DESC_SIZE,               /* bLength */
    CS_INTERFACE,                                  /* bDescriptorType */
    VS_INPUT_HEADER,                               /* bDescriptorSubtype */
    0x02,                                          /* bNumFormats: 2 video formats payload (MJPEG and uncompressed) */
    WBVAL(VS_HEADER_SIZE),                         /* wTotalLength: Total size of Video Streaming Specific Descriptors */
    UVC_IN_EP,                                     /* bEndPointAddress: In endpoint is used for the alternate setting */
    0x00,                                          /* bmInfo: dynamic format change not supported */
    0x02,                                          /* bTerminalLink: output to terminal ID 2 */
    0x00,                                          /* bStillCaptureMethod: not supported */
    0x00,                                          /* bTriggerSupport: not supported */
    0x00,                                          /* bTriggerUsage: not supported */
    0x01,                                          /* bControlSize: 1 byte field size */
    0x00,                                          /* bmaControls: Format 1 - no specific controls used */
    0x00,                                          /* bmaControls: Format 2 - no specific controls used */

    /* Payload Format Descriptor - MJPEG */
    VS_FORMAT_MJPEG_DESC_SIZE,                     /* blength */
    CS_INTERFACE,                                  /* bDescriptorType */
    VS_FORMAT_MJPEG,                               /* bDescriptorSubType */
    UVC_FORMAT_INDEX_MJPEG,                        /* bFormatIndex */
    0x02,                                          /* bNumFrameDescriptor: 2 frames (640x480 and 320x240) */
    0x01,                                          /* bmFlags: FixedSizeSamples */
    0x01,                                          /* bDefaultFrameIndex: default frame used is frame 1 */
    0x00,                                          /* bAspectRatioX: not required by specification */
    0x00,                                          /* bAspectRatioY: not required by specification */
    0x00,                                          /* bInterlaceFlags: non interlaced stream */
    0x00,                                          /* bCopyProtect: no protection restrictions */

    /* Class-specific VS Frame Descriptor - MJPEG VGA */
    VS_FRAME_DESC_SIZE,                            /* bLength */
    CS_INTERFACE,                                  /* bDescriptorType */
    VS_FRAME_MJPEG,                                /* bDescriptorSubType */
    UVC_FRAME_INDEX_VGA,                           /* bFrameIndex */
    0x02,                                          /* bmCapabilities: fixed frame rate supported */
    WBVAL(640),                                    /* wWidth: Image Frame Width */
    WBVAL(480),                                    /* wHeight: Image Frame Height */
    DBVAL(UVC_MIN_BIT_RATE(UVC_FPS_HS_MJPEG_VGA)), /* dwMinBitRate */
    DBVAL(UVC_MAX_BIT_RATE(UVC_FPS_HS_MJPEG_VGA)), /* dwMaxBitRate */
    DBVAL(640 * 480 * 16 / 8),                     /* dwMaxVideoFrameBufSize */
    DBVAL(UVC_INTERVAL(UVC_FPS_HS_MJPEG_VGA)),     /* dwDefaultFrameInterval */
    0x01,                                          /* bFrameIntervalType: Discrete */
    DBVAL(UVC_INTERVAL(UVC_FPS_HS_MJPEG_VGA)),     /* dwMinFrameInterval */

    /* Class-specific VS Frame Descriptor - MJPEG QVGA */
    VS_FRAME_DESC_SIZE,                            /* bLength */
    CS_INTERFACE,                                  /* bDescriptorType */
    VS_FRAME_MJPEG,                                /* bDescriptorSubType */
    UVC_FRAME_INDEX_QVGA,                          /* bFrameIndex */
    0x02,                                          /* bmCapabilities: fixed frame rate supported */
    WBVAL(320),                                    /* wWidth */
    WBVAL(240),                                    /* wHeight */
    DBVAL(UVC_MIN_BIT_RATE(UVC_FPS_HS_MJPEG_QVGA)),/* dwMinBitRate */
    DBVAL(UVC_MAX_BIT_RATE(UVC_FPS_HS_MJPEG_QVGA)),/* dwMaxBitRate */
    DBVAL(320 * 240 * 16 / 8),                     /* dwMaxVideoFrameBufSize */
    DBVAL(UVC_INTERVAL(UVC_FPS_HS_MJPEG_QVGA)),    /* dwDefaultFrameInterval */
    0x01,                                          /* bFrameIntervalType: Discrete */
    DBVAL(UVC_INTERVAL(UVC_FPS_HS_MJPEG_QVGA)),    /* dwMinFrameInterval */

    /* Color Matching Descriptor - MJPEG */
    VS_COLOR_MATCHING_DESC_SIZE,                   /* bLength */
    CS_INTERFACE,                                  /* bDescriptorType: CS_INTERFACE */
    VS_COLORFORMAT,                                /* bDescriptorSubType: VS_COLORFORMAT */
    UVC_COLOR_PRIMARIE,                            /* bColorPrimarie */
    UVC_TFR_CHARACTERISTICS,                       /* bTransferCharacteristics */
    UVC_MATRIX_COEFFICIENTS,                       /* bMatrixCoefficients */

    /* Payload Format Descriptor - YUY2 */
    VS_FORMAT_UNCOMPRESSED_DESC_SIZE,              /* blength */
    CS_INTERFACE,                                  /* bDescriptorType */
    VS_FORMAT_UNCOMPRESSED,                        /* bDescriptorSubType */
    UVC_FORMAT_INDEX_YUY2,                         /* bFormatIndex */
    0x02,                                          /* bNumFrameDescriptor */
    DBVAL(UVC_GUID_YUY2),                          /* GUID Format: YUY2 */
    0x00, 0x00,
    0x10, 0x00,
    0x80, 0x00,
    0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71,
    16U,                                           /* bBitsPerPixel */
    0x01,                                          /* bDefaultFrameIndex */
    0x00,                                          /* bAspectRatioX */
    0x00,                                          /* bAspectRatioY */
    0x00,                                          /* bInterlaceFlags */
    0x00,                                          /* bCopyProtect */

    /* Class-specific VS Frame Descriptor - YUY2 VGA */
    VS_FRAME_DESC_SIZE,                            /* bLength */
    CS_INTERFACE,                                  /* bDescriptorType */
    VS_FRAME_UNCOMPRESSED,                         /* bDescriptorSubType */
    UVC_FRAME_INDEX_VGA,                           /* bFrameIndex */
    0x02,                                          /* bmCapabilities */
    WBVAL(640),                                    /* wWidth */
    WBVAL(480),                                    /* wHeight */
    DBVAL(UVC_MIN_BIT_RATE(UVC_FPS_HS_YUY2_VGA)),  /* dwMinBitRate */
    DBVAL(UVC_MAX_BIT_RATE(UVC_FPS_HS_YUY2_VGA)),  /* dwMaxBitRate */
    DBVAL(640 * 480 * 16 / 8),                     /* dwMaxVideoFrameBufSize */
    DBVAL(UVC_INTERVAL(UVC_FPS_HS_YUY2_VGA)),      /* dwDefaultFrameInterval */
    0x01,                                          /* bFrameIntervalType */
    DBVAL(UVC_INTERVAL(UVC_FPS_HS_YUY2_VGA)),      /* dwMinFrameInterval */

    /* Class-specific VS Frame Descriptor - YUY2 QVGA */
    VS_FRAME_DESC_SIZE,                            /* bLength */
    CS_INTERFACE,                                  /* bDescriptorType */
    VS_FRAME_UNCOMPRESSED,                         /* bDescriptorSubType */
    UVC_FRAME_INDEX_QVGA,                          /* bFrameIndex */
    0x02,                                          /* bmCapabilities */
    WBVAL(320),                                    /* wWidth */
    WBVAL(240),                                    /* wHeight */
    DBVAL(UVC_MIN_BIT_RATE(UVC_FPS_HS_YUY2_QVGA)), /* dwMinBitRate */
    DBVAL(UVC_MAX_BIT_RATE(UVC_FPS_HS_YUY2_QVGA)), /* dwMaxBitRate */
    DBVAL(320 * 240 * 16 / 8),                     /* dwMaxVideoFrameBufSize */
    DBVAL(UVC_INTERVAL(UVC_FPS_HS_YUY2_QVGA)),     /* dwDefaultFrameInterval */
    0x01,                                          /* bFrameIntervalType */
    DBVAL(UVC_INTERVAL(UVC_FPS_HS_YUY2_QVGA)),     /* dwMinFrameInterval */

    /* Color Matching Descriptor - YUY2 */
    VS_COLOR_MATCHING_DESC_SIZE,                   /* bLength */
    CS_INTERFACE,                                  /* bDescriptorType: CS_INTERFACE */
    VS_COLORFORMAT,                                /* bDescriptorSubType: VS_COLORFORMAT */
    UVC_COLOR_PRIMARIE,                            /* bColorPrimarie */
    UVC_TFR_CHARACTERISTICS,                       /* bTransferCharacteristics */
    UVC_MATRIX_COEFFICIENTS,                       /* bMatrixCoefficients */

    /* Standard VS Interface Descriptor - alternate setting 1 = data transfer mode */
    USB_IF_DESC_SIZE,                              /* bLength */
    USB_DESC_TYPE_INTERFACE,                       /* bDescriptorType */
    UVC_VS_IF_NUM,                                 /* bInterfaceNumber */
    0x01,                                          /* bAlternateSetting */
    0x01,                                          /* bNumEndpoints: one endpoint is used */
    UVC_CC_VIDEO,                                  /* bInterfaceClass */
    SC_VIDEOSTREAMING,                             /* bInterfaceSubClass */
    PC_PROTOCOL_UNDEFINED,                         /* bInterfaceProtocol */
    0x00,                                          /* iInterface */

    /* Standard VS data Endpoint */
    USB_EP_DESC_SIZE,                              /* bLength */
    USB_DESC_TYPE_ENDPOINT,                        /* bDescriptorType */
    UVC_IN_EP,                                     /* bEndpointAddress */
    0x05,                                          /* bmAttributes: ISO transfer */
    LOBYTE(UVC_ISO_HS_MPS),                        /* wMaxPacketSize */
    HIBYTE(UVC_ISO_HS_MPS),
    0x01,                                          /* bInterval */
};

__ALIGN_BEGIN static uint8_t USBD_VIDEO_CfgFSDesc[] __ALIGN_END =
{
    /* Configuration 1 */
    USB_CONF_DESC_SIZE,                            /* bLength */
    USB_DESC_TYPE_CONFIGURATION,                   /* bDescriptorType */
    LOBYTE(UVC_CONFIG_DESC_SIZ),                   /* wTotalLength */
    HIBYTE(UVC_CONFIG_DESC_SIZ),
    0x02,                                          /* bNumInterfaces */
    0x01,                                          /* bConfigurationValue */
    0x00,                                          /* iConfiguration */
#if (USBD_SELF_POWERED == 1U)
    0xC0,                                          /* bmAttributes */
#else
    0x80,                                          /* bmAttributes */
#endif
    USBD_MAX_POWER,                                /* bMaxPower */

    /* Interface Association Descriptor */
    USB_IAD_DESC_SIZE,                             /* bLength */
    USB_DESC_TYPE_IAD,                             /* bDescriptorType */
    0x00,                                          /* bFirstInterface */
    0x02,                                          /* bInterfaceCount */
    UVC_CC_VIDEO,                                  /* bFunctionClass */
    SC_VIDEO_INTERFACE_COLLECTION,                 /* bFunctionSubClass */
    PC_PROTOCOL_UNDEFINED,                         /* bInterfaceProtocol */
    0x00,                                          /* iFunction */

    /* Standard VC Interface Descriptor */
    USB_IF_DESC_SIZE,                              /* bLength */
    USB_DESC_TYPE_INTERFACE,                       /* bDescriptorType */
    UVC_VC_IF_NUM,                                 /* bInterfaceNumber */
    0x00,                                          /* bAlternateSetting */
    0x00,                                          /* bNumEndpoints */
    UVC_CC_VIDEO,                                  /* bInterfaceClass */
    SC_VIDEOCONTROL,                               /* bInterfaceSubClass */
    PC_PROTOCOL_UNDEFINED,                         /* bInterfaceProtocol */
    0x00,                                          /* iFunction */

    /* Class-specific VC Interface Descriptor */
    VIDEO_VC_IF_HEADER_DESC_SIZE,                  /* bLength */
    CS_INTERFACE,                                  /* bDescriptorType */
    VC_HEADER,                                     /* bDescriptorSubtype */
    LOBYTE(UVC_VERSION),
    HIBYTE(UVC_VERSION),
    VC_FRAME_DESC_SIZE,                            /* wTotalLength */
    0x00,
    0x00,                                          /* dwClockFrequency */
    0x6C,
    0xDC,
    0x02,
    0x01,                                          /* bInCollection */
    0x01,                                          /* baInterfaceNr(1) */

    /* Input Terminal Descriptor */
    VIDEO_IN_TERMINAL_DESC_SIZE,                   /* bLength */
    CS_INTERFACE,                                  /* bDescriptorType */
    VC_INPUT_TERMINAL,                             /* bDescriptorSubtype */
    0x01,                                          /* bTerminalID */
    LOBYTE(ITT_CAMERA),
    HIBYTE(ITT_CAMERA),
    0x00,                                          /* bAssocTerminal */
    0x00,                                          /* iTerminal */
    WBVAL(0x0000),
    WBVAL(0x0000),
    WBVAL(0x0000),
    0x03,                                          /* bControlSize */
    0x00,
    0x00,
    0x00,

    /* Output Terminal Descriptor */
    VIDEO_OUT_TERMINAL_DESC_SIZE,                  /* bLength */
    CS_INTERFACE,                                  /* bDescriptorType */
    VC_OUTPUT_TERMINAL,                            /* bDescriptorSubtype */
    0x02,                                          /* bTerminalID */
    LOBYTE(TT_STREAMING),
    HIBYTE(TT_STREAMING),
    0x00,                                          /* bAssocTerminal */
    0x01,                                          /* bSourceID */
    0x00,                                          /* iTerminal */

    /* Standard VS Interface - alternate setting 0 */
    USB_IF_DESC_SIZE,                              /* bLength */
    USB_DESC_TYPE_INTERFACE,                       /* bDescriptorType */
    UVC_VS_IF_NUM,                                 /* bInterfaceNumber */
    0x00,                                          /* bAlternateSetting */
    0x00,                                          /* bNumEndpoints */
    UVC_CC_VIDEO,                                  /* bInterfaceClass */
    SC_VIDEOSTREAMING,                             /* bInterfaceSubClass */
    PC_PROTOCOL_UNDEFINED,                         /* bInterfaceProtocol */
    0x00,                                          /* iInterface */

    /* Class-specific VS Header Descriptor */
    VIDEO_VS_IF_IN_HEADER_DESC_SIZE,               /* bLength */
    CS_INTERFACE,                                  /* bDescriptorType */
    VS_INPUT_HEADER,                               /* bDescriptorSubtype */
    0x02,                                          /* bNumFormats */
    WBVAL(VS_HEADER_SIZE),                         /* wTotalLength */
    UVC_IN_EP,                                     /* bEndPointAddress */
    0x00,                                          /* bmInfo */
    0x02,                                          /* bTerminalLink */
    0x00,                                          /* bStillCaptureMethod */
    0x00,                                          /* bTriggerSupport */
    0x00,                                          /* bTriggerUsage */
    0x01,                                          /* bControlSize */
    0x00,                                          /* bmaControls */
    0x00,                                          /* bmaControls */

    /* Payload Format Descriptor - MJPEG */
    VS_FORMAT_MJPEG_DESC_SIZE,                     /* blength */
    CS_INTERFACE,                                  /* bDescriptorType */
    VS_FORMAT_MJPEG,                               /* bDescriptorSubType */
    UVC_FORMAT_INDEX_MJPEG,                        /* bFormatIndex */
    0x02,                                          /* bNumFrameDescriptor */
    0x01,                                          /* bmFlags */
    0x01,                                          /* bDefaultFrameIndex */
    0x00,                                          /* bAspectRatioX */
    0x00,                                          /* bAspectRatioY */
    0x00,                                          /* bInterlaceFlags */
    0x00,                                          /* bCopyProtect */

    /* Class-specific VS Frame Descriptor - MJPEG VGA */
    VS_FRAME_DESC_SIZE,                            /* bLength */
    CS_INTERFACE,                                  /* bDescriptorType */
    VS_FRAME_MJPEG,                                /* bDescriptorSubType */
    UVC_FRAME_INDEX_VGA,                           /* bFrameIndex */
    0x02,                                          /* bmCapabilities */
    WBVAL(640),
    WBVAL(480),
    DBVAL(UVC_MIN_BIT_RATE(UVC_FPS_FS_MJPEG_VGA)),
    DBVAL(UVC_MAX_BIT_RATE(UVC_FPS_FS_MJPEG_VGA)),
    DBVAL(640 * 480 * 16 / 8),
    DBVAL(UVC_INTERVAL(UVC_FPS_FS_MJPEG_VGA)),
    0x01,
    DBVAL(UVC_INTERVAL(UVC_FPS_FS_MJPEG_VGA)),

    /* Class-specific VS Frame Descriptor - MJPEG QVGA */
    VS_FRAME_DESC_SIZE,                            /* bLength */
    CS_INTERFACE,                                  /* bDescriptorType */
    VS_FRAME_MJPEG,                                /* bDescriptorSubType */
    UVC_FRAME_INDEX_QVGA,                          /* bFrameIndex */
    0x02,                                          /* bmCapabilities */
    WBVAL(320),
    WBVAL(240),
    DBVAL(UVC_MIN_BIT_RATE(UVC_FPS_FS_MJPEG_QVGA)),
    DBVAL(UVC_MAX_BIT_RATE(UVC_FPS_FS_MJPEG_QVGA)),
    DBVAL(320 * 240 * 16 / 8),
    DBVAL(UVC_INTERVAL(UVC_FPS_FS_MJPEG_QVGA)),
    0x01,
    DBVAL(UVC_INTERVAL(UVC_FPS_FS_MJPEG_QVGA)),

    /* Color Matching Descriptor - MJPEG */
    VS_COLOR_MATCHING_DESC_SIZE,
    CS_INTERFACE,
    VS_COLORFORMAT,
    UVC_COLOR_PRIMARIE,
    UVC_TFR_CHARACTERISTICS,
    UVC_MATRIX_COEFFICIENTS,

    /* Payload Format Descriptor - YUY2 */
    VS_FORMAT_UNCOMPRESSED_DESC_SIZE,
    CS_INTERFACE,
    VS_FORMAT_UNCOMPRESSED,
    UVC_FORMAT_INDEX_YUY2,
    0x02,
    DBVAL(UVC_GUID_YUY2),
    0x00, 0x00,
    0x10, 0x00,
    0x80, 0x00,
    0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71,
    16U,
    0x01,
    0x00,
    0x00,
    0x00,
    0x00,

    /* Class-specific VS Frame Descriptor - YUY2 VGA */
    VS_FRAME_DESC_SIZE,
    CS_INTERFACE,
    VS_FRAME_UNCOMPRESSED,
    UVC_FRAME_INDEX_VGA,
    0x02,
    WBVAL(640),
    WBVAL(480),
    DBVAL(UVC_MIN_BIT_RATE(UVC_FPS_FS_YUY2_VGA)),
    DBVAL(UVC_MAX_BIT_RATE(UVC_FPS_FS_YUY2_VGA)),
    DBVAL(640 * 480 * 16 / 8),
    DBVAL(UVC_INTERVAL(UVC_FPS_FS_YUY2_VGA)),
    0x01,
    DBVAL(UVC_INTERVAL(UVC_FPS_FS_YUY2_VGA)),

    /* Class-specific VS Frame Descriptor - YUY2 QVGA */
    VS_FRAME_DESC_SIZE,
    CS_INTERFACE,
    VS_FRAME_UNCOMPRESSED,
    UVC_FRAME_INDEX_QVGA,
    0x02,
    WBVAL(320),
    WBVAL(240),
    DBVAL(UVC_MIN_BIT_RATE(UVC_FPS_FS_YUY2_QVGA)),
    DBVAL(UVC_MAX_BIT_RATE(UVC_FPS_FS_YUY2_QVGA)),
    DBVAL(320 * 240 * 16 / 8),
    DBVAL(UVC_INTERVAL(UVC_FPS_FS_YUY2_QVGA)),
    0x01,
    DBVAL(UVC_INTERVAL(UVC_FPS_FS_YUY2_QVGA)),

    /* Color Matching Descriptor - YUY2 */
    VS_COLOR_MATCHING_DESC_SIZE,
    CS_INTERFACE,
    VS_COLORFORMAT,
    UVC_COLOR_PRIMARIE,
    UVC_TFR_CHARACTERISTICS,
    UVC_MATRIX_COEFFICIENTS,

    /* Standard VS Interface - alternate setting 1 */
    USB_IF_DESC_SIZE,
    USB_DESC_TYPE_INTERFACE,
    UVC_VS_IF_NUM,
    0x01,
    0x01,
    UVC_CC_VIDEO,
    SC_VIDEOSTREAMING,
    PC_PROTOCOL_UNDEFINED,
    0x00,

    /* Standard VS data Endpoint */
    USB_EP_DESC_SIZE,
    USB_DESC_TYPE_ENDPOINT,
    UVC_IN_EP,
    0x05,
    LOBYTE(UVC_ISO_FS_MPS),
    HIBYTE(UVC_ISO_FS_MPS),
    0x01,
};

/* USB Standard Device Descriptor */
__ALIGN_BEGIN static uint8_t USBD_VIDEO_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] __ALIGN_END =
{
    USB_LEN_DEV_QUALIFIER_DESC,
    USB_DESC_TYPE_DEVICE_QUALIFIER,
    0x00,
    0x02,
    0xEF,
    0x02,
    0x01,
    0x40,
    0x01,
    0x00,
};

/* Video Commit data structure - FIXED WITH VALID VALUES */
static USBD_VideoControlTypeDef video_Commit_Control =
{
    .bmHint = 0x0001U,
    .bFormatIndex = UVC_FORMAT_INDEX_YUY2,      /* ★★★ Changed from MJPEG ★★★ */
    .bFrameIndex = UVC_FRAME_INDEX_QVGA,        /* 320x240 */
    .dwFrameInterval = 0x00051615U,             /* ★★★ 30fps = 333,333 × 100ns ★★★ */
    .wKeyFrameRate = 0x0000U,
    .wPFrameRate = 0x0000U,
    .wCompQuality = 0x0000U,
    .wCompWindowSize = 0x0000U,
    .wDelay = 0x0000U,
    .dwMaxVideoFrameSize = UVC_MAX_FRAME_SIZE_QVGA,  /* ★★★ 153,600 bytes ★★★ */
    .dwMaxPayloadTransferSize = UVC_ISO_FS_MPS,      /* ★★★ 512 bytes ★★★ */
    .dwClockFrequency = 0x02DC6C00U,
    .bmFramingInfo = 0x00U,
    .bPreferedVersion = 0x00U,
    .bMinVersion = 0x00U,
    .bMaxVersion = 0x00U,
};

/* Video Probe data structure - FIXED WITH VALID VALUES */
static USBD_VideoControlTypeDef video_Probe_Control =
{
    .bmHint = 0x0001U,
    .bFormatIndex = UVC_FORMAT_INDEX_YUY2,      /* ★★★ Changed from MJPEG ★★★ */
    .bFrameIndex = UVC_FRAME_INDEX_QVGA,        /* 320x240 */
    .dwFrameInterval = 0x00051615U,             /* ★★★ 30fps = 333,333 × 100ns ★★★ */
    .wKeyFrameRate = 0x0000U,
    .wPFrameRate = 0x0000U,
    .wCompQuality = 0x0000U,
    .wCompWindowSize = 0x0000U,
    .wDelay = 0x0000U,
    .dwMaxVideoFrameSize = UVC_MAX_FRAME_SIZE_QVGA,  /* ★★★ 153,600 bytes ★★★ */
    .dwMaxPayloadTransferSize = UVC_ISO_FS_MPS,      /* ★★★ 512 bytes ★★★ */
    .dwClockFrequency = 0x02DC6C00U,
    .bmFramingInfo = 0x00U,
    .bPreferedVersion = 0x00U,
    .bMinVersion = 0x00U,
    .bMaxVersion = 0x00U,
};

/**
 * @}
 */

/** @defgroup USBD_VIDEO_Private_Functions
 * @{
 */

/**
 * @brief  USBD_VIDEO_Init
 */
static uint8_t USBD_VIDEO_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
    USBD_VIDEO_HandleTypeDef *hVIDEO;

    hVIDEO = USBD_malloc(sizeof(USBD_VIDEO_HandleTypeDef));
    if (hVIDEO == NULL)
    {
        return (uint8_t)USBD_FAIL;
    }

    pdev->pClassData = (void *)hVIDEO;

    if (pdev->dev_speed == USBD_SPEED_HIGH)
    {
        (void)USBD_LL_OpenEP(pdev, UVC_IN_EP, USBD_EP_TYPE_ISOC, UVC_ISO_HS_MPS);
        pdev->ep_in[UVC_IN_EP & 0xFU].is_used = 1U;
        pdev->ep_in[UVC_IN_EP & 0xFU].maxpacket = UVC_ISO_HS_MPS;
    }
    else
    {
        (void)USBD_LL_OpenEP(pdev, UVC_IN_EP, USBD_EP_TYPE_ISOC, UVC_ISO_FS_MPS);
        pdev->ep_in[UVC_IN_EP & 0xFU].is_used = 1U;
        pdev->ep_in[UVC_IN_EP & 0xFU].maxpacket = UVC_ISO_FS_MPS;
    }

    ((USBD_VIDEO_ItfTypeDef *)pdev->pUserData[pdev->classId])->Init();

    hVIDEO->interface = 0U;
    hVIDEO->uvc_state = UVC_PLAY_STATUS_STOP;        /* ★★★ FIX: Initialize state ★★★ */

    UNUSED(cfgidx);
    return (uint8_t)USBD_OK;
}

/**
 * @brief  USBD_VIDEO_DeInit
 */
static uint8_t USBD_VIDEO_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
    UNUSED(cfgidx);

    if (pdev->pClassData == NULL)
    {
        return (uint8_t)USBD_FAIL;
    }

    (void)USBD_LL_CloseEP(pdev, UVC_IN_EP);
    pdev->ep_in[UVC_IN_EP & 0xFU].is_used = 0U;

    ((USBD_VIDEO_ItfTypeDef *)pdev->pUserData[pdev->classId])->DeInit();

    USBD_free(pdev->pClassData);
    pdev->pClassData = NULL;

    return (uint8_t)USBD_OK;
}

/**
 * @brief  USBD_VIDEO_Setup
 */
static uint8_t USBD_VIDEO_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
    USBD_VIDEO_HandleTypeDef *hVIDEO = (USBD_VIDEO_HandleTypeDef *)pdev->pClassData;
    uint8_t ret = (uint8_t)USBD_OK;
    uint16_t len = 0U;
    uint8_t *pbuf = NULL;
    uint16_t status_info = 0U;

    switch (req->bmRequest & USB_REQ_TYPE_MASK)
    {
        case USB_REQ_TYPE_CLASS:
            switch (req->bRequest)
            {
                case UVC_GET_CUR:
                case UVC_GET_DEF:
                case UVC_GET_MIN:
                case UVC_GET_MAX:
                    VIDEO_REQ_GetCurrent(pdev, req);
                    break;
                case UVC_GET_RES:
                case UVC_GET_LEN:
                case UVC_GET_INFO:
                    break;
                case UVC_SET_CUR:
                    VIDEO_REQ_SetCurrent(pdev, req);
                    break;
                default:
                    (void)USBD_CtlError(pdev, req);
                    ret = (uint8_t)USBD_FAIL;
                    break;
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
                        ret = (uint8_t)USBD_FAIL;
                    }
                    break;
                case USB_REQ_GET_DESCRIPTOR:
                    if ((req->wValue >> 8) == CS_DEVICE)
                    {
                        pbuf = USBD_VIDEO_CfgHSDesc + 18;
                        len = MIN((uint16_t)USB_CONF_DESC_SIZE, (uint16_t)req->wLength);
                    }
                    (void)USBD_CtlSendData(pdev, pbuf, len);
                    break;
                case USB_REQ_GET_INTERFACE:
                    if (pdev->dev_state == USBD_STATE_CONFIGURED)
                    {
                        (void)USBD_CtlSendData(pdev, (uint8_t *)&hVIDEO->interface, 1);
                    }
                    else
                    {
                        USBD_CtlError(pdev, req);
                        ret = (uint8_t)USBD_FAIL;
                    }
                    break;
                case USB_REQ_SET_INTERFACE:
                    if (pdev->dev_state == USBD_STATE_CONFIGURED)
                    {
                        if (req->wValue <= USBD_MAX_NUM_INTERFACES)
                        {
                            hVIDEO->interface = LOBYTE(req->wValue);
                            if (hVIDEO->interface == 1U)
                            {
                                (void)USBD_LL_FlushEP(pdev, UVC_IN_EP);
                                ((USBD_VIDEO_ItfTypeDef *)pdev->pUserData[pdev->classId])->Start();
                                hVIDEO->uvc_state = UVC_PLAY_STATUS_READY;
                            }
                            else
                            {
                                ((USBD_VIDEO_ItfTypeDef *)pdev->pUserData[pdev->classId])->Stop();
                                hVIDEO->uvc_state = UVC_PLAY_STATUS_STOP;
                                (void)USBD_LL_FlushEP(pdev, UVC_IN_EP);
                            }
                        }
                        else
                        {
                            USBD_CtlError(pdev, req);
                            ret = (uint8_t)USBD_FAIL;
                        }
                    }
                    else
                    {
                        USBD_CtlError(pdev, req);
                        ret = (uint8_t)USBD_FAIL;
                    }
                    break;
                case USB_REQ_CLEAR_FEATURE:
                    break;
                default:
                    USBD_CtlError(pdev, req);
                    ret = (uint8_t)USBD_FAIL;
                    break;
            }
            break;

        default:
            USBD_CtlError(pdev, req);
            ret = (uint8_t)USBD_FAIL;
            break;
    }
    return ret;
}

/**
 * @brief  USBD_VIDEO_EP0_RxReady
 */
static uint8_t USBD_VIDEO_EP0_RxReady(USBD_HandleTypeDef *pdev)
{
    USBD_VIDEO_HandleTypeDef *hVIDEO;
    hVIDEO = (USBD_VIDEO_HandleTypeDef *)pdev->pClassData;

    if (hVIDEO == NULL)
    {
        return (uint8_t)USBD_FAIL;
    }

    if (pdev->request.wValue == (uint16_t)VS_COMMIT_CONTROL)
    {
        ((USBD_VIDEO_ItfTypeDef *)pdev->pUserData[pdev->classId])->Control(&video_Commit_Control);
    }
    return (uint8_t)USBD_OK;
}

/**
 * @brief  USBD_VIDEO_DataIn
 */
static uint8_t USBD_VIDEO_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
    UNUSED(pdev);
    UNUSED(epnum);
    return USBD_OK;
}

/**
 * @brief  USBD_VIDEO_SOF
 *         handle SOF event - FIXED VERSION
 */
/**
 * @brief  USBD_VIDEO_SOF
 *         handle SOF event - COMPLETELY FIXED VERSION
 *         NO STATIC VARIABLES THAT PERSIST ACROSS CALLS
 */
/**
 * @brief  USBD_VIDEO_SOF
 *         handle SOF event - COMPLETELY FIXED VERSION
 *         NO STATIC VARIABLES THAT PERSIST ACROSS CALLS (except packet buffer for DMA)
 */
__ALIGN_BEGIN static uint8_t packet[UVC_ISO_FS_MPS] __ALIGN_END;

static uint8_t USBD_VIDEO_SOF(USBD_HandleTypeDef *pdev)
{
    USBD_VIDEO_HandleTypeDef *hVIDEO = (USBD_VIDEO_HandleTypeDef *) pdev->pClassData;

    if (hVIDEO == NULL) {
        return (uint8_t)USBD_OK;
    }

    /* State: READY -> Send initial header -> STREAMING */
    if (hVIDEO->uvc_state == UVC_PLAY_STATUS_READY) {
        uint8_t payload[2] = {0x02U, 0x00U};
        (void)USBD_LL_Transmit(pdev, UVC_IN_EP, payload, 2U);
        hVIDEO->uvc_state = UVC_PLAY_STATUS_STREAMING;
        return (uint8_t)USBD_OK;
    }

    if (hVIDEO->uvc_state != UVC_PLAY_STATUS_STREAMING) {
        return (uint8_t)USBD_OK;
    }

    /* ★★★ FIX: ONLY packet buffer is static (for DMA alignment) ★★★ */

    /* ★★★ FIX: ALL OTHER variables are LOCAL - reset every SOF ★★★ */
    uint8_t *pbuf = NULL;
    uint16_t psize = 0;
    uint16_t pcktidx = 0;

    /* Call interface Data callback */
    ((USBD_VIDEO_ItfTypeDef *)pdev->pUserData[pdev->classId])->Data(&pbuf, &psize, &pcktidx);

    if (psize > 2 && pbuf != NULL) {
        /* ★★★ FIX: Build header FRESH from pbuf - NO static payload_header ★★★ */
        uint8_t header_byte = pbuf[0];  /* Get header from interface */
        uint8_t frame_id = pbuf[1];      /* Get Frame ID from interface */

        /* Copy payload (skip 2 bytes which contain header) */
        uint16_t payload_len = psize - 2;
        if (payload_len > (UVC_ISO_FS_MPS - 2)) {
            payload_len = UVC_ISO_FS_MPS - 2;
        }

        /* ★★★ FIX: Build packet fresh - NO modification of static data ★★★ */

        packet[0] = header_byte;
        packet[1] = frame_id;
        USBD_memcpy(&packet[2], &pbuf[2], payload_len);

        /* Transmit */
        uint16_t tx_size = payload_len + 2;
        if (tx_size > UVC_ISO_FS_MPS) tx_size = UVC_ISO_FS_MPS;
        (void)USBD_LL_Transmit(pdev, UVC_IN_EP, packet, tx_size);
    }

    return (uint8_t)USBD_OK;
}
/**
 * @brief  USBD_VIDEO_IsoINIncomplete
 */
static uint8_t USBD_VIDEO_IsoINIncomplete(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
    UNUSED(pdev);
    UNUSED(epnum);
    return (uint8_t)USBD_OK;
}

/**
 * @brief  VIDEO_REQ_GetCurrent
 */
static void VIDEO_REQ_GetCurrent(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
    USBD_VIDEO_HandleTypeDef *hVIDEO;
    hVIDEO = (USBD_VIDEO_HandleTypeDef *)(pdev->pClassData);
    static __IO uint8_t EntityStatus[8] = {0};

    (void)USBD_memset(hVIDEO->control.data, 0, USB_MAX_EP0_SIZE);

    if (LOBYTE(req->wIndex) == 0x00U)
    {
        if (HIBYTE(req->wValue) == 0x02U)
        {
            EntityStatus[0] = 0x06U;
            (void)USBD_CtlSendData(pdev, (uint8_t *)&EntityStatus, 1U);
        }
        else
        {
            USBD_CtlError(pdev, req);
        }
    }
    else
    {
        if (LOBYTE(req->wValue) == (uint8_t)VS_PROBE_CONTROL)
        {
            video_Probe_Control.bPreferedVersion = 0x00U;
            video_Probe_Control.bMinVersion = 0x00U;
            video_Probe_Control.bMaxVersion = 0x00U;
            video_Probe_Control.dwClockFrequency = 0x02DC6C00U;

            uint8_t format_index = video_Probe_Control.bFormatIndex;
            uint8_t frame_index = video_Probe_Control.bFrameIndex;

            if (pdev->dev_speed == USBD_SPEED_HIGH)
            {
                const uint32_t streaming_params_hs[2][2][2] = {
                    {
                        {UVC_INTERVAL(UVC_FPS_HS_MJPEG_VGA),  UVC_MAX_FRAME_SIZE_VGA},
                        {UVC_INTERVAL(UVC_FPS_HS_MJPEG_QVGA), UVC_MAX_FRAME_SIZE_QVGA},
                    },
                    {
                        {UVC_INTERVAL(UVC_FPS_HS_YUY2_VGA),   UVC_MAX_FRAME_SIZE_VGA},
                        {UVC_INTERVAL(UVC_FPS_HS_YUY2_QVGA),  UVC_MAX_FRAME_SIZE_QVGA},
                    }
                };
                video_Probe_Control.dwMaxPayloadTransferSize = UVC_ISO_HS_MPS;
                video_Probe_Control.dwFrameInterval = streaming_params_hs[format_index - 1][frame_index - 1][0];
                video_Probe_Control.dwMaxVideoFrameSize = streaming_params_hs[format_index - 1][frame_index - 1][1];
            }
            else
            {
                const uint32_t streaming_params_fs[2][2][2] = {
                    {
                        {UVC_INTERVAL(UVC_FPS_FS_MJPEG_VGA),  UVC_MAX_FRAME_SIZE_VGA},
                        {UVC_INTERVAL(UVC_FPS_FS_MJPEG_QVGA), UVC_MAX_FRAME_SIZE_QVGA},
                    },
                    {
                        {UVC_INTERVAL(UVC_FPS_FS_YUY2_VGA),   UVC_MAX_FRAME_SIZE_VGA},
                        {UVC_INTERVAL(UVC_FPS_FS_YUY2_QVGA),  UVC_MAX_FRAME_SIZE_QVGA},
                    }
                };
                video_Probe_Control.dwMaxPayloadTransferSize = UVC_ISO_FS_MPS;
                video_Probe_Control.dwFrameInterval = streaming_params_fs[format_index - 1][frame_index - 1][0];
                video_Probe_Control.dwMaxVideoFrameSize = streaming_params_fs[format_index - 1][frame_index - 1][1];
            }

            (void)USBD_CtlSendData(pdev, (uint8_t *)&video_Probe_Control, req->wLength);
        }
        else if (req->wValue == (uint8_t)VS_COMMIT_CONTROL)
        {
            (void)USBD_CtlSendData(pdev, (uint8_t *)&video_Commit_Control, req->wLength);
        }
        else
        {
            (void)USBD_CtlSendData(pdev, hVIDEO->control.data, req->wLength);
        }
    }
}

/**
 * @brief  VIDEO_REQ_SetCurrent
 */
static void VIDEO_REQ_SetCurrent(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
    USBD_VIDEO_HandleTypeDef *hVIDEO = (USBD_VIDEO_HandleTypeDef *)(pdev->pClassData);

    if (req->wLength > 0U)
    {
        if (req->wValue == (uint16_t)VS_PROBE_CONTROL)
        {
            (void)USBD_CtlPrepareRx(pdev, (uint8_t *)&video_Probe_Control, req->wLength);
        }
        else if (req->wValue == (uint16_t)VS_COMMIT_CONTROL)
        {
            (void)USBD_CtlPrepareRx(pdev, (uint8_t *)&video_Commit_Control, req->wLength);
        }
        else
        {
            (void)USBD_CtlPrepareRx(pdev, hVIDEO->control.data, req->wLength);
        }
    }
}

/**
 * @brief  USBD_VIDEO_GetFSCfgDesc
 */
static uint8_t *USBD_VIDEO_GetFSCfgDesc(uint16_t *length)
{
    *length = (uint16_t)(sizeof(USBD_VIDEO_CfgFSDesc));
    return USBD_VIDEO_CfgFSDesc;
}

/**
 * @brief  USBD_VIDEO_GetHSCfgDesc
 */
static uint8_t *USBD_VIDEO_GetHSCfgDesc(uint16_t *length)
{
    *length = (uint16_t)(sizeof(USBD_VIDEO_CfgHSDesc));
    return USBD_VIDEO_CfgHSDesc;
}

/**
 * @brief  USBD_VIDEO_GetOtherSpeedCfgDesc
 */
static uint8_t *USBD_VIDEO_GetOtherSpeedCfgDesc(uint16_t *length)
{
    *length = (uint16_t)(sizeof(USBD_VIDEO_CfgFSDesc));
    return USBD_VIDEO_CfgFSDesc;
}

/**
 * @brief  USBD_VIDEO_GetDeviceQualifierDesc
 */
static uint8_t *USBD_VIDEO_GetDeviceQualifierDesc(uint16_t *length)
{
    *length = (uint16_t)(sizeof(USBD_VIDEO_DeviceQualifierDesc));
    return USBD_VIDEO_DeviceQualifierDesc;
}

uint8_t USBD_VIDEO_RegisterInterface(USBD_HandleTypeDef *pdev, USBD_VIDEO_ItfTypeDef *fops)
{
    if (fops == NULL)
    {
        return (uint8_t)USBD_FAIL;
    }

    pdev->pUserData[pdev->classId] = fops;
    return (uint8_t)USBD_OK;
}

#endif /* USB_DEVICE_ENABLED */
#endif /* STM32H743xx */
