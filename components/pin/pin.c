#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "soc/reg_base.h"
#include "driver/rtc_io.h"
#include "pin.h"

#define GPIO_OUT_REG                  (DR_REG_GPIO_BASE + 0x04)
#define GPIO_OUT_W1TS_REG             (DR_REG_GPIO_BASE + 0x08)
#define GPIO_OUT_W1TC_REG             (DR_REG_GPIO_BASE + 0x0C)
#define GPIO_IN_REG                   (DR_REG_GPIO_BASE + 0x3C)
#define GPIO_IN1_REG                  (DR_REG_GPIO_BASE + 0x40)
#define GPIO_FUNCN_OUT_SEL_CFG_REG(n) (DR_REG_GPIO_BASE + 0x530 + 0x04 * (n))
#define GPIO_PINN_REG(n)              (DR_REG_GPIO_BASE + 0x88 + 0x04 * (n))

#define IO_MUX_REG(n) (DR_REG_IO_MUX_BASE + PIN_MUX_REG_OFFSET[n])

#define FUN_WPD 7
#define FUN_WPU 8
#define FUN_IE 9
#define FUN_DRV 10
#define MCU_SEL 12
#define PAD_DRIVER_BIT 0

#define REG(addr) (*(volatile uint32_t *)(addr))
#define REG_SET_BIT(addr, bit) (REG(addr) |= (1U << (bit)))
#define REG_CLR_BIT(addr, bit) (REG(addr) &= ~(1U << (bit)))
#define REG_GET_BIT(addr, bit) ((REG(addr) >> (bit)) & 0x01U)

static const uint8_t PIN_MUX_REG_OFFSET[] = {
    0x44, 0x88, 0x40, 0x84, 0x48, 0x6C, 0x60, 0x64,
    0x68, 0x54, 0x58, 0x5C, 0x34, 0x38, 0x30, 0x3C,
    0x4C, 0x50, 0x70, 0x74, 0x78, 0x7C, 0x80, 0x8C,
    0x90, 0x24, 0x28, 0x2C, 0xFF, 0xFF, 0xFF, 0xFF,
    0x1C, 0x20, 0x14, 0x18, 0x04, 0x08, 0x0C, 0x10,
};

int32_t pin_reset(pin_num_t pin) {
    if (rtc_gpio_is_valid_gpio(pin)) {
        rtc_gpio_deinit(pin);
        rtc_gpio_pullup_en(pin);
        rtc_gpio_pulldown_dis(pin);
    }

    REG(GPIO_PINN_REG(pin)) = 0;
    REG(GPIO_FUNCN_OUT_SEL_CFG_REG(pin)) = 0x100;
    REG(IO_MUX_REG(pin)) = 0x2B00;  

    return pin_set_level(pin, 0);
}

int32_t pin_pullup(pin_num_t pin, bool enable) {
    if (rtc_gpio_is_valid_gpio(pin)) {
        return enable ? rtc_gpio_pullup_en(pin) : rtc_gpio_pullup_dis(pin);
    }

    uint32_t reg = IO_MUX_REG(pin);
    if (enable) {
        REG_SET_BIT(reg, FUN_WPU);
    } else {
        REG_CLR_BIT(reg, FUN_WPU);
    }

    return 0;
}

int32_t pin_pulldown(pin_num_t pin, bool enable) {
    if (rtc_gpio_is_valid_gpio(pin)) {
        return enable ? rtc_gpio_pulldown_en(pin) : rtc_gpio_pulldown_dis(pin);
    }

    uint32_t reg = IO_MUX_REG(pin);
    if (enable) {
        REG_SET_BIT(reg, FUN_WPD);
    } else {
        REG_CLR_BIT(reg, FUN_WPD);
    }

    return 0;
}

int32_t pin_input(pin_num_t pin, bool enable) {
    uint32_t reg = IO_MUX_REG(pin);
    if (enable) {
        REG_SET_BIT(reg, FUN_IE);
    } else {
        REG_CLR_BIT(reg, FUN_IE);
    }

    return 0;
}

int32_t pin_output(pin_num_t pin, bool enable) {
    uint32_t reg = (pin < 32) ? (DR_REG_GPIO_BASE + 0x20) : (DR_REG_GPIO_BASE + 0x24);
    uint32_t bit = (pin < 32) ? pin : (pin - 32);

    if (enable) {
        REG_SET_BIT(reg, bit);
    } else {
        REG_CLR_BIT(reg, bit);
    }

    return 0;
}

int32_t pin_odrain(pin_num_t pin, bool enable) {
    uint32_t reg = IO_MUX_REG(pin);
    if (enable) {
        REG_SET_BIT(reg, PAD_DRIVER_BIT);
    } else {
        REG_CLR_BIT(reg, PAD_DRIVER_BIT);
    }

    return 0;
}

int32_t pin_set_level(pin_num_t pin, int32_t level) {
    uint32_t reg_set = (pin < 32) ? GPIO_OUT_W1TS_REG : (GPIO_OUT_W1TS_REG + 4);
    uint32_t reg_clr = (pin < 32) ? GPIO_OUT_W1TC_REG : (GPIO_OUT_W1TC_REG + 4);
    uint32_t bit = (pin < 32) ? pin : (pin - 32);

    if (level) {
        REG(reg_set) = (1U << bit);
    } else {
        REG(reg_clr) = (1U << bit);
    }

    return 0;
}

int32_t pin_get_level(pin_num_t pin) {
    uint32_t reg = (pin < 32) ? GPIO_IN_REG : GPIO_IN1_REG;
    uint32_t bit = (pin < 32) ? pin : (pin - 32);

    return ((REG(reg) & (1U << bit)) != 0) ? 1 : 0;
}

uint64_t pin_get_in_reg(void) {
    uint64_t low = (uint64_t)REG(GPIO_IN_REG);
    uint64_t high = (uint64_t)REG(GPIO_IN1_REG);

    return (high << 32) | low;
}

uint64_t pin_get_out_reg(void) {
    uint64_t low = (uint64_t)REG(GPIO_OUT_REG);
    uint64_t high = (uint64_t)REG(GPIO_OUT_REG + 4);

    return (high << 32) | low;
}
