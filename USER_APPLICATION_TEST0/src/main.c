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
#include <util/delay.h>
#include <stdio.h>

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
	
	/*below part of code only for checking not use in final form of code*/
	char* buffer_check;
	for(int c = 0; c < 3; c++){
		sprintf(buffer_check, "%d", check_plant[c]);
		gfx_mono_draw_string(buffer_check, 70, (c+1)*8, &sysfont);
	}
	//bool shooting = gpio_pin_is_high(QTOUCH_BUTTON_SNSK);
/*
	if(planting && check_plant[cursor_position]){
		gfx_mono_draw_string("OK", 115, 24, &sysfont);

		tc_enable(&TCE0);
		tc_set_overflow_interrupt_callback(&TCE0, bullet_seed);
		tc_set_wgm(&TCE0, TC_WG_NORMAL);
		tc_write_period(&TCE0, 15625);
		tc_set_overflow_interrupt_level(&TCE0, TC_INT_LVL_LO);
		tc_write_clock_source(&TCE0, TC_CLKSEL_DIV64_gc);
	}
	*/
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
	
	gpio_set_pin_high(NHD_C12832A1Z_BACKLIGHT);
	
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
	tc_write_period(&TCE1, 31250);
	tc_set_overflow_interrupt_level(&TCE1, TC_INT_LVL_LO);
	tc_write_clock_source(&TCE1, TC_CLKSEL_DIV64_gc);
	
	while(1){
		
	}
}
