Setup PATH variable in Eclipse to windows build tools and compiler.
Change gdb path in debug window.

To debug:
- Build project without -DNOCLKINIT (remove in makefile)
- Upload firmware to spi flash with disabled SD card to use JTAG
- Reboot CPU and let it initialize all DDR clocks
- Clean project
- Build project with -DNOCLKINIT (add in makefile)
- Run debug "xboot segger", your program will be loaded in RAM