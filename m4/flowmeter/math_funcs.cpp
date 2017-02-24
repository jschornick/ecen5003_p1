/*--------------------------------------------------------
ECEN5003 - Project 1, Module 4
  math_funcs.cpp
 --------------------------------------------------------*/

const unsigned short pow10_table[] = {
1220,16588,
1226,16820,
1232,17058,
1238,17301,
1244,17550,
1251,17806,
1257,18068,
1263,18337,
1270,18612,
1276,18894,
1283,19184,
1290,19481,
1296,19786,
1303,20099,
1310,20421,
1317,20751,
1324,21090,
1331,21438,
1338,21796,
1346,22164,
1353,22542,
1360,22931,
1368,23331,
1376,23742,
1383,24166,
1391,24602,
1399,25050,
1407,25513,
1415,25989,
1423,26480,
1431,26985,
1439,27507,
1448,28045,
1456,28600,
1465,29172,
1474,29764,
1483,30374,
1491,31004,
1500,31656,
1510,32329,
1519,33025,
1528,33745,
1538,34490,
1547,35261,
1557,36060,
1567,36886,
1577,37743,
1587,38631,
1597,39552,
1608,40507,
1618,41497,
1629,42526,
1639,43594,
1650,44704,
1661,45858,
1673,47058,
1684,48305,
1696,49604,
1707,50956,
1719,52365,
1731,53833,
1743,55364,
1756,56961,
1768,58628,
1781,60369,
1794,62187,
1807,64089,
0,0
};

// find approximation for 10^x from lookup table
// num = x*1000
int pow10(int num) {
  int i = 0;
  int result=0;
  for(i = 0; pow10_table[i] != 0; i+=2) {
    if ( pow10_table[i] < num) {
      result = pow10_table[i+1];
    }
  }
  return result;
}


// Finds the truncated integer square root of value passed in via r0
//   r0 : input value / return value
//   r1 : low search boundary
//   r2 : high search boundary
//   r3 : new guess
//   r4 : previous guess
//   r5 : square of new_guess
__asm int sqrt(unsigned int x)
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

