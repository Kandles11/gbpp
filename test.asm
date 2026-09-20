SECTION "Entry", ROM0[$0000]
Start:
    ld bc, 0xBCBC
    ld de, 0xDEDE
    ld hl, 0xAFAF
    ld sp, 0x4242
    nop

; pad or add more code here