/* AVR GPIO */

#ifndef GPIO_HPP_
#define GPIO_HPP_

#include <stdint.h>

namespace Gpio
{
	enum Pin: uint8_t {
		A0, A1, A2, A3, A4, A5, A6, A7,
		B0, B1, B2, B3, B4, B5, B6, B7,
		C0, C1, C2, C3, C4, C5, C6, C7,
		D0, D1, D2, D3, D4, D5, D6, D7,
		E0, E1, E2, E3, E4, E5, E6, E7,
		F0, F1, F2, F3, F4, F5, F6, F7,
		G0, G1, G2, G3, G4, G5, G6, G7,
	};

	bool read(Pin p);
	void write(Pin p, bool v);
	void tri(Pin p);
	void pull_up(Pin p);
}

#endif
