#ifndef _POWER_MANAGER_H_
#define _POWER_MANAGER_H_
#include <Arduino.h>

class PowerManager {
	gpio_num_t pow_;     // Power LED
	gpio_num_t btn_;     // switch button
	gpio_num_t pin_;     // BAT_ADC
	int vref_;

	PowerManager(gpio_num_t pow, gpio_num_t btn);
	void init();
	void shutdown();

public:
	static PowerManager* create(gpio_num_t pow, gpio_num_t btn);
	double get();
	void loop();
};

#endif //_POWER_MANAGER_H_
