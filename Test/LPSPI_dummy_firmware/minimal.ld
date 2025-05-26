SECTIONS
{
  . = 0x00000000;
  .text : { *(.vector_table) *(.text*) }
}
ENTRY(_reset_handler)