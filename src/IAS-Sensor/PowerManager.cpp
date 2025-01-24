#include <PowerManager.h>

RTC_DATA_ATTR boolean switchOnOff = true;
RTC_DATA_ATTR int lastBtnState = LOW;


const double NaN = std::numeric_limits<double>::quiet_NaN();

PowerManager::PowerManager(gpio_num_t pow, gpio_num_t btn, gpio_num_t adc) :
		pow_(pow), btn_(btn), adc_(adc), vref_(1100) {
	init();
}

PowerManager* PowerManager::create(gpio_num_t pow, gpio_num_t btn, gpio_num_t adc) {
	return new PowerManager(pow, btn, adc);
}

void PowerManager::init() {
	pinMode(btn_, INPUT);
	loop();
	pinMode(pow_, OUTPUT);
	digitalWrite(pow_, HIGH);
}

void PowerManager::loop() {
	int btnState = digitalRead(btn_);
	if (btnState == LOW && lastBtnState == HIGH) {
		switchOnOff = !switchOnOff;
	}
	lastBtnState = btnState;
	if (!switchOnOff)
		shutdown();
}

void PowerManager::shutdown() {
	esp_sleep_enable_ext0_wakeup(btn_, ESP_EXT1_WAKEUP_ANY_HIGH);
	esp_deep_sleep_start();

}

double PowerManager::get() {
	//return NaN;
	int ADC_VALUE = analogRead(adc_);
	int adc_percentage = int(100 * ADC_VALUE / 4095);
	return (ADC_VALUE * 3.3) / (4095);
}
