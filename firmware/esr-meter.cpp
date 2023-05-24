/* Main program */

#include <stdint.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "common.hpp"
#include "delay.hpp"
#include "volatile.hpp"
#include "gpio.hpp"
#include "bubble.hpp"

/* Peripherals are configured for 8 MHz system clock */
static_assert(F_CPU == 8e6, "");

constexpr Gpio::Pin power_hold = Gpio::B2;
constexpr Gpio::Pin button = Gpio::C2;

constexpr Gpio::Pin unused_pins[] = {
	Gpio::C3,
	Gpio::C4,
	Gpio::C5,
};

constexpr uint8_t probe_adc_channel = 0;
constexpr uint8_t fuel_adc_channel = 1;
constexpr uint8_t iref_adc_channel = 14;

struct Flags {
	bool button_pressed: 1;
	bool button_pressed_long: 1;
	bool probe_updated: 1;
	bool probe_adc_enable: 1;
};

static Bubble display;
static Flags flags;
static uint16_t probe;
static uint16_t probe_open;
static uint8_t inactivity_timer = 0;  /* seconds */

/* 1024 Hz general-purpose interrupt */
ISR(TIMER1_COMPA_vect)
{
	static uint16_t cycle = 0;

	/* Poll the button (32 Hz) */
	if (cycle % 32 == 0) {
		constexpr uint8_t long_press = 16;  /* 0.5 s */
		static uint8_t cnt = 0;
		static bool prev = 1;
		if (Gpio::read(button) != prev) {
			if (prev == 0 && cnt < long_press)
				flags.button_pressed = 1;
			prev = !prev;
			cnt = 0;
		}
		else if (prev == 0) {
			if (cnt <= long_press)
				cnt++;
			if (cnt == long_press)
				flags.button_pressed_long = 1;
		}
	}

	/* Measure the probe voltage  */
	if (flags.probe_adc_enable) {
		constexpr uint8_t nr_samples = 64;  /* 16 Hz */
		static uint8_t cnt = 0;
		static uint16_t acc = 0;

		acc += ADC;
		if (++cnt == nr_samples) {
			probe = acc/nr_samples;
			flags.probe_updated = 1;
			acc = 0;
			cnt = 0;
		}

		ADMUX = probe_adc_channel & 15;
		ADCSRA = 1<<ADEN | 1<<ADSC | 1<<ADPS2 | 1<<ADPS1;  /* 125 kHz */
	}

	/* Scan the display (512 Hz) */
	if (cycle % 2 == 0)
		display.scan();

	/* Increase inactivity timer (1 Hz) */
	if (cycle % 1024 == 0)
		inactivity_timer++;

	cycle++;
}

static void power_off()
{
	Gpio::write(power_hold, 0);
	for (;;) {}
}

static uint16_t adc_measure(uint8_t channel)
{
	constexpr uint8_t nr_samples = 64;

	/* Temporarily disable probe measurements in background */
	bool probe_adc_enable = flags.probe_adc_enable;
	flags.probe_adc_enable = 0;

	uint16_t acc = 0;

	/* Averaging */
	for (uint8_t i = 0; i < nr_samples; i++) {
		ADMUX = channel & 15;
		ADCSRA = 1<<ADEN | 1<<ADSC | 1<<ADPS2 | 1<<ADPS1;  /* 125 kHz */
		while (ADCSRA & (1<<ADSC))
			memory_barrier();
		acc += ADC;
	}

	/* Restore the flag */
	flags.probe_adc_enable = probe_adc_enable;

	return acc/nr_samples;
}

static void self_test()
{
	/* Check the display and skip transients after power on */
	display.printf(PSTR("\n8.8.8.8."));
	delay_ms(500);

	/* Check internal (1.1 V) vs external (1.24 V) references */
	constexpr uint16_t iref_min = 1.1 * 1024/1.24 * 0.9;
	constexpr uint16_t iref_max = 1.1 * 1024/1.24 * 1.1;
	uint16_t iref = adc_measure(iref_adc_channel);
	if (iref < iref_min || iref > iref_max) {
		display.printf(PSTR("\nE.rEF"));
		delay_s(1);
		power_off();
	}

	/* Check the battery voltage */
	constexpr uint16_t fuel_min = 3.3/(31.6+10)*10 * 1024/1.24;
	constexpr uint16_t fuel_max = 4.2/(31.6+10)*10 * 1024/1.24;
	uint16_t fuel = adc_measure(fuel_adc_channel);
	if (fuel < fuel_min) {
		display.printf(PSTR("\nE.F.LO"));
		delay_s(1);
		power_off();
	}

	/* Check and save the open probe voltage */
	constexpr uint16_t probe_min = 25e-3 * 45.3 * 1024/1.24 * 0.8;
	constexpr uint16_t probe_max = 1023;
	probe_open = adc_measure(probe_adc_channel);
	if (probe_open < probe_min || probe_open == probe_max) {
		display.printf(PSTR("\nE.Prb"));
		delay_s(1);
		power_off();
	}

	/* Show the battery level in % */
	uint8_t fuel_percents = (fuel - fuel_min) * 100 / (fuel_max - fuel_min);
	display.printf(PSTR("\nF%3u"), fuel_percents);
	delay_ms(500);
}

/* Return ESR in Ω/100 */
static int16_t calc_esr(uint16_t v_loaded, uint16_t v_open, uint16_t r_zero)
{
	constexpr int32_t r_out = 499.0*10.0/(499.0+10.0) * 100;

	int32_t vl = v_loaded, vo = v_open, rz = r_zero;
	int32_t r = (vl >= vo) ? INT32_MAX : r_out * vl / (vo - vl) - rz;

	return min<int32_t>(r, INT16_MAX);
}

int main()
{
	Gpio::write(power_hold, 1);

	for (auto p : unused_pins)
		Gpio::set(p, Gpio::pull_up);
	
	/* Disable unused peripherals */
	ACSR = 1<<ACD;
	DIDR0 = 0b111011;
	DIDR1 = 0b11;
	PRR = 1<<PRTWI | 1<<PRTIM2 | 1<<PRTIM0 | 1<<PRSPI | 1<<PRUSART0;

	/* Set up T/C1 for 1024 Hz interrupt */
	TCCR1A = 0;
	TCCR1B = 1<<WGM12 | 1<<CS11 | 1<<CS10;
	OCR1A = 121;
	TIMSK1 = 1<<OCIE1A;

	display.init();
	sei();

	self_test();

	flags = {};
	flags.probe_adc_enable = 1;

	int16_t zero = 0;

	for (;;)
	{
		if (flags.button_pressed || inactivity_timer > 180)
			power_off();

		if (flags.button_pressed_long) {
			int16_t z = calc_esr(atomic_read(probe), probe_open, 0);
			if (z > 200)
				display.printf(PSTR("\nSHIt"));
			else {
				zero = z;
				display.printf(PSTR("\nZErO"));
			}
			inactivity_timer = 0;
			delay_ms(500);
			flags.button_pressed_long = 0;
		}

		if (flags.probe_updated) {
			int16_t r = calc_esr(atomic_read(probe), probe_open, zero);
			if (r > 9999)
				display.printf(PSTR("\n-OL-"));
			else {
				display.printf(PSTR("\n%4.3d\b\b."), r);
				inactivity_timer = 0;
			}
			flags.probe_updated = 0;
		}

		memory_barrier();
	}

	return 0;
}
