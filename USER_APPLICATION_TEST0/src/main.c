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
//test Github
#include <asf.h>
#include "led.h"
#include <util/delay.h>
#include "interrupt.h"
#include <avr/interrupt.h> 
//#include "QT2160.h"
//#include "touch_qt_config.h"//"touch_api.h"
//#include "touch_qt_config.h"
//#include "keyboard.h"
//#include "cdc.h"
#include "xmega_a3bu_xplained.h"
#include "adc.h"
#include <stdio.h>
#include "rtc32.h"
//
//static void my_callback(void)
//{
	//gpio_toggle_pin(NHD_C12832A1Z_BACKLIGHT);
//}
//int count = 0;

//uint8_t plant_location = 0;

/*
static void leds_toggle()
{
	LED_Toggle(LED2);
	LED_Toggle(LED3);
}

static void change_text()
{
	//gfx_mono_draw_filled_rect(0, 8, 128, 8, GFX_PIXEL_CLR);
	
	
	if(count == 0){
		gfx_mono_draw_string("     ", 78, 24, &sysfont);
		gfx_mono_draw_string("ESAT", 20, 16, &sysfont);
		//_delay_ms(2000);
		//gfx_mono_draw_filled_rect(20, 16, 24, 8, GFX_PIXEL_CLR);
	}
	else{
		gfx_mono_draw_string("    ", 20, 16, &sysfont);
		gfx_mono_draw_string("SUPER", 78, 24, &sysfont);
		//_delay_ms(2000);
		//gfx_mono_draw_filled_rect(78, 24, 30, 8, GFX_PIXEL_CLR);
	}
	count = (count + 1) % 2;
}
*/
uint16_t sun_value = 0;

static void sun_count()
{
	sun_value++;
	
	char* sun_val_string = NULL;
	sprintf(sun_val_string, "%4d", sun_value);
	//gfx_mono_draw_filled_rect(25, 0, 24, 8, GFX_PIXEL_CLR);
	gfx_mono_draw_string(sun_val_string, 25, 0, &sysfont);
}

uint8_t zombie_position = 122; //in x

static void zombie_walk()
{
	//if(gpio_pin_is_low(GPIO_PUSH_BUTTON_0)){
	gfx_mono_draw_filled_rect(zombie_position+5, 8, 1, 8, GFX_PIXEL_CLR);
	zombie_position--;
	gfx_mono_draw_string("@", zombie_position, 8, &sysfont);
	//}
}

uint8_t cursor_position = 8; //in y

uint8_t bullet_position = 13; //in x

static void bullet_seed()
{
	gfx_mono_draw_filled_rect(bullet_position, cursor_position, 1, 8, GFX_PIXEL_CLR);
	bullet_position++;
	gfx_mono_draw_string("*", bullet_position, cursor_position, &sysfont);
}


static void cursor_select()
{
	bool up_pressed = gpio_pin_is_low(GPIO_PUSH_BUTTON_1);
	bool down_pressed = gpio_pin_is_low(GPIO_PUSH_BUTTON_2);
	
	if(up_pressed){
		
		gfx_mono_draw_filled_rect(0, cursor_position, 6, 8, GFX_PIXEL_CLR);
		
		if(cursor_position == 8){
			cursor_position = 24;
		}
		else{
			cursor_position -= 8;
		}

		gfx_mono_draw_string(">", 0, cursor_position, &sysfont);
	}

	if(down_pressed){
		gfx_mono_draw_filled_rect(0, cursor_position, 6, 8, GFX_PIXEL_CLR);
		
		if(cursor_position == 24){
			cursor_position = 8;
		}
		else{
			cursor_position += 8;
		}

		gfx_mono_draw_string(">", 0, cursor_position, &sysfont);
	}
	
	bool planting = gpio_pin_is_low(GPIO_PUSH_BUTTON_0);

	bool check_plant[] = {false, false, false};
	uint8_t plant_index = cursor_position / 8 - 1;

	if(planting && sun_value >= 10 && !check_plant[plant_index]){
		gfx_mono_draw_string("P", 6, cursor_position, &sysfont);
		sun_value -= 10;
		uint8_t temp_p = plant_index; 
		check_plant[temp_p] = true;
		
		//if(check_plant[temp_p]){gfx_mono_draw_string("POK", 70, 24, &sysfont);}
	}
	
	char* buffer_check;
	for(int c = 0; c < 3; c++){
		sprintf(buffer_check, "%d", check_plant[c]);
		gfx_mono_draw_string(buffer_check, 70, (c+1)*8, &sysfont);
	}
	//bool shooting = gpio_pin_is_high(QTOUCH_BUTTON_SNSK);

	if(planting && check_plant[cursor_position]){
		gfx_mono_draw_string("OK", 115, 24, &sysfont);

		tc_enable(&TCE0);
		tc_set_overflow_interrupt_callback(&TCE0, bullet_seed);
		tc_set_wgm(&TCE0, TC_WG_NORMAL);
		tc_write_period(&TCE0, 15625);
		tc_set_overflow_interrupt_level(&TCE0, TC_INT_LVL_LO);
		tc_write_clock_source(&TCE0, TC_CLKSEL_DIV64_gc);
	}
}

int main (void)
{
	
	board_init();
	sysclk_init();
	pmic_init();
	
	tc_enable(&TCC0);
	tc_set_overflow_interrupt_callback(&TCC0, sun_count);
	tc_set_wgm(&TCC0, TC_WG_NORMAL);
	tc_write_period(&TCC0, 31250);
	tc_set_overflow_interrupt_level(&TCC0, TC_INT_LVL_LO);
	tc_write_clock_source(&TCC0, TC_CLKSEL_DIV256_gc);
	
	cpu_irq_enable();
	
	gfx_mono_init();
	gfx_mono_draw_string("SUN:   0", 0, 0, &sysfont);
	gfx_mono_draw_string(">", 0, cursor_position, &sysfont);
	//gfx_mono_draw_string(sun_val_string, 25, 0, &sysfont);
	//gfx_mono_draw_string("123456789012345678901", 0, 8, &sysfont);
	// Insert application code here, after the board has been initialized.
	//bool is_pressed, is_touched;//, is_light;
	
	gpio_set_pin_high(NHD_C12832A1Z_BACKLIGHT);
	/*PraktikumX3*/
	//gfx_mono_draw_string("Praktikum", 40, 8, &sysfont);
	//gfx_mono_draw_string("X3", 58, 16, &sysfont);
	
	/* jadul
	uint32_t sun_value = 107;
	char* sun_val_string;
	*/
	
	//gfx_mono_draw_string("SUN: 17", 0, 0, &sysfont);
	//gfx_mono_draw_string("SCORE:  7", 75, 0, &sysfont);
	//gfx_mono_draw_string("  P", 0, 16, &sysfont);
	////gfx_mono_draw_string("ESAT", 5, 24, &sysfont);
	//gfx_mono_draw_string("> P     )", 0, 24, &sysfont);
	//gfx_mono_draw_string("@", 118, 24, &sysfont);
	
	tc_enable(&TCC1);
	tc_set_overflow_interrupt_callback(&TCC1, cursor_select);
	tc_set_wgm(&TCC1, TC_WG_NORMAL);
	tc_write_period(&TCC1, 50000);
	tc_set_overflow_interrupt_level(&TCC1, TC_INT_LVL_LO);
	tc_write_clock_source(&TCC1, TC_CLKSEL_DIV8_gc);
	
	/* bullet seed timer was moved to cursor_select() */
		
	tc_enable(&TCE1);
	tc_set_overflow_interrupt_callback(&TCE1, zombie_walk);
	tc_set_wgm(&TCE1, TC_WG_NORMAL);
	tc_write_period(&TCE1, 15625);
	tc_set_overflow_interrupt_level(&TCE1, TC_INT_LVL_LO);
	tc_write_clock_source(&TCE1, TC_CLKSEL_DIV64_gc);
	
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
		//is_pressed = gpio_pin_is_low(GPIO_PUSH_BUTTON_1);	
		//is_touched = gpio_pin_is_high(QTOUCH_BUTTON_SNSK);//check_touch_key_pressed();
		//is_light = gpio_pin_is_high(LIGHT_SENSOR_SIGNAL_PIN);
		//gpio_toggle_pin
		//check_touch_key_pressed();
		
		/* RTC failed
		sun_value = rtc_get_time();
		sprintf(sun_val_string, "%4ld", sun_value);
		gfx_mono_draw_string("SUN:", 0, 0, &sysfont);
		gfx_mono_draw_string(sun_val_string, 21, 0, &sysfont);
		*/
		/*if(is_pressed){
			LED_Toggle(LED0);
			_delay_ms(200);
		}
		if(is_touched){
			gpio_toggle_pin(NHD_C12832A1Z_BACKLIGHT);
			_delay_ms(200);			
		}*/
		/*if(is_light){
			LED_Toggle(LED1);
			_delay_ms(200);
		}*/
	}
}
