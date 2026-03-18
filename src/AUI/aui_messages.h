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

#define MSG_ENABLE       2
#define MSG_DISABLE      3
#define MSG_RESET        4

/* ---------------------------------------------------------------------------
 * GPIO messages 
 * -------------------------------------------------------------------------*/


#define MSG_GPIO_WRITE          5

#define MSG_GPIO_READ           6

#define MSG_PAINT              8
/**
 * @brief Generic GPIO toggle/switch command.
 *
 */
#define MSG_GPIO_SWITCH          7

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

/*
 * @brief Debug UART text message.
 *
 * Payload: null‑terminated char string.
 */
#define MSG_UART_WRITE     13


#define MSG_UART_READ     14

