/* OUTPUT_FORMAT("elf64-littlearm", "elf64-littlearm", "elf64-littlearm") */
OUTPUT_ARCH(aarch64)
ENTRY(_start)

MEMORY
{
    ROM (rx) : ORIGIN = 0x02080000, LENGTH = 1M
}

SECTIONS
{
    .text : {
        *(.vector_table)
        *(.text)
        *(.text*)
					/* Todo: check if we need the next 3 lines */
		*(.glue_7)         /* glue arm to thumb code */
		*(.glue_7t)        /* glue thumb to arm code */
		*(.eh_frame)

		KEEP (*(.init)) /* libc ctors */
		KEEP (*(.fini)) /* libc dtors */

        . = ALIGN(8);
     } > ROM


	.ARM.attributes 0 : { *(.ARM.attributes) }
}
