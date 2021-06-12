#include <avr/io.h>
#include "gpio.hpp"
#include "common.hpp"
#include "volatile.hpp"

using namespace Gpio;

#ifdef PORTA
static_assert(&DDRA + 1 == &PORTA, "");
static_assert(&PINA + 2 == &PORTA, "");
#endif
#ifdef PORTB
static_assert(&DDRB + 1 == &PORTB, "");
static_assert(&PINB + 2 == &PORTB, "");
#endif
#ifdef PORTC
static_assert(&DDRC + 1 == &PORTC, "");
static_assert(&PINC + 2 == &PORTC, "");
#endif
#ifdef PORTD
static_assert(&DDRD + 1 == &PORTD, "");
static_assert(&PIND + 2 == &PORTD, "");
#endif
#ifdef PORTE
static_assert(&DDRE + 1 == &PORTE, "");
static_assert(&PINE + 2 == &PORTE, "");
#endif
#ifdef PORTF
static_assert(&DDRF + 1 == &PORTF, "");
static_assert(&PINF + 2 == &PORTF, "");
#endif
#ifdef PORTG
static_assert(&DDRG + 1 == &PORTG, "");
static_assert(&PING + 2 == &PORTG, "");
#endif

static constexpr uint8_t *port_reg(Pin pin)
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

static constexpr uint8_t *ddr_reg(uint8_t *port)
{
        return port ? (port - 1) : nullptr;
}

static constexpr uint8_t *portin_reg(uint8_t *port)
{
        return port ? (port - 2) : nullptr;
}

static constexpr uint8_t pin_mask(Pin pin)
{
        return 1 << (pin % 8);
}

void Gpio::set(Pin pin, State state)
{
        memory_barrier();
        uint8_t *const port = port_reg(pin);
        if (port) {
                uint8_t *const ddr = ddr_reg(port);
                const uint8_t mask = pin_mask(pin);
                set_bits(*ddr, mask, state & 0b10);
                set_bits(*port, mask, state & 0b01);
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
        uint8_t *const port = port_reg(pin);
        if (port) {
                *port ^= pin_mask(pin);
        }
        memory_barrier();
}

bool Gpio::read(Pin pin)
{
        memory_barrier();
        uint8_t *const port = port_reg(pin);
        const bool v = port ? *portin_reg(port) & pin_mask(pin) : 0;
        memory_barrier();
        return v;
}
