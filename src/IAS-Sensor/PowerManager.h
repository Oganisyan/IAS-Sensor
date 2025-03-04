#ifndef _POWER_MANAGER_H_
#define _POWER_MANAGER_H_
#include <Arduino.h>

class PowerManager {
	gpio_num_t pow_;     // Power LED
	gpio_num_t btn_;     // switch button
	gpio_num_t adc_;     // BAT_ADC
	int vref_;

	PowerManager(gpio_num_t pow, gpio_num_t btn, gpio_num_t adc);
	void init();
	void shutdown();

public:
	static PowerManager* create(gpio_num_t pow, gpio_num_t btn, gpio_num_t adc);
	double get();
	void loop();
	void lowPower();
};

#endif //_POWER_MANAGER_H_
