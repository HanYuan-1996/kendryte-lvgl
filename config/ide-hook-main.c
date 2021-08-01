#include "super-flash.h"
static int has_already_init = 0; // back comp sdk
__attribute__((constructor)) void initialize_kendryte_ide_hook() {
	if (has_already_init) return;
	has_already_init = 1;
	super_flash_main();
}
