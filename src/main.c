/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/drivers/display.h>
#include <zephyr/logging/log.h>
#include <lvgl.h>


LOG_MODULE_REGISTER(main);



/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   1000

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led3)

/* Display device from device tree */
#define DISPLAY_NODE DT_NODELABEL(ili9341)


/*
 * A build error on this line means your board is unsupported.
 * See the sample documentation for information on how to fix this.
 */
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct device *display_dev = DEVICE_DT_GET(DISPLAY_NODE);


int main(void)
{
	int ret;
	bool led_state = true;


	if (!gpio_is_ready_dt(&led)) {
		return 0;
	}

	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return 0;
	}

	const struct device *spi1 = DEVICE_DT_GET(DT_NODELABEL(spi1));
    if (!device_is_ready(spi1)) {
        printk("SPI1 device not ready\n");
        return 0;
    }
    printk("SPI1 initialized successfully\n");

	/* Initialize ILI9341 display */
    if (!device_is_ready(display_dev)) {
        printk("Display device not ready\n");
        return 0;
    }
	lv_obj_t *hello_world_label;


	hello_world_label = lv_label_create(lv_screen_active());

	lv_label_set_text(hello_world_label, "Zacck DIY CAM! \n Connect Camera to Proceed");
	lv_obj_align(hello_world_label, LV_ALIGN_CENTER, 0, 0);

	
	lv_timer_handler();

    /* Turn off display blanking */
    ret = display_blanking_off(display_dev);
    if (ret < 0) {
        printk("Failed to turn off display blanking: %d\n", ret);
        return 0;
    }







	while (1) {
		lv_timer_handler();
		ret = gpio_pin_toggle_dt(&led);
		if (ret < 0) {
			return 0;
		}

		led_state = !led_state;
		printf("LED state: %s\n", led_state ? "ON" : "OFF");
		k_msleep(SLEEP_TIME_MS);
	}
	return 0;
}
