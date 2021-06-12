/* HP QDSP-6064 bubble display */

#ifndef BUBBLE_HPP_
#define BUBBLE_HPP_

#include <stdint.h>
#include "print.hpp"

class Bubble: public Print {
public:
        Bubble();
        void init();
        void clear();
        void scan();
        void putc(char c) override;
private:
        char buffer[4];
        uint8_t scan_pos;
        uint8_t putc_pos;
};

#endif
