FILENAME = testio
LIBEMB   = y

${FILENAME}.elf: ${FILENAME}.c
ifdef LIBEMB
	msp430-gcc -mmcu=msp430g2553 -o ${FILENAME}.elf ${FILENAME}.c -lserial -lconio -lshell
else
	msp430-gcc -mmcu=msp430g2553 -o ${FILENAME}.elf ${FILENAME}.c
endif

flash: ${FILENAME}.elf
	mspdebug rf2500 'prog ${FILENAME}.elf'

clean:
	rm -rf *.elf

erase:
	mspdebug rf2500 erase
