// SPDX-License-Identifier: EUPL-1.2
//
// This file is part of AUI.
// AUI is distributed under the terms of the European Union Public License v1.2.
// You may obtain a copy of the license at:
// https://joinup.ec.europa.eu/collection/eupl/eupl-text-eupl-12

#pragma once

/**
 * @file aui_messages.h
 * @brief Defines all message IDs used by the AUI framework.
 *
 * Messages are the central communication mechanism in AUI. Every element
 * receives messages through handle_message(), allowing the system to remain
 * modular, deterministic, and event‑driven.
 *
 * Message IDs are grouped by purpose:
 *  - Core lifecycle messages
 *  - UI/visual messages
 *  - Hardware interaction messages
 *  - UART/logging messages
 */

/* ---------------------------------------------------------------------------
 * Core lifecycle messages
 * -------------------------------------------------------------------------*/

/**
 * @brief Sent once per loop iteration.
 *
 * Used for periodic updates, polling, and time‑based logic.
 */
#define MSG_ONLOOP              1

/**
 * @brief Sent once during system initialization.
 *
 * Elements typically use this to configure hardware or initialize state.
 */
#define MSG_ONSETUP             0


/* ---------------------------------------------------------------------------
 * Visual/UI messages
 * -------------------------------------------------------------------------*/

/**
 * @brief Requests a visual element to render itself.
 *
 * Display backends or UI managers trigger this message when repainting.
 */
#define MSG_PAINT               2

/**
 * @brief Generic LED toggle/switch command.
 *
 * Used by button elements or other controllers to toggle LED widgets.
 */
#define MSG_LED_SWITCH          3

/* ---------------------------------------------------------------------------
 * GPIO messages 
 * -------------------------------------------------------------------------*/
#define MSG_GPIO_WRITE          4

#define MSG_GPIO_READ           5

/* ---------------------------------------------------------------------------
 * UART / Serial communication messages
 * -------------------------------------------------------------------------*/

/**
 * @brief Raw UART byte received.
 *
 * Payload: pointer to a single char (size = 1).
 * Sent by aui_serial::on_serial_event().
 */
#define MSG_UART_RAW_RX         12

/**
 * @brief Informational UART text message.
 *
 * Payload: null‑terminated char string.
 * Printed only if log level >= INFO.
 */
#define MSG_UART_INFO_TEXT      13

/**
 * @brief Error UART text message.
 *
 * Payload: null‑terminated char string.
 * Printed only if log level >= ERROR.
 */
#define MSG_UART_ERROR_TEXT     14

/**
 * @brief Verbose UART text message.
 *
 * Payload: null‑terminated char string.
 * Printed only if log level >= VERBOSE.
 */
#define MSG_UART_VERBOSE_TEXT   15

/**
 * @brief Debug UART text message.
 *
 * Payload: null‑terminated char string.
 * Printed only if log level >= DEBUG.
 */
#define MSG_UART_DEBUG_TEXT     16


