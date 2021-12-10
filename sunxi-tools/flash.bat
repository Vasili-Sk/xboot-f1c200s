sunxi-fel spl .\..\output/xboot.bin
sunxi-fel -p spiflash-write 0 .\..\output/xboot.bin
sunxi-fel exec 0
PAUSE