#include <avr/io.h>
#include <api/macro_api.h>

const char eeprom[EEPROM_SIZE] EEPROM = "0123456789ABCDEF";

const char usersig[USER_SIGNATURES_SIZE] USERSIG = "0123456789ABCDEF The_quick_brown_fox_jumps_over_the_lazy_dog.";

#ifdef BOOTROW
const char bootsig[BOOTROW_SIZE] BOOTSIG = "The_quick_brown_fox_jumps_over_the_lazy_dog. 0123456789abcdef";
#endif

// end of code
