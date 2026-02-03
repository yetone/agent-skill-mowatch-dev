MEMORY
{
    FLASH (rx)      : ORIGIN = 0x10050000, LENGTH = 40K
    RAM (xrw)       : ORIGIN = 0x11001000, LENGTH = 4K
}

ENTRY(app_init)

SECTIONS
{
    .text : ALIGN(4)
    {
        *(.text .text.*)
        *(.rodata .rodata.* .constdata .constdata.*)
        KEEP(*(.eh_frame*))
        *(.glue_7)
        *(.glue_7t)
    } >FLASH

    _sidata = LOADADDR(.data);

    .data : ALIGN(4)
    {
        _sdata = . ;
        *(.data .data.*)
        . = ALIGN(4);
        _edata = . ;
    } >RAM AT>FLASH

    .bss (NOLOAD) : ALIGN(4)
    {
        _sbss = .;
        *(.bss .bss.*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } >RAM

    .keep_section : ALIGN(4)
    {
        *(.keep_section)
        KEEP(*(.keep_section*))
    } >FLASH
}
