start	lw	0	1	one
	lw	0	2	two
	jalr	1	2
	lw	0	2	three
	halt
one	.fill	1			// First number
two	.fill	2			// Second number
three	.fill	3
