# ESR meter

![](photo.png)

Yet another ESR meter. More precisely, it measures the impedance at 100 kHz in
assumption it's pure resistive. So this meter is not for laboratory
measurements, but for finding bad electrolytic capacitors (a typical fault in
aged electronics).

- The test signal is 100 kHz 50 mVpp 10 Ω sine wave
- Rechargeable from Micro-USB at 300 mA (Li-Po/Li-ion)
- Auto power-off after 3 minutes of inactivity


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

To compensate test leads (set zero), short the leads and long press the button.
You can't compensate more than 2 Ω — find a better leads or connect capacitors
directly to the binding post. Note that inductance of wires matters at 100 kHz.
If the meter refuse to compensate, it shows `SHIt` (stands for "your leads are
shit").


## License

Unless otherwise noted, this work is licensed under Creative Commons Zero 1.0.

[![CC0 button](https://licensebuttons.net/p/zero/1.0/88x31.png)](http://creativecommons.org/publicdomain/zero/1.0/)
