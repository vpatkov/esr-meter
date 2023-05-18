# ESR meter

![](photo.png)

Yet another ESR meter. Like most of such meters, it measures the impedance
at 100 kHz, as if it be pure resistive. So this meter is not for laboratory
measurements, but for finding bad electrolytic capacitors (a typical fault in
aged electronics).

- The test signal is a 100 kHz sine wave
- Open terminals voltage is 50 mVpp (ok for in-circuit measurements, safe
  for polarized caps)
- Short terminals current is 5 mApp
- Rechargeable by Micro-USB (300 mA)
- Auto power-off after 3 minutes of inactivity
- Sexy HP bubble display (it's the main feature)


## Usage

To switch the meter on or off, press the button.

On startup, the meter performs a self-test. Keep the terminals open. During the
test, the meter turns on all the segments on the display, then shows an error
if any. Errors are:

- `E.F.LO`: low fuel (battery voltage < 3.3 V);
- `E.rEF`: something wrong with voltage references or ADC;
- `E.Prb`: something wrong with the test signal (or the terminals aren't open).

When the test is passed, the meter shows the current battery level in percents
as `F <level>`.

During normal operation the meter continuously shows the measured ESR in Ω,
or `-OL-` if it's more than 99.99 Ω.

To compensate test leads resistance (set zero), short the leads and long press
the button. You can't compensate more than 2 Ω — find a better leads or connect
capacitors directly to the binding post. If the meter refuse to compensate, it
shows `SHIt` (stands for "your leads are shit").


## License

Unless otherwise noted, this work is licensed under Creative Commons Zero 1.0.

[![CC0 button](https://licensebuttons.net/p/zero/1.0/88x31.png)](http://creativecommons.org/publicdomain/zero/1.0/)
