// Per-layer and per-key RGB for the Keebart Corne Choc Pro (Vial / RGB Matrix).
// Put in keyboards/keebart/corne_choc_pro/keymaps/vial/ and add `SRC += rgb_layers.c` to rules.mk.
//
// Behaviour:
//  - Base layers (0 = Mac, 4 = Win/Linux): your normal Vial RGB effect runs untouched.
//    Optional: the 6 thumb LEDs show which OS mode is active.
//  - Any other layer: every key is painted. Unused keys go dark, keys that do something
//    get the layer colour, and special key kinds get their own accent colour.
#include QMK_KEYBOARD_H
#include "rgb_layer_map.h"

// ============================ EDIT COLOURS HERE ============================
// HSV: hue 0-255 (0 red, 21 orange, 43 yellow, 85 green, 128 cyan, 170 blue, 200 purple, 213 pink)
// Final brightness is scaled by your Vial brightness setting, so V here is "relative".
#define HSV_(h, s, v) { h, s, v }

// Layer colour = colour of the "ordinary" keys on that layer. Mac (1-3) and Win/Linux (5-7)
// use the same hue per function on purpose, so you learn the colours once.
static const HSV layer_color[16] = {
    [1] = HSV_(170, 255, 255),  // nav / numbers  : blue
    [2] = HSV_( 21, 255, 255),  // symbols        : orange
    [3] = HSV_(200, 255, 255),  // adjust         : purple
    [5] = HSV_(170, 255, 255),
    [6] = HSV_( 21, 255, 255),
    [7] = HSV_(200, 255, 255),
};

// Accent colours per key kind. A zero-V entry means "use the layer colour instead".
static const HSV class_color[CLS_COUNT] = {
    [CLS_OFF]    = HSV_(  0,   0,   0),  // unused keys: dark
    [CLS_PASS]   = HSV_(  0,   0,  40),  // transparent keys (thumbs etc.): dim white
    [CLS_ALTGR]  = HSV_( 43, 255, 255),  // OS-dependent chars { } [ ] | \ @ $ : yellow
    [CLS_MACRO]  = HSV_(213, 255, 255),  // dead keys ` ´ ^ ~ : pink
    [CLS_LAYER]  = HSV_(  0,   0, 255),  // layer keys: white
    [CLS_SYSTEM] = HSV_(  0, 255, 255),  // RGB controls / bootloader: red
    [CLS_OS_MAC] = HSV_(  0,   0, 255),  // "switch to Mac mode" key: white
    [CLS_OS_WIN] = HSV_( 85, 255, 255),  // "switch to Win/Linux mode" key: green
    // everything below falls back to the layer colour (V = 0):
    [CLS_LETTER] = HSV_(0, 0, 0), [CLS_DIGIT] = HSV_(0, 0, 0), [CLS_SYM] = HSV_(0, 0, 0),
    [CLS_NAV]    = HSV_(85, 255, 255),   // arrows / delete / pgup: green (set V=0 to use layer colour)
    [CLS_EDIT]   = HSV_(0, 0, 0), [CLS_MOD] = HSV_(0, 0, 0), [CLS_MEDIA] = HSV_(128, 255, 255),
};

// OS indicator on the base layers: thumb-cluster LEDs. Set V to 0 to disable.
#define OS_INDICATOR_MAC HSV_(  0,   0, 120)  // Mac      : soft white
#define OS_INDICATOR_WIN HSV_( 85, 255, 120)  // Win/Linux: green
// ===========================================================================

static void set_hsv(uint8_t led, HSV c) {
    c.v = scale8(c.v, rgb_matrix_get_val());
    RGB rgb = hsv_to_rgb(c);
    rgb_matrix_set_color(led, rgb.r, rgb.g, rgb.b);
}

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    uint8_t layer   = get_highest_layer(layer_state);
    bool    overlay = layer_state != 0 && layer_has_overlay[layer];
    bool    win     = (default_layer_state & (1UL << 4)) != 0;

    for (uint8_t r = 0; r < MATRIX_ROWS; r++) {
        for (uint8_t c = 0; c < MATRIX_COLS; c++) {
            uint8_t led = g_led_config.matrix_co[r][c];
            if (led == NO_LED || led < led_min || led >= led_max) continue;

            if (overlay) {
                uint8_t cls = pgm_read_byte(&layer_key_class[layer][r][c]);
                if (cls == CLS_NONE) continue;
                HSV col = class_color[cls];
                if (col.v == 0 && cls != CLS_OFF) col = layer_color[layer];
                set_hsv(led, col);
            } else if ((r == 3 || r == 7) && layer_state == 0) {
                set_hsv(led, win ? (HSV)OS_INDICATOR_WIN : (HSV)OS_INDICATOR_MAC);
            }
        }
    }
    return false;
}
