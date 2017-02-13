/*----------------------------------------------------------------------------
ECEN5003 - Project 1, Module 1 - SQUARE ROOT APPROXIMATION
 ----------------------------------------
Write an assembly code subroutine to approximate the square root of an 
argument using the bisection method. All math is done with integers, so the 
resulting square root will also be an integer
 *----------------------------------------------------------------------------*/


// Finds the truncated integer square root of value passed in via r0
//   r0 : input value / return value
//   r1 : low search boundary
//   r2 : high search boundary
//   r3 : new guess
//   r4 : previous guess
//   r5 : square of new_guess
__asm int my_sqrt(unsigned int x)
{
	// can we do this as skip next instruction?
	CMP r0, #1   ; make sure we handle the edge case so sqrt(1)=1
	BLS exit		 ; just return the input if it is 0 or 1
	
	MOVS r1, #0  ; low search bound starts at 0
	MOVS r2, r0  ; use input as high search bound
	
	PUSH {r3, r4}
	// store r4-r5 on stack?
	MOVS r3, #0  ; init guess
	MOVS r4, r0  ; make last_guess != new_guess
	
loop
	
	CMP r3, r4   ; if our guess was same as previous, we are done
	BEQ endloop
	
	// update guess
	MOVS r4, r3  ; last_guess = new_guess
	ADDS r3, r1, r2  ; guess = low+high
	LSRS r3, #1      ; guess/2
	
	MOVS r5, r3;     ; 
  MULS r5, r5, r5  ; square of guess
	
	CMP r5, r0   ; if guess^2 = input, we are done
	BEQ endloop
	
	// can we use IT?
	BHI toohigh
toolow
	MOVS r1, r3  ; guess to low, make guess new low bound
	B loop  ; can we do a "skip next" instead?
toohigh
  MOVS r2, r3  ; guess to high, make guess new high bound
	B loop

endloop
  MOVS r0, r3  ; return guess
	POP {r3, r4}

exit
	BX lr

}

/*----------------------------------------------------------------------------
 MAIN function
 *----------------------------------------------------------------------------*/
int main(void){
	volatile int r, j=0;
	int i;
  r = my_sqrt(0);     // should be 0
  r = my_sqrt(25);    // should be 5
	r = my_sqrt(133); 	// should be 11
  for (i=0; i<10000; i++){
		r = my_sqrt(i);
    j+=r;
  }
	while(1)
		;
}

