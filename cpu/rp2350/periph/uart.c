/*
 * Copyright (C) 2021 Nick Weiler, Justus Krebs, Franz Freitag
 * Copyright (C) 2021 Otto-von-Guericke Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_rpx0xx
 * @ingroup     drivers_periph_uart
 * @{
 *
 * @file
 * @brief       UART driver implementation for the RP2040
 *
 * @author      Franz Freitag <franz.freitag@st.ovgu.de>
 * @author      Justus Krebs <justus.krebs@st.ovgu.de>
 * @author      Nick Weiler <nick.weiler@st.ovgu.de>
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include "assert.h"
#include "bitarithm.h"
#include "board.h"
#include "io_reg.h"
#include "irq.h"
#include "mutex.h"
#include "RP2350.h"
#include "periph/gpio.h"
#include "periph/uart.h"
#include "periph_conf.h"
#include "periph_cpu.h"
