#!/bin/bash

# Generate LD MEMORY command based on FDT memory node.
# See https://sourceware.org/binutils/docs/ld/MEMORY.html for command
# documentation.

set -e

# $1 = dtb name
# $2 = region name
output_region() {
    local dtb="$1"
    local region="$2"
    local attr reg addr len

    attr=$(fdtget ${dtb} /memory/${region} attr)
    reg=($(fdtget ${dtb} /memory/${region} reg))
    addr=${reg[0]}
    len=${reg[1]}

    printf "\t%s (%s) : ORIGIN = %#x, LENGTH = %#x\n" "${region}" "${attr}" "${addr}" "${len}"
}

if (( $# != 1 )); then
    echo "Usage: $0 <dt file>"
    exit $E_BADARGS
fi

dtb="$1"

memory_regions=$(fdtget -l ${dtb} /memory)

echo "/*"
echo " * Automatically generated"
echo " * DO NOT EDIT"
echo " */"
echo

echo "MEMORY {"
for region in ${memory_regions}; do
    output_region ${dtb} ${region}
done
echo "}"
