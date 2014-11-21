.section .text

.thumb
.global fileOpen
.type fileOpen STT_FUNC
fileOpen:
	push {r4, lr}
	ldr r4, =0x0805CF05
	blx r4
	pop {r4, pc}
.pool


.thumb
.global fileClose
.type fileClose STT_FUNC
fileClose:
	push {r4, lr}
	ldr r4, =0x0805CFC5
	blx r4
	pop {r4, pc}
.pool


.thumb
.global fileRead
.type fileRead STT_FUNC
fileRead:
	push {r4, lr}
	ldr r4, =0x0804E315
	blx r4
	pop {r4, pc}
.pool


.thumb
.global fileWrite
.type fileWrite STT_FUNC
fileWrite:
	push {r4, lr}
	sub sp, sp, #4
	ldr r4, [sp, #12]
	str r4, [sp, #0]
	ldr r4, =0x0805E181
	blx r4
	add sp, sp, #4
	pop {r4, pc}
.pool
