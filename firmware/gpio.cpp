#include <avr/io.h>
#include "gpio.hpp"
#include "common.hpp"
#include "volatile.hpp"

using namespace Gpio;

static constexpr uint8_t *PORT(Pin pin)
{
	switch (pin/8) {
#ifdef PORTA
	case 0: return const_cast<uint8_t*>(&PORTA);
#endif
#ifdef PORTB
	case 1: return const_cast<uint8_t*>(&PORTB);
#endif
#ifdef PORTC
	case 2: return const_cast<uint8_t*>(&PORTC);
#endif
#ifdef PORTD
	case 3: return const_cast<uint8_t*>(&PORTD);
#endif
#ifdef PORTE
	case 4: return const_cast<uint8_t*>(&PORTE);
#endif
#ifdef PORTF
	case 5: return const_cast<uint8_t*>(&PORTF);
#endif
#ifdef PORTG
	case 6: return const_cast<uint8_t*>(&PORTG);
#endif
	default: return nullptr;
	}
}

static constexpr uint8_t *DDR(uint8_t *port)
{
	return port ? (port - 1) : nullptr;
}

static constexpr uint8_t *PIN(uint8_t *port)
{
	return port ? (port - 2) : nullptr;
}

static constexpr uint8_t mask(Pin pin)
{
	return 1 << (pin % 8);
}

void Gpio::set(Pin pin, State state)
{
	memory_barrier();
	uint8_t *const port = PORT(pin);
	if (port) {
		uint8_t *const ddr = DDR(port);
		const uint8_t m = mask(pin);
		if (state & 0b10) {
			set_bits(*port, m, state & 0b01);
			memory_barrier();
			set_bits(*ddr, m, 1);
		} else {
			set_bits(*ddr, m, 0);
			memory_barrier();
			set_bits(*port, m, state & 0b01);
		}
	}
	memory_barrier();
}

void Gpio::write(Pin pin, bool val)
{
	set(pin, val ? high : low);
}

void Gpio::toggle(Pin pin)
{
	memory_barrier();
	uint8_t *const port = PORT(pin);
	if (port) {
		*port ^= mask(pin);
	}
	memory_barrier();
}

bool Gpio::read(Pin pin)
{
	memory_barrier();
	uint8_t *const port = PORT(pin);
	const bool v = port ? *PIN(port) & mask(pin) : 0;
	memory_barrier();
	return v;
}
