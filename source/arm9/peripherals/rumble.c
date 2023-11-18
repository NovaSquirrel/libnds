// SPDX-License-Identifier: Zlib
//
// Copyright (c) 2023 Adrian "asie" Siekierka

#include <nds/ndstypes.h>
#include <nds/memory.h>
#include <nds/system.h>
#include <nds/arm9/rumble.h>

bool isRumbleInserted(void) {
    if (!peripheralSlot2IsDetected())
        rumbleInit();
    return peripheralSlot2GetSupportMask() & SLOT2_PERIPHERAL_RUMBLE_ANY;
}

uint8_t rumbleGetMaxStrength(void) {
    uint32_t mask = peripheralSlot2GetSupportMask();
    if (mask & SLOT2_PERIPHERAL_RUMBLE_EZ)
        return 3;
    if (mask & SLOT2_PERIPHERAL_RUMBLE_ANY)
        return 1;
    return 0;
}

#define RUMBLE_DS_PAK  (*(vuint16 *)0x08000000)
#define GPIO_DATA      (*(vuint16 *)0x080000C4)
extern void slot2EzCommand(uint32_t address, uint16_t value);
static const uint8_t ez_rumble_table[] = {0x00, 0xF0, 0xF2, 0xF1};

void setRumble(uint8_t strength) {
    uint32_t mask = peripheralSlot2GetSupportMask();
    peripheralSlot2Open(SLOT2_PERIPHERAL_RUMBLE_ANY);

    if (mask & SLOT2_PERIPHERAL_RUMBLE_GPIO) {
        GPIO_DATA = (GPIO_DATA & ~0x8) | (strength ? 0x8 : 0x0);
    } else if (mask & SLOT2_PERIPHERAL_RUMBLE_PAK) {
        if (mask & SLOT2_PERIPHERAL_RUMBLE_EZ)
            slot2EzCommand(0x9E20000, ez_rumble_table[strength > 3 ? 3 : strength]);
        RUMBLE_DS_PAK = strength ? 0x2 : 0x0;
    } else if (mask & SLOT2_PERIPHERAL_SLIDE_MAGKID) {
        // TODO: Untested.
        RUMBLE_DS_PAK = strength ? 0x100 : 0x000;
    }
}
