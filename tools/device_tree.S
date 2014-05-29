/*
 * Build device tree source into object file.
 * Build with DEVICE_TREE_SOURCE defined, with quotes
 */
.section .dtb, "wa"
.incbin DEVICE_TREE_SOURCE
