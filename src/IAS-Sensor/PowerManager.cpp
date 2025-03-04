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
	pinMode(adc_, INPUT);
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

void PowerManager::lowPower() {
	pinMode(pow_, OUTPUT);
	for(int i=0; i < 10; i++) {
		digitalWrite(pow_, LOW);
		delay(100);
		digitalWrite(pow_, HIGH);
		delay(100);
	}
	shutdown();
}

void PowerManager::shutdown() {
	//pinMode(pow_, OUTPUT);
	//gpio_hold_en(pow_);
	//digitalWrite(pow_, LOW);
	esp_sleep_enable_ext0_wakeup(btn_, ESP_EXT1_WAKEUP_ANY_HIGH);
	esp_deep_sleep_start();
}

double PowerManager::get() {
	double rv =(analogRead(adc_) * 3.3) / (4095) *2;
	return rv;
}
