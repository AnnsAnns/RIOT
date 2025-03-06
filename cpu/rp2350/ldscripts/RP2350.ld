INCLUDE cortexm.ld

MEMORY
{
    boot_loader (rx)    : ORIGIN = 0x10000000, LENGTH = 0x100
}

SECTIONS {
    .boot_loader :
    {
        __boot_loader_start__ = .;
        KEEP(*(.boot2));
        __boot_loader_end__ = .;
        KEEP(*(.boot2));
    } > boot_loader

    ASSERT(__boot_loader_end__ - __boot_loader_start__ == 256,
           "ERROR: RP2040 second stage bootloader must be 256 bytes in size")
}
