MEMORY
{
    flash (rx) : ORIGIN = 0x00000, LENGTH = 248K  
    boot (rx)  : ORIGIN = 0x3E000, LENGTH = 8K   
}

SECTIONS
{
    .text : 
    {
        *(.text)
        *(.text.*)
    } > flash

    .bootloader :
    {
        *(.bootloader)
    } > boot
}