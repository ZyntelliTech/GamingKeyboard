/* Copyright 2015-2016 Jack Humbert
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "usb_hid_keycode.h"

// clang-format off

/*
 * ┌───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───────┐
 * │ ² │ & │ é │ " │ ' │ ( │ - │ è │ _ │ ç │ à │ ) │ = │       │
 * ├───┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─────┤
 * │     │ A │ Z │ E │ R │ T │ Y │ U │ I │ O │ P │ ^ │ $ │     │
 * ├─────┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┐    │
 * │      │ Q │ S │ D │ F │ G │ H │ J │ K │ L │ M │ ù │ * │    │
 * ├────┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴───┴────┤
 * │    │ < │ W │ X │ C │ V │ B │ N │ , │ ; │ : │ ! │          │
 * ├────┼───┴┬──┴─┬─┴───┴───┴───┴───┴───┴──┬┴───┼───┴┬────┬────┤
 * │    │    │    │                        │    │    │    │    │
 * └────┴────┴────┴────────────────────────┴────┴────┴────┴────┘
 */
// Row 1
#define FR_SUP2 KEY_GRAVE  // ²
#define FR_AMPR KEY_1    // &
#define FR_EACU KEY_2    // é
#define FR_DQUO KEY_3    // "
#define FR_QUOT KEY_4    // '
#define FR_LPRN KEY_5    // (
#define FR_MINS KEY_6    // -
#define FR_EGRV KEY_7    // è
#define FR_UNDS KEY_8    // _
#define FR_CCED KEY_9    // ç
#define FR_AGRV KEY_0    // à
#define FR_RPRN KEY_MINUS // )
#define FR_EQL  KEY_EQUAL  // =
// Row 2
#define FR_A    KEY_Q    // A
#define FR_Z    KEY_W    // Z
#define FR_E    KEY_E    // E
#define FR_R    KEY_R    // R
#define FR_T    KEY_T    // T
#define FR_Y    KEY_Y    // Y
#define FR_U    KEY_U    // U
#define FR_I    KEY_I    // I
#define FR_O    KEY_O    // O
#define FR_P    KEY_P    // P
#define FR_CIRC KEY_LEFTBRACE // ^ (dead)
#define FR_DLR  KEY_RIGHTBRACE // $
// Row 3
#define FR_Q    KEY_A    // Q
#define FR_S    KEY_S    // S
#define FR_D    KEY_D    // D
#define FR_F    KEY_F    // F
#define FR_G    KEY_G    // G
#define FR_H    KEY_H    // H
#define FR_J    KEY_J    // J
#define FR_K    KEY_K    // K
#define FR_L    KEY_L    // L
#define FR_M    KEY_SEMICOLON // M
#define FR_UGRV KEY_APOSTROPHE // ù
#define FR_ASTR KEY_HASHTILDE // *
// Row 4
#define FR_LABK KEY_102ND // <
#define FR_W    KEY_Z    // W
#define FR_X    KEY_X    // X
#define FR_C    KEY_C    // C
#define FR_V    KEY_V    // V
#define FR_B    KEY_B    // B
#define FR_N    KEY_N    // N
#define FR_COMM KEY_M    // ,
#define FR_SCLN KEY_COMMA // ;
#define FR_COLN KEY_DOT  // :
#define FR_EXLM KEY_SLASH // !

/* Shifted symbols
 * ┌───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───────┐
 * │   │ 1 │ 2 │ 3 │ 4 │ 5 │ 6 │ 7 │ 8 │ 9 │ 0 │ ° │ + │       │
 * ├───┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─────┤
 * │     │   │   │   │   │   │   │   │   │   │   │ ¨ │ £ │     │
 * ├─────┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┐    │
 * │      │   │   │   │   │   │   │   │   │   │   │ % │ µ │    │
 * ├────┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴───┴────┤
 * │    │ > │   │   │   │   │   │   │ ? │ . │ / │ § │          │
 * ├────┼───┴┬──┴─┬─┴───┴───┴───┴───┴───┴──┬┴───┼───┴┬────┬────┤
 * │    │    │    │                        │    │    │    │    │
 * └────┴────┴────┴────────────────────────┴────┴────┴────┴────┘
 */
// Row 1
#define FR_1    FR_AMPR // 1
#define FR_2    FR_EACU // 2
#define FR_3    FR_DQUO // 3
#define FR_4    FR_QUOT // 4
#define FR_5    FR_LPRN // 5
#define FR_6    FR_MINS // 6
#define FR_7    FR_EGRV // 7
#define FR_8    FR_UNDS // 8
#define FR_9    FR_CCED // 9
#define FR_0    FR_AGRV // 0
#define FR_DEG  FR_RPRN // °
#define FR_PLUS FR_EQL  // +
// Row 2
#define FR_DIAE FR_CIRC // ¨ (dead)
#define FR_PND  FR_DLR  // £
// Row 3
#define FR_PERC FR_UGRV // %
#define FR_MICR FR_ASTR // µ
// Row 4
#define FR_RABK FR_LABK // >
#define FR_QUES FR_COMM // ?
#define FR_DOT  FR_SCLN // .
#define FR_SLSH FR_COLN // /
#define FR_SECT FR_EXLM // §

/* AltGr symbols
 * ┌───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───────┐
 * │   │   │ ~ │ # │ { │ [ │ | │ ` │ \ │   │ @ │ ] │ } │       │
 * ├───┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─────┤
 * │     │   │   │ € │   │   │   │   │   │   │   │   │ ¤ │     │
 * ├─────┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┐    │
 * │      │   │   │   │   │   │   │   │   │   │   │   │   │    │
 * ├────┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴───┴────┤
 * │    │   │   │   │   │   │   │   │   │   │   │   │          │
 * ├────┼───┴┬──┴─┬─┴───┴───┴───┴───┴───┴──┬┴───┼───┴┬────┬────┤
 * │    │    │    │                        │    │    │    │    │
 * └────┴────┴────┴────────────────────────┴────┴────┴────┴────┘
 */
// Row 1
#define FR_TILD FR_EACU // ~ (dead)
#define FR_HASH FR_DQUO // #
#define FR_LCBR FR_QUOT // {
#define FR_LBRC FR_LPRN // [
#define FR_PIPE FR_MINS // |
#define FR_GRV  FR_EGRV // ` (dead)
#define FR_BSLS FR_UNDS // (backslash)
#define FR_AT   FR_AGRV // @
#define FR_RBRC FR_RPRN // ]
#define FR_RCBR FR_EQL  // }
// Row 2
#define FR_EURO KC_E   // €
#define FR_CURR FR_DLR // ¤