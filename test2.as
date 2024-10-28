start	add	0	3	2
load	nor	3	2	0
	lw	0	2	load
	sw	0	2	5
	beq	0	1	2
	jalr	3	0
	noop
done	halt
