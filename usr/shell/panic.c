#include <kernel/fault.h>
#include "app.h"

void induce_panic(int argc, char **argv) {
    panic_print("Induced panic");
}
DEFINE_APP(induce_panic);
