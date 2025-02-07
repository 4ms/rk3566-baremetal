OUTPUT_ARCH(aarch64)
ENTRY(_start)

MEMORY
{
    ROM (rwx) : ORIGIN = 0x02080000, LENGTH = 223M
    RAM (rwx): ORIGIN = 0x10000000, LENGTH = 512M
}

__HEAP_SIZE = 256M;

SECTIONS
{
    .text : {
		*(.start)
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

    /* .rodata sections (constants, strings, etc.) */
    .rodata :
    {
        . = ALIGN(8);
        *(.rodata)         
        *(.rodata*) 
        . = ALIGN(8);
    } > ROM

    /* used for unwinding (probably not used, but is ignored if your app doens't use exceptions */
    .ARM.extab     : { *(.ARM.extab* .gnu.linkonce.armextab.*) } >ROM
    .ARM : { __exidx_start = .; *(.ARM.exidx*) __exidx_end = .; } >ROM

    .preinit_array :
    {
        . = ALIGN(8);
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP (*(.preinit_array*))
        PROVIDE_HIDDEN (__preinit_array_end = .);
        . = ALIGN(8);
    } > ROM

    .init_array :
    {
        . = ALIGN(8);
        PROVIDE_HIDDEN (_init_array_start = .);
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array*))
        PROVIDE_HIDDEN (_init_array_end = .);
        . = ALIGN(8);
    } > ROM

    .fini_array :
    {
        . = ALIGN(8);
        PROVIDE_HIDDEN (_fini_array_start = .);
        KEEP (*(SORT(.fini_array.*)))
        KEEP (*(.fini_array*))
        PROVIDE_HIDDEN (_fini_array_end = .);
        . = ALIGN(8);
    } > ROM

    .data : 
    {
        . = ALIGN(8);
        _data_start = .;
        *(.data)
        *(.data*)
        . = ALIGN(8);
        _data_end = .;
    } > ROM

    .bss : {
        _bss_start = .;
        *(.bss)
        *(.bss*)   /* required for some malloc calls */
        *(COMMON)  /* required for libc, such as __lock___atexit_recursive_mutex */
        . = ALIGN(8);
        _bss_end = .;
        . = ALIGN(256);
        _ram_aligned_end = .;
    } > RAM

    .heap (NOLOAD):
    {
        . = ALIGN(8);
        _sheap = .;
        . += __HEAP_SIZE;
        _eheap = .;
    } > RAM

    _user_stack_start = _ram_aligned_end;
    _user_stack_end = _user_stack_start + 0x1000000; /* 16 MB */

	/* not used? */
    _svc_stack_start = _user_stack_end;
    _svc_stack_end = _svc_stack_start + 0x6000; /* 24k for ISRs*/

    _irq_stack_start = _svc_stack_end;
    _irq_stack_end = _irq_stack_start + 0x100; 

	/* not used? */
    _fiq_stack_start = _irq_stack_end;
    _fiq_stack_end = _fiq_stack_start + 0x100;

    _core1_user_stack_start = _fiq_stack_end;
    _core1_user_stack_end = _core1_user_stack_start + 0x1000000; /* 16MB */

    _core2_user_stack_start = _core1_user_stack_end;
    _core2_user_stack_end = _core2_user_stack_start + 0x1000000; /* 16MB */

    _core3_user_stack_start = _core2_user_stack_end;
    _core3_user_stack_end = _core3_user_stack_start + 0x1000000; /* 16MB */


  /* Remove information from the compiler libraries */
  /* /DISCARD/ : */
  /* { */
  /*   libc.a ( * ) */
  /*   libm.a ( * ) */
  /*   libgcc.a ( * ) */
  /* } */

    .ARM.attributes 0 : { *(.ARM.attributes) }
}
