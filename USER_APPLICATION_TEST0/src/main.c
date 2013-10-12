/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
#include <asf.h>
#include "led.h"
#include "util/delay.h"
#include "interrupt.h"

//static void my_callback(void)
//{
	//gpio_toggle_pin(NHD_C12832A1Z_BACKLIGHT);
//}

static void led0_button()
{
		
}

int main (void)
{
	//pmic_init();
	sysclk_init();
	//tc_enable(&TCC0);
	//tc_set_overflow_interrupt_callback(&TCC0, my_callback);
	//tc_set_wgm(&TCC0, TC_WG_NORMAL);
	//tc_write_period(&TCC0, 15625);
	//tc_set_overflow_interrupt_level(&TCC0, TC_INT_LVL_LO);
	irq_initialize_vectors();
	cpu_irq_enable();
	//tc_write_clock_source(&TCC0, TC_CLKSEL_DIV64_gc);
	
	board_init();
	gfx_mono_init();
	
	// Insert application code here, after the board has been initialized.
	bool is_pressed;
	
	gpio_set_pin_high(NHD_C12832A1Z_BACKLIGHT);
	gfx_mono_draw_string("ESAT", 90, 8, &sysfont);
	
	while(1){
		////LED_Off(LED1);
		////LED_On(LED0);
		////_delay_ms(700);
		////LED_Off(LED0);
		////LED_On(LED1);
		////LED_Toggle(LED2);
		//LED_On(LED2);
		//_delay_ms(700);
		//LED_Off(LED2);
		//////LED_Toggle(LED3);
		//LED_On(LED3);
		//_delay_ms(700);
		//LED_Off(LED3);
		//_delay_ms(700);
		is_pressed = gpio_pin_is_low(GPIO_PUSH_BUTTON_1);	
		
		if(is_pressed){
			LED_Toggle(LED0);
			_delay_ms(700);
		}
	}
}
