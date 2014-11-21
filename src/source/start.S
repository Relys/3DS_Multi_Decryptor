.section .data
.balign 4
run_once: .word 1

.section .text.start
.arm
.global _start
.type _start, %function

_start:
    @FlushProcessDataCache
    ldr r0, =0xFFFF8001
    ldr r1, =start_addr
    ldr r2, =total_size
    svc 0x54
    
    bl main
loop:
    b loop

.global ExitThread
ExitThread:
	STMFD   SP!, {R3-R12,LR}
	svc 0x09
	LDMFD   SP!, {R3-R12,PC}
