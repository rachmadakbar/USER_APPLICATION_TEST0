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
#include <avr/eeprom.h>
#include <string.h>
#include <asf.h>
#include "adc_sensors.h"
#include "sysfont.h"
#include "util/delay.h"

uint16_t sun_value = 0;
uint16_t score = 0;
uint16_t zombie = 10;
uint8_t plant_cost = 5;

char atas[] =  {"                                                                                          "};
char tengah[] = {"                                                                                          "};
char bawah[] = {"                                                                                          "};

char tembak_atas[] ={"                   "};
char tembak_tengah[] ={"                   "};
char tembak_bawah[] ={"                   "};

uint16_t i,j;
uint8_t rn;
bool lose = false;
uint8_t cursor_position = 8; //in y
bool plant[] = {false, false, false};
uint8_t state = 1;

void tampilkanPeta(void);
void tampilkanTembak(void);
void sun_count(void);
void randomPeta(void);
void start_game(void);
void led_start(void);
void sunBurst(void);
void button_press(void);

int main (void)
{
	sysclk_init();
	board_init();
	pmic_init();
	gfx_mono_init();
	adc_sensors_init();
	// Enable display backlight
	gpio_set_pin_high(NHD_C12832A1Z_BACKLIGHT);
	cpu_irq_enable();
	
	while(true){
		
		if(state==1){
			start_game();
			}else if(state==2){
			tc_enable(&TCC0);
			tc_set_overflow_interrupt_callback(&TCC0, sun_count);
			tc_set_wgm(&TCC0, TC_WG_NORMAL);
			tc_write_period(&TCC0, 13500);
			tc_set_overflow_interrupt_level(&TCC0, TC_INT_LVL_LO);
			tc_write_clock_source(&TCC0, TC_CLKSEL_DIV256_gc);
			
			tc_enable(&TCC1);
			tc_set_overflow_interrupt_callback(&TCC1, button_press);
			tc_set_wgm(&TCC1, TC_WG_NORMAL);
			tc_write_period(&TCC1, 62500);
			tc_set_overflow_interrupt_level(&TCC1, TC_INT_LVL_LO);
			tc_write_clock_source(&TCC1, TC_CLKSEL_DIV8_gc);
			
			gfx_mono_draw_string("SUN:   0", 0, 0, &sysfont);
			gfx_mono_draw_string(">", 0, cursor_position, &sysfont);
			gfx_mono_draw_string("Score:  0", 63, 0, &sysfont);
			
			randomPeta();
			
			char* score_string = NULL;
			uint16_t old_score = 0;
			
			for(j = 0; j <= 70; j++){
				
				if(sun_value > 10){
					
					lightsensor_measure();
					while (!lightsensor_data_is_ready()) {
						// Wait until the conversion is complete
					}
					if(lightsensor_get_raw_value() > 250){
						sun_value -= 10;
						sunBurst();
						gfx_mono_draw_filled_rect(12,8,114,24,GFX_PIXEL_CLR);
					}
				}
				

				if(score > old_score){
					sprintf(score_string, "%3d", score);
					gfx_mono_draw_string(score_string, 100, 0, &sysfont);
					old_score = score;
				}
				
				if(lose){
					state=3;
					break;
					}else if(zombie==0){
					state=4;
					break;
				}
				
				
				tampilkanPeta();
				tampilkanTembak();
				delay_ms(1000);
			}
			}else if(state==3){
			cpu_irq_disable();
			gfx_mono_draw_filled_rect(0,0,128,32,GFX_PIXEL_CLR);
			while(true){
				gfx_mono_draw_string("GAME OVER",36,8,&sysfont)	;
				gfx_mono_draw_string("You Lose",39,20,&sysfont)	;
			}
			}else if(state==4){
			cpu_irq_disable();
			gfx_mono_draw_filled_rect(0,0,128,32,GFX_PIXEL_CLR);
			while(true){
				gfx_mono_draw_string("GAME OVER",36,2,&sysfont)	;
				gfx_mono_draw_string("You Win",42,12,&sysfont)	;
				gfx_mono_draw_string("Score = ",30,22,&sysfont)	;
				char* score_string = NULL;
				sprintf(score_string, "%3d", score);
				gfx_mono_draw_string(score_string, 79, 22, &sysfont);
			}
		}
	}
	
}

void button_press(void){
	if(gpio_pin_is_low(GPIO_PUSH_BUTTON_1)){
		gfx_mono_draw_filled_rect(0,cursor_position,5,8,GFX_PIXEL_CLR);
		if(cursor_position==8) cursor_position = 24;
		else cursor_position-=8;
		gfx_mono_draw_char('>', 0, cursor_position, &sysfont);
	}
	if(gpio_pin_is_low(GPIO_PUSH_BUTTON_2)){
		gfx_mono_draw_filled_rect(0,cursor_position,5,8,GFX_PIXEL_CLR);
		if(cursor_position==24) cursor_position = 8;
		else cursor_position+=8;
		gfx_mono_draw_char('>', 0, cursor_position, &sysfont);
	}
	if(gpio_pin_is_low(GPIO_PUSH_BUTTON_0)){
		if(cursor_position==8){
			if(plant[0]==false){
				if(sun_value>=plant_cost){
					sun_value -= plant_cost;
					plant[0] = true;
					gfx_mono_draw_char('P',6,cursor_position,&sysfont);
				}
			}
			else{
				if(sun_value>=2){
					sun_value -= 2;
					tembak_atas[0]='*';
				}
				
			}
			
			}else if(cursor_position==16){
			if(plant[1]==false){
				if(sun_value>=plant_cost){
					sun_value -= plant_cost;
					plant[1] = true;
					gfx_mono_draw_char('P',6,cursor_position,&sysfont);
				}
			}
			else{
				if(sun_value>=2){
					sun_value -= 2;
					tembak_tengah[0]='*';
				}
				
			}
			
			}else{
			if(plant[2]==false){
				if(sun_value>=plant_cost){
					sun_value -= plant_cost;
					plant[2] = true;
					gfx_mono_draw_char('P',6,cursor_position,&sysfont);
				}
			}
			else{
				if(sun_value>=2){
					sun_value -= 2;
					tembak_bawah[0]='*';
				}
			}
		}
	}
}

void sunBurst(void){
	for(i=0;i<19;++i){
		tembak_atas[i] = ' ';
		tembak_tengah[i] = ' ';
		tembak_bawah[i] = ' ';
	}
	for(i=0;i<20;++i){
		if(atas[i+j]=='@'){
			atas[i+j] = ' ';
			zombie--;
			score += 2;
			}else if(tengah[i+j]=='@'){
			tengah[i+j] = ' ';
			zombie--;
			score += 2;
			}else if(bawah[i+j]=='@'){
			bawah[i+j] = ' ';
			zombie--;
			score += 2;
		}
	}
}

void tampilkanPeta(void){
	for(i=0;i<20;++i){
		if(atas[i+j]=='@'){
			if(i==0){
				if(plant[0]){
					plant[0] = false;
					gfx_mono_draw_filled_rect(i*6+6,8,12,7,GFX_PIXEL_CLR);
					atas[i+j] = ' ';
				}else lose = true;
				}else{
				gfx_mono_draw_filled_rect(i*6+12,8,6,7,GFX_PIXEL_CLR);
				if(tembak_atas[i+1]=='*'){
					tembak_atas[i+1]=' ';
					atas[i+j] = ' ';
					zombie--;
					score++;
				}
				gfx_mono_draw_char(atas[i+j],i*6+6,8,&sysfont);
			}
		}
		
		if(tengah[i+j]=='@'){
			if(i==0){
				if(plant[1]){
					plant[1] = false;
					gfx_mono_draw_filled_rect(i*6+6,16,12,7,GFX_PIXEL_CLR);
					tengah[i+j] = ' ';
				}else lose = true;
				}else{
				gfx_mono_draw_filled_rect(i*6+12,16,6,7,GFX_PIXEL_CLR);
				if(tembak_tengah[i+1]=='*'){
					tembak_tengah[i+1]=' ';
					tengah[i+j] = ' ';
					zombie--;
					score++;
				}
				gfx_mono_draw_char(tengah[i+j],i*6+6,16,&sysfont);
			}
		}
		if(bawah[i+j]=='@'){
			if(i==0){
				if(plant[2]){
					plant[2] = false;
					gfx_mono_draw_filled_rect(i*6+6,24,12,7,GFX_PIXEL_CLR);
					bawah[i+j] = ' ';
				}else lose = true;
				}else{
				gfx_mono_draw_filled_rect(i*6+12,24,5,7,GFX_PIXEL_CLR);
				if(tembak_bawah[i+1]=='*'){
					tembak_bawah[i+1]=' ';
					bawah[i+j]=' ';
					zombie--;
					score++;
				}
				gfx_mono_draw_char(bawah[i+j],i*6+6,24,&sysfont);
			}
		}
	}
}

void tampilkanTembak(void){
	for(i=0;i<19;++i){
		
		if(i==18){
			if(tembak_atas[i+1]=='*')
			gfx_mono_draw_filled_rect(i*6+12,8,10,7,GFX_PIXEL_CLR);
			if(tembak_tengah[i+1]=='*')
			gfx_mono_draw_filled_rect(i*6+12,16,10,7,GFX_PIXEL_CLR);
			if(tembak_bawah[i+1]=='*')
			gfx_mono_draw_filled_rect(i*6+12,24,10,7,GFX_PIXEL_CLR);
		}
		
		if(tembak_atas[i]=='*')
		{
			if(atas[i+j] =='@'){
				tembak_atas[i]=' ';
				atas[i+j] = ' ';
				zombie--;
				score++;
				gfx_mono_draw_filled_rect(i*6+6,8,12,7,GFX_PIXEL_CLR);
				}else{
				if(i>0){
					gfx_mono_draw_filled_rect(i*6+6,8,6,7,GFX_PIXEL_CLR);
				}
				
				gfx_mono_draw_char(tembak_atas[i],i*6+12,8,&sysfont);
			}
		}
		
		if(tembak_tengah[i]=='*')
		{
			if(tengah[i+j] =='@'){
				tembak_tengah[i]=' ';
				tengah[i+j] = ' ';
				zombie--;
				score++;
				gfx_mono_draw_filled_rect(i*6+6,16,12,7,GFX_PIXEL_CLR);
				}else {
				if(i>0){
					gfx_mono_draw_filled_rect(i*6+6,16,6,7,GFX_PIXEL_CLR);
				}
				gfx_mono_draw_char(tembak_tengah[i],i*6+12,16,&sysfont);
			}
		}
		
		if(tembak_bawah[i]=='*')
		{
			if(bawah[i+j] =='@'){
				tembak_bawah[i]=' ';
				bawah[i+j] = ' ';
				zombie--;
				score++;
				gfx_mono_draw_filled_rect(i*6+6,24,12,7,GFX_PIXEL_CLR);
				}else{
				if(i>0){
					gfx_mono_draw_filled_rect(i*6+6,24,6,7,GFX_PIXEL_CLR);
				}
				gfx_mono_draw_char(tembak_bawah[i],i*6+12,24,&sysfont);
			}
		}
	}
	for(i=19;i>0;i--){
		tembak_atas[i] =  tembak_atas[i-1];
		tembak_tengah[i] = tembak_tengah[i-1];
		tembak_bawah[i] = tembak_bawah[i-1];
	}
	tembak_atas[0] = ' ';
	tembak_tengah[0] = ' ';
	tembak_bawah[0] = ' ';
}

void sun_count(void)
{
	sun_value++;
	
	char* sun_val_string = NULL;
	sprintf(sun_val_string, "%4d", sun_value);
	gfx_mono_draw_filled_rect(25, 0, 24, 8, GFX_PIXEL_CLR);
	gfx_mono_draw_string(sun_val_string, 25, 0, &sysfont);
}

void randomPeta(void){
	rn = eeprom_read_byte((uint8_t*)46);
	++rn;
	srand(rn);
	rn=rand();
	eeprom_write_byte((uint8_t*)46,rn);

	for(int i=0;i<10;++i){
		if(rand()%2){
			if(rand()%2)
			atas[i*5+25] = '@';
			else tengah[i*5+25] = '@';
		}
		else{
			if(rand()%2)
			bawah[i*5+25] = '@';
			else tengah[i*5+25] = '@';
		}
	}
}

void led_start(void)
{
	
	LED_On(LED0);
	_delay_ms(250);
	LED_Off(LED0);
	LED_On(LED1);
	_delay_ms(250);
	LED_Off(LED1);
	LED_On(LED2);
	_delay_ms(250);
	LED_Off(LED2);
	LED_On(LED3);
	_delay_ms(250);
	LED_Off(LED3);
	
	for(int t = 0; t < 2; t++){
		LED_On(LED0);
		LED_On(LED2);
		_delay_ms(250);
		LED_On(LED1);
		LED_On(LED3);
		LED_Off(LED0);
		LED_Off(LED2);
		_delay_ms(250);
		LED_Off(LED1);
		LED_Off(LED3);
	}
}


void start_game(void)
{
	char* string1 = "Plant Shooter";
	char* string2 = "ESAT vs Zombies";
	uint8_t string_length = 0;

	gfx_mono_draw_string(string1, (128-strlen(string1)*6)/2, 12, &sysfont);
	string_length = strlen(string1)*6;

	led_start();

	gfx_mono_draw_filled_rect((128-string_length)/2, 12, string_length, 8, GFX_PIXEL_CLR);
	gfx_mono_draw_string(string2, (128-strlen(string2)*6)/2, 12, &sysfont);
	string_length = strlen(string2)*6;

	led_start();

	gfx_mono_draw_filled_rect((128-string_length)/2, 12, string_length, 8, GFX_PIXEL_CLR);
	gfx_mono_draw_string(string1, (128-strlen(string1)*6)/2, 7, &sysfont);
	gfx_mono_draw_string(string2, (128-strlen(string2)*6)/2, 18, &sysfont);
	
	led_start();
	
	gfx_mono_draw_filled_rect(0, 0, 128, 32, GFX_PIXEL_CLR);
	
	state = 2;
}


