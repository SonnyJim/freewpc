;;; Copyright 2007, 2008, 2009 by Brian Dominy <brian@oddchange.com>
;;;
;;; This file is part of FreeWPC.
;;;
;;; FreeWPC is free software; you can redistribute it and/or modify
;;; it under the terms of the GNU General Public License as published by
;;; the Free Software Foundation; either version 2 of the License, or
;;; (at your option) any later version.
;;;
;;; FreeWPC is distributed in the hope that it will be useful,
;;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;;; GNU General Public License for more details.
;;;
;;; You should have received a copy of the GNU General Public License
;;; along with FreeWPC; if not, write to the Free Software
;;; Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
;;;

DMD_LOW_BASE=0x3800
DMD_PAGE_WIDTH=512

	.module dmd_6809.s

	; Keep loop_count in a fast direct memory slot.
#define loop_count *m0

	.area .text

	;--------------------------------------------------------
	;
	; void dmd_clean_page (void *dst);
	;
	; X = pointer to display page
	;--------------------------------------------------------
	.globl _dmd_clean_page
_dmd_clean_page:
	clrb

	;--------------------------------------------------------
	;
	; void dmd_memset (void *dst, U8 fill);
	;
	; X = pointer to display page
	; B = character to fill
	;--------------------------------------------------------
	.globl _dmd_memset
_dmd_memset:
	pshs	y,u

	leau	DMD_PAGE_WIDTH,x

	lda	#5
	sta	loop_count

	tfr	b,a
	tfr	d,x
	tfr	x,y

	; The core loop consists of 17 pshu instructions,
	; each initializing 6 bytes for a total of 102
	; bytes per iteration.  Over 5 iterations, this
	; initialized 510 of the 512 DMD display bytes.
	; At the end one more pshu is needed to assign
	; the final 2 bytes.
	; TODO - when doing zero operation, we can push
	; DP=0 to get one extra byte moved per instruction.
	; If interrupts are disabled, we can set S=0 before
	; this loop and get 2 more bytes out of it.  There
	; would be a penalty for the setup/restore though.
	; (102 bytes in 17x12=204 cycles, means this takes
	; about 0.5ms to execute.)
1$:
	pshu	a,b,x,y
	pshu	a,b,x,y
	pshu	a,b,x,y
	pshu	a,b,x,y
	pshu	a,b,x,y
	pshu	a,b,x,y
	pshu	a,b,x,y
	pshu	a,b,x,y
	pshu	a,b,x,y
	pshu	a,b,x,y
	pshu	a,b,x,y
	pshu	a,b,x,y
	pshu	a,b,x,y
	pshu	a,b,x,y
	pshu	a,b,x,y
	pshu	a,b,x,y
	pshu	a,b,x,y
	dec	loop_count
	bne	1$
	pshu	x

	puls	y,u,pc


	;--------------------------------------------------------
	;
	; void dmd_copy_asm (void *dst, const void *src);
	;
	; X = pointer to destination buffer
	; top of stack = pointer to source buffer
	;
	;--------------------------------------------------------
	.globl _dmd_copy_asm
_dmd_copy_asm:
	pshs	u,y

   ; Get the source pointer off the stack
	ldu	6,s

	; In each iteration, we will copy 16 bytes.
	lda	#(DMD_PAGE_WIDTH / 16)
	sta	loop_count

1$:
	; Read 4 bytes at a time using a fast pull instruction.
	; Then write them out using 2 word stores.
	; Do all of this 4 times to transfer a total of 16 bytes.
	; U moves automatically throughout; X is adjusted
	; forward once at the end of each block.
	pulu	d,y
	std	,x
	sty	2,x
	pulu	d,y
	std	4,x
	sty	6,x
	pulu	d,y
	std	8,x
	sty	10,x
	pulu	d,y
	std	12,x
	sty	14,x
	leax	16,x
	dec	loop_count
	bne	1$
	puls	u,y,pc


	;--------------------------------------------------------
	;
	; void frame_decode_rle_asm (void *src);
	;
	; X = pointer to compressed source image data
	;
	;--------------------------------------------------------
	.globl _frame_decode_rle_asm
_frame_decode_rle_asm:
	pshs	u

	; Decoding is always done to the low mapped buffer.
	ldu	#DMD_LOW_BASE

rle_loop:
	; The main loop reads 16-bit at a time.  If the upper 8-bits
	; is 0xA8, that indicates a special macro sequence follows.
	; This value was chosen because it is unlikely to appear in
	; actual image data.  Otherwise the data is just copied.
	ldd	,x++
	cmpa	#0xA8
	beq	rle_run
	std	,u++

	ldd	,x++
	cmpa	#0xA8
	beq	rle_run
	std	,u++

	bra	rle_loop

rle_done:
	puls	u,pc

rle_run:
	; Here, a macro sequence has been started (0xA8).  The second
	; byte tells us what to do.
	tstb
	beq	rle_escape        ; Zero = escape character
	bmi	rle_done          ; Any negative value = end-of-image

	; Positive values indicate the run-length count.  The actual
	; byte count must be EVEN.  The value here is that divided by 2;
	; i.e. the number of 16-bit words which follow.  One more byte
	; must be read, which says what the data value is.
	stb	*m0
	lda	,x+
	tfr	a,b

rle_run_loop:
	std	,u++
	dec	*m0
	bne	rle_run_loop
	bra	rle_loop

rle_escape:
	; A negative count means that 0xA8 really was the image data.
	; Read a third byte to see what should follow it.
	; (For example, a word of 0xA8BF in the image data would be
	; encoded as 0xA8 0x80 0xBF.)
	ldb	,x+
	std	,u++
	bra	rle_loop


	;--------------------------------------------------------
	;
	; void frame_decode_sparse_asm (void *src);
	;
	; X = pointer to source image data
	;
	;--------------------------------------------------------
	.globl _frame_decode_sparse_asm
_frame_decode_sparse_asm:
	pshs	u
	tfr	x,u
	ldx	#DMD_LOW_BASE
	jsr	_dmd_clean_page
	tfr	u,x
	ldu	#DMD_LOW_BASE

sparse_loop:
	; First byte is literal data count, in 16-bit words.
	; If zero, this means no more blocks
	lda	,x+
	beq	sparse_done
	sta	*m0

	; Second byte of block is skip count.
	; The cursor is moved forward this many BYTES
	; before copying the literals to memory.
	ldb	,x+
	leau	b,u

	; Remaining bytes of block are the data
sparse_block_loop:
	ldd	,x++
	std	,u++
	dec	*m0
	bne	sparse_block_loop

	; On to the next block
	bra	sparse_loop

sparse_done:
	puls	u,pc


