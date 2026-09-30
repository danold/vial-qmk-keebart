// Automatic OS -> base layer. Mac = layer 0, Windows/Linux = layer 4.
// Needs `OS_DETECTION_ENABLE = yes` in rules.mk and `SRC += os_detect_layer.c`.
#include QMK_KEYBOARD_H
#include "os_detection.h"

bool process_detected_host_os_user(os_variant_t os) {
    switch (os) {
        case OS_MACOS:
        case OS_IOS:
            default_layer_set(1UL << 0);
            break;
        case OS_WINDOWS:
        case OS_LINUX:
            default_layer_set(1UL << 4);
            break;
        default:
            break;  // OS_UNSURE: keep whatever PDF(0)/PDF(4) last stored
    }
    return true;
}
