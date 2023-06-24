#include <avr/io.h>
#include "gpio.hpp"
#include "common.hpp"
#include "volatile.hpp"

using namespace Gpio;

static constexpr uint8_t *PORTx(Pin p)
{
	switch (p/8) {
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

static constexpr uint8_t *DDRx(uint8_t *portx)
{
	return portx ? (portx - 1) : nullptr;
}

static constexpr uint8_t *PINx(uint8_t *portx)
{
	return portx ? (portx - 2) : nullptr;
}

static constexpr uint8_t mask(Pin p)
{
	return 1 << (p % 8);
}

bool Gpio::read(Pin p)
{
	uint8_t *const portx = PORTx(p);
	memory_barrier();
	const bool v = portx ? *PINx(portx) & mask(p) : 0;
	memory_barrier();
	return v;
}

void Gpio::write(Pin p, bool v)
{
	uint8_t *const portx = PORTx(p);
	if (portx) {
		uint8_t *const ddrx = DDRx(portx);
		const uint8_t m = mask(p);
		memory_barrier();
		set_bits(*portx, m, v);
		memory_barrier();
		set_bits(*ddrx, m, 1);
		memory_barrier();
	}
}

void Gpio::tri(Pin p)
{
	uint8_t *const portx = PORTx(p);
	if (portx) {
		uint8_t *const ddrx = DDRx(portx);
		const uint8_t m = mask(p);
		memory_barrier();
		set_bits(*ddrx, m, 0);
		memory_barrier();
		set_bits(*portx, m, 0);
		memory_barrier();
	}
}

void Gpio::pull_up(Pin p)
{
	uint8_t *const portx = PORTx(p);
	if (portx) {
		uint8_t *const ddrx = DDRx(portx);
		const uint8_t m = mask(p);
		memory_barrier();
		set_bits(*ddrx, m, 0);
		memory_barrier();
		set_bits(*portx, m, 1);
		memory_barrier();
	}
}
