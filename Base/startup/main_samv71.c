#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
/* #include "definitions.h" */
#include "config.h"
/* #include "stub.h"                       // Supress unused functions errors */

/* Main entry point, triggered by starter_samv71q21.c and points to c++ main */
int main(void) {
    /* Initialize all modules */

    main_cpp();

    /* Execution should not come here during normal operation */

    return (EXIT_FAILURE);
}
