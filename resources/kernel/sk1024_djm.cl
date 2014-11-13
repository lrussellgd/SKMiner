/* CoinShield kernel implementation.
 *
 * ==========================(LICENSE BEGIN)============================
 * 
 * Copyright (c) 2014 djm34
 * 
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * ===========================(LICENSE END)=============================
 *
 * @author   djm34
 */

#ifndef SK1024_CL
#define SK1024_CL
//#pragma OPENCL EXTENSION cl_amd_printf : enable
//#if __ENDIAN_LITTLE__

__constant static const int ROT1024[8][8] = 
{
	{ 55, 43, 37, 40, 16, 22, 38, 12 },
    { 25, 25, 46, 13, 14, 13, 52, 57 },
    { 33,  8, 18, 57, 21, 12, 32, 54 },
    { 34, 43, 25, 60, 44,  9, 59, 34 },
    { 28,  7, 47, 48, 51,  9, 35, 41 },
    { 17,  6, 18, 25, 43, 42, 40, 15 },
    { 58,  7, 32, 45, 19, 18,  2, 56 },
    { 47, 49, 27, 58, 37, 48, 53, 56 }
};
__constant static const ulong t12[9] = 
{ 0x80, 
  0x7000000000000000, 
  0x7000000000000080,
  0xd8, 
  0xb000000000000000,
  0xb0000000000000d8,
  0x08,
  0xff00000000000000, 
  0xff00000000000008
};
__constant static const ulong KeccakF1600_RoundConstant[24] = 
{
    0x0000000000000001ull, 0x0000000000008082ull,
    0x800000000000808aull, 0x8000000080008000ull,
    0x000000000000808bull, 0x0000000080000001ull,
    0x8000000080008081ull, 0x8000000000008009ull,
    0x000000000000008aull, 0x0000000000000088ull,
    0x0000000080008009ull, 0x000000008000000aull,
    0x000000008000808bull, 0x800000000000008bull,
    0x8000000000008089ull, 0x8000000000008003ull,
    0x8000000000008002ull, 0x8000000000000080ull,
    0x000000000000800aull, 0x800000008000000aull,
    0x8000000080008081ull, 0x8000000000008080ull,
    0x0000000080000001ull, 0x8000000080008008ull
};




#define ROL2(x, n)        (((x) << (n)) | ((x) >> (64 - (n)))) //faster rotation
//#define ROL2(x,n)         rotate((ulong)x,(ulong)n)

#define Round1024(p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,pA,pB,pC,pD,pE,pF,ROT)  \
{ \
p0 += p1;  \
p1 = ROL2(p1, ROT1024[ROT][0]);  \
p1 ^= p0;   \
p2 += p3;   \
p3 = ROL2(p3, ROT1024[ROT][1]);  \
p3 ^= p2;   \
p4 += p5;   \
p5 = ROL2(p5, ROT1024[ROT][2]);  \
p5 ^= p4;   \
p6 += p7;   \
p7 = ROL2(p7, ROT1024[ROT][3]);  \
p7 ^= p6;   \
p8 += p9;   \
p9 = ROL2(p9, ROT1024[ROT][4]);  \
p9 ^= p8;   \
pA += pB;   \
pB = ROL2(pB, ROT1024[ROT][5]); \
pB ^= pA;   \
pC += pD;   \
pD = ROL2(pD, ROT1024[ROT][6]); \
pD ^= pC;   \
pE += pF;   \
pF = ROL2(pF, ROT1024[ROT][7]); \
pF ^= pE;   \
};

inline void keccak_block(ulong *s) {
	size_t i;
	ulong t[5], u[5], v, w;

	for (i = 0; i < 24; i++) {
		/* theta: c = a[0,i] ^ a[1,i] ^ .. a[4,i] */
		t[0] = s[0] ^ s[5] ^ s[10] ^ s[15] ^ s[20];
		t[1] = s[1] ^ s[6] ^ s[11] ^ s[16] ^ s[21];
		t[2] = s[2] ^ s[7] ^ s[12] ^ s[17] ^ s[22];
		t[3] = s[3] ^ s[8] ^ s[13] ^ s[18] ^ s[23];
		t[4] = s[4] ^ s[9] ^ s[14] ^ s[19] ^ s[24];

		/* theta: d[i] = c[i+4] ^ rotl(c[i+1],1) */
		u[0] = t[4] ^ ROL2(t[1], 1);
		u[1] = t[0] ^ ROL2(t[2], 1);
		u[2] = t[1] ^ ROL2(t[3], 1);
		u[3] = t[2] ^ ROL2(t[4], 1);
		u[4] = t[3] ^ ROL2(t[0], 1);

		/* theta: a[0,i], a[1,i], .. a[4,i] ^= d[i] */
		s[0] ^= u[0]; s[5] ^= u[0]; s[10] ^= u[0]; s[15] ^= u[0]; s[20] ^= u[0];
		s[1] ^= u[1]; s[6] ^= u[1]; s[11] ^= u[1]; s[16] ^= u[1]; s[21] ^= u[1];
		s[2] ^= u[2]; s[7] ^= u[2]; s[12] ^= u[2]; s[17] ^= u[2]; s[22] ^= u[2];
		s[3] ^= u[3]; s[8] ^= u[3]; s[13] ^= u[3]; s[18] ^= u[3]; s[23] ^= u[3];
		s[4] ^= u[4]; s[9] ^= u[4]; s[14] ^= u[4]; s[19] ^= u[4]; s[24] ^= u[4];

		/* rho pi: b[..] = rotl(a[..], ..) */
		v = s[1];
		s[1] = ROL2(s[6], 44);
		s[6] = ROL2(s[9], 20);
		s[9] = ROL2(s[22], 61);
		s[22] = ROL2(s[14], 39);
		s[14] = ROL2(s[20], 18);
		s[20] = ROL2(s[2], 62);
		s[2] = ROL2(s[12], 43);
		s[12] = ROL2(s[13], 25);
		s[13] = ROL2(s[19], 8);
		s[19] = ROL2(s[23], 56);
		s[23] = ROL2(s[15], 41);
		s[15] = ROL2(s[4], 27);
		s[4] = ROL2(s[24], 14);
		s[24] = ROL2(s[21], 2);
		s[21] = ROL2(s[8], 55);
		s[8] = ROL2(s[16], 45);
		s[16] = ROL2(s[5], 36);
		s[5] = ROL2(s[3], 28);
		s[3] = ROL2(s[18], 21);
		s[18] = ROL2(s[17], 15);
		s[17] = ROL2(s[11], 10);
		s[11] = ROL2(s[7], 6);
		s[7] = ROL2(s[10], 3);
		s[10] = ROL2(v, 1);

		/* chi: a[i,j] ^= ~b[i,j+1] & b[i,j+2] */
		v = s[0]; w = s[1]; s[0] ^= (~w) & s[2]; s[1] ^= (~s[2]) & s[3]; s[2] ^= (~s[3]) & s[4]; s[3] ^= (~s[4]) & v; s[4] ^= (~v) & w;
		v = s[5]; w = s[6]; s[5] ^= (~w) & s[7]; s[6] ^= (~s[7]) & s[8]; s[7] ^= (~s[8]) & s[9]; s[8] ^= (~s[9]) & v; s[9] ^= (~v) & w;
		v = s[10]; w = s[11]; s[10] ^= (~w) & s[12]; s[11] ^= (~s[12]) & s[13]; s[12] ^= (~s[13]) & s[14]; s[13] ^= (~s[14]) & v; s[14] ^= (~v) & w;
		v = s[15]; w = s[16]; s[15] ^= (~w) & s[17]; s[16] ^= (~s[17]) & s[18]; s[17] ^= (~s[18]) & s[19]; s[18] ^= (~s[19]) & v; s[19] ^= (~v) & w;
		v = s[20]; w = s[21]; s[20] ^= (~w) & s[22]; s[21] ^= (~s[22]) & s[23]; s[22] ^= (~s[23]) & s[24]; s[23] ^= (~s[24]) & v; s[24] ^= (~v) & w;

		/* iota: a[0,0] ^= round constant */
		s[0] ^= KeccakF1600_RoundConstant[i];
	}
}; 


__attribute__((reqd_work_group_size(WORKSIZE, 1, 1)))
__kernel void sk1024(__global ulong* uMessage, __constant ulong* c_hv, const ulong startNonce, __global ulong* hash, const ulong target)
{

	ulong nonce = startNonce + (ulong)get_global_id(0);
	ulong h[17];
	ulong t[3];
	ulong p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15;
	
	p0 = uMessage[16];
	p1 = uMessage[17];
	p2 = uMessage[18];
	p3 = uMessage[19];
	p4 = uMessage[20];
	p5 = uMessage[21];
	p6 = uMessage[22];
	p7 = uMessage[23];
	p8 = uMessage[24];
	p9 = uMessage[25];

	p10 = nonce;
	
	t[0] = t12[3]; // ptr  
	t[1] = t12[4]; // etype
	t[2] = t12[5];
	

	p0 += c_hv[0];
	p1 += c_hv[1];
	p2 += c_hv[2];
	p3 += c_hv[3];
	p4 += c_hv[4];
	p5 += c_hv[5];
	p6 += c_hv[6];
	p7 += c_hv[7];
	p8 += c_hv[8];
	p9 += c_hv[9];
	p10 += c_hv[10];
	p11 = c_hv[11];
	p12 = c_hv[12];
	p13 = c_hv[13] + t[0];
	p14 = c_hv[14] + t[1];
	p15 = c_hv[15];

	
	for (int i = 1; i < 21; i += 2)
	{
		Round1024(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, 0);
		Round1024(p0, p9, p2, p13, p6, p11, p4, p15, p10, p7, p12, p3, p14, p5, p8, p1, 1);
		Round1024(p0, p7, p2, p5, p4, p3, p6, p1, p12, p15, p14, p13, p8, p11, p10, p9, 2);
		Round1024(p0, p15, p2, p11, p6, p13, p4, p9, p14, p1, p8, p5, p10, p3, p12, p7, 3);

		p0 += c_hv[(i + 0) % 17];
		p1 += c_hv[(i + 1) % 17];
		p2 += c_hv[(i + 2) % 17];
		p3 += c_hv[(i + 3) % 17];
		p4 += c_hv[(i + 4) % 17];
		p5 += c_hv[(i + 5) % 17];
		p6 += c_hv[(i + 6) % 17];
		p7 += c_hv[(i + 7) % 17];
		p8 += c_hv[(i + 8) % 17];
		p9 += c_hv[(i + 9) % 17];
		p10 += c_hv[(i + 10) % 17];
		p11 += c_hv[(i + 11) % 17];
		p12 += c_hv[(i + 12) % 17];
		p13 += c_hv[(i + 13) % 17] + t[(i + 0) % 3];
		p14 += c_hv[(i + 14) % 17] + t[(i + 1) % 3];
		p15 += c_hv[(i + 15) % 17] + (ulong)i;


		Round1024(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, 4);
		Round1024(p0, p9, p2, p13, p6, p11, p4, p15, p10, p7, p12, p3, p14, p5, p8, p1, 5);
		Round1024(p0, p7, p2, p5, p4, p3, p6, p1, p12, p15, p14, p13, p8, p11, p10, p9, 6);
		Round1024(p0, p15, p2, p11, p6, p13, p4, p9, p14, p1, p8, p5, p10, p3, p12, p7, 7);

		p0 += c_hv[(i + 1) % 17];
		p1 += c_hv[(i + 2) % 17];
		p2 += c_hv[(i + 3) % 17];
		p3 += c_hv[(i + 4) % 17];
		p4 += c_hv[(i + 5) % 17];
		p5 += c_hv[(i + 6) % 17];
		p6 += c_hv[(i + 7) % 17];
		p7 += c_hv[(i + 8) % 17];
		p8 += c_hv[(i + 9) % 17];
		p9 += c_hv[(i + 10) % 17];
		p10 += c_hv[(i + 11) % 17];
		p11 += c_hv[(i + 12) % 17];
		p12 += c_hv[(i + 13) % 17];
		p13 += c_hv[(i + 14) % 17] + t[(i + 1) % 3];
		p14 += c_hv[(i + 15) % 17] + t[(i + 2) % 3];
		p15 += c_hv[(i + 16) % 17] + (ulong)(i + 1);
	}
	
	p0 ^= uMessage[16];
	p1 ^= uMessage[17];
	p2 ^= uMessage[18];
	p3 ^= uMessage[19];
	p4 ^= uMessage[20];
	p5 ^= uMessage[21];
	p6 ^= uMessage[22];
	p7 ^= uMessage[23];
	p8 ^= uMessage[24];
	p9 ^= uMessage[25];
	p10 ^= nonce;

	////////////////////////////// round 3 /////////////////////////////////////
	h[0] = p0;
	h[1] = p1;
	h[2] = p2;
	h[3] = p3;
	h[4] = p4;
	h[5] = p5;
	h[6] = p6;
	h[7] = p7;
	h[8] = p8;
	h[9] = p9;
	h[10] = p10;
	h[11] = p11;
	h[12] = p12;
	h[13] = p13;
	h[14] = p14;
	h[15] = p15;
	h[16] = 0x5555555555555555ull;
	for (int i = 0; i<16; i++) { h[16] ^= h[i]; }

	t[0] = t12[6];
	t[1] = t12[7];
	t[2] = t12[8];

	p13 += t[0];  //p13 already equal h[13] 
	p14 += t[1];

	for (int i = 1; i < 21; i += 2)
	{
		Round1024(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, 0);
		Round1024(p0, p9, p2, p13, p6, p11, p4, p15, p10, p7, p12, p3, p14, p5, p8, p1, 1);
		Round1024(p0, p7, p2, p5, p4, p3, p6, p1, p12, p15, p14, p13, p8, p11, p10, p9, 2);
		Round1024(p0, p15, p2, p11, p6, p13, p4, p9, p14, p1, p8, p5, p10, p3, p12, p7, 3);

		p0 += h[(i + 0) % 17];
		p1 += h[(i + 1) % 17];
		p2 += h[(i + 2) % 17];
		p3 += h[(i + 3) % 17];
		p4 += h[(i + 4) % 17];
		p5 += h[(i + 5) % 17];
		p6 += h[(i + 6) % 17];
		p7 += h[(i + 7) % 17];
		p8 += h[(i + 8) % 17];
		p9 += h[(i + 9) % 17];
		p10 += h[(i + 10) % 17];
		p11 += h[(i + 11) % 17];
		p12 += h[(i + 12) % 17];
		p13 += h[(i + 13) % 17] + t[(i + 0) % 3];
		p14 += h[(i + 14) % 17] + t[(i + 1) % 3];
		p15 += h[(i + 15) % 17] + (ulong)(i);

		Round1024(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, 4);
		Round1024(p0, p9, p2, p13, p6, p11, p4, p15, p10, p7, p12, p3, p14, p5, p8, p1, 5);
		Round1024(p0, p7, p2, p5, p4, p3, p6, p1, p12, p15, p14, p13, p8, p11, p10, p9, 6);
		Round1024(p0, p15, p2, p11, p6, p13, p4, p9, p14, p1, p8, p5, p10, p3, p12, p7, 7);

		p0 += h[(i + 1) % 17];
		p1 += h[(i + 2) % 17];
		p2 += h[(i + 3) % 17];
		p3 += h[(i + 4) % 17];
		p4 += h[(i + 5) % 17];
		p5 += h[(i + 6) % 17];
		p6 += h[(i + 7) % 17];
		p7 += h[(i + 8) % 17];
		p8 += h[(i + 9) % 17];
		p9 += h[(i + 10) % 17];
		p10 += h[(i + 11) % 17];
		p11 += h[(i + 12) % 17];
		p12 += h[(i + 13) % 17];
		p13 += h[(i + 14) % 17] + t[(i + 1) % 3];
		p14 += h[(i + 15) % 17] + t[(i + 2) % 3];
		p15 += h[(i + 16) % 17] + (ulong)(i + 1);


	}

	ulong state[25];
	

//	for (int i = 0; i<9; i++) { state[i] = outputHash[i]; }

	state[0] = p0;
	state[1] = p1;
	state[2] = p2;
	state[3] = p3;
	state[4] = p4;
	state[5] = p5;
	state[6] = p6;
	state[7] = p7;
	state[8] = p8;

	for (int i = 9; i<25; i++) { state[i] = 0; }
	
	keccak_block(state);

	state[0] ^= p9;
	state[1] ^= p10;
	state[2] ^= p11;
	state[3] ^= p12;
	state[4] ^= p13;
	state[5] ^= p14;
	state[6] ^= p15;
	state[7] ^= 0x05ull;
	state[8] ^= (ulong)(1ULL << 63);

	keccak_block(state);

	keccak_block(state);

	barrier(CLK_GLOBAL_MEM_FENCE);

	if (state[6] <= target) {hash[0] = nonce;}
		



};

__attribute__((reqd_work_group_size(WORKSIZE, 1, 1)))
__kernel void skeinProcess(__global ulong* uMessage, __global ulong* c_hv, const ulong startNonce, __global ulong* hash)
{

      ulong nonce = startNonce + (ulong)get_global_id(0);
    __global ulong* outputHash = &(hash[get_global_id(0)-get_global_offset(0)]);


// Skein 2 last round
		ulong h[17];
//		printf("coming here skein Process %08x startnonce %d nonce %d\n", c_hv->h4[get_global_id(0) - get_global_offset(0)], startNonce, nonce);
//		printf("coming here skein Process %08x\n", uMessage->h4[get_global_id(0) - get_global_offset(0)]);
        ulong t[3];
		ulong p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13,p14, p15;
		
		p0 = uMessage[16];    
		p1 = uMessage[17];
		p2 = uMessage[18];
		p3 = uMessage[19];
		p4 = uMessage[20];
		p5 = uMessage[21];
		p6 = uMessage[22];
		p7 = uMessage[23];
		p8 = uMessage[24];
		p9 = uMessage[25];
		p10 = nonce; 
				
		t[0] = t12[3]; // ptr  
		t[1] = t12[4]; // etype
		t[2] = t12[5];
		
		p0 += c_hv[0];
		p1 += c_hv[1];
		p2 += c_hv[2];
		p3 += c_hv[3];
		p4 += c_hv[4];
		p5 += c_hv[5];
		p6 += c_hv[6];
		p7 += c_hv[7];
		p8 += c_hv[8];
		p9 += c_hv[9];
		p10 += c_hv[10];
		p11 = c_hv[11];
		p12 = c_hv[12];
		p13 = c_hv[13] +t[0];
		p14 = c_hv[14] +t[1];
		p15 = c_hv[15];

        
		for (int i = 1; i < 21; i += 2)
		{
			Round1024(p0, p1, p2, p3, p4, p5, p6, p7,p8, p9, p10, p11, p12, p13, p14, p15, 0);			
			Round1024(p0, p9, p2, p13, p6, p11, p4, p15,p10, p7, p12, p3, p14, p5, p8, p1, 1);
			Round1024(p0, p7, p2, p5, p4, p3, p6, p1,p12, p15, p14, p13, p8, p11, p10, p9, 2);
			Round1024(p0, p15, p2, p11, p6, p13, p4, p9,p14, p1, p8, p5, p10, p3, p12, p7, 3);

     			p0 += c_hv[(i + 0) % 17];
				p1 += c_hv[(i + 1) % 17];
				p2 += c_hv[(i + 2) % 17];
				p3 += c_hv[(i + 3) % 17];
				p4 += c_hv[(i + 4) % 17];
				p5 += c_hv[(i + 5) % 17];
				p6 += c_hv[(i + 6) % 17];
				p7 += c_hv[(i + 7) % 17];
				p8 += c_hv[(i + 8) % 17];
				p9 += c_hv[(i + 9) % 17];
				p10 += c_hv[(i + 10) % 17];
				p11 += c_hv[(i + 11) % 17];
				p12 += c_hv[(i + 12) % 17];
				p13 += c_hv[(i + 13) % 17] + t[(i + 0) % 3];
				p14 += c_hv[(i + 14) % 17] + t[(i + 1) % 3];
				p15 += c_hv[(i + 15) % 17] + (ulong) i;
				

				Round1024(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, 4);
				Round1024(p0, p9, p2, p13, p6, p11, p4, p15, p10, p7, p12, p3, p14, p5, p8, p1, 5);
				Round1024(p0, p7, p2, p5, p4, p3, p6, p1, p12, p15, p14, p13, p8, p11, p10, p9, 6);
				Round1024(p0, p15, p2, p11, p6, p13, p4, p9, p14, p1, p8, p5, p10, p3, p12, p7, 7);

				p0 += c_hv[(i + 1) % 17];
				p1 += c_hv[(i + 2) % 17];
				p2 += c_hv[(i + 3) % 17];
				p3 += c_hv[(i + 4) % 17];
				p4 += c_hv[(i + 5) % 17];
				p5 += c_hv[(i + 6) % 17];
				p6 += c_hv[(i + 7) % 17];
				p7 += c_hv[(i + 8) % 17];
				p8 += c_hv[(i + 9) % 17];
				p9 += c_hv[(i + 10) % 17];
				p10 += c_hv[(i + 11) % 17];
				p11 += c_hv[(i + 12) % 17];
				p12 += c_hv[(i + 13) % 17];
				p13 += c_hv[(i + 14) % 17] + t[(i + 1) % 3];
				p14 += c_hv[(i + 15) % 17] + t[(i + 2) % 3];
				p15 += c_hv[(i + 16) % 17] + (ulong) (i+1);
                }

		p0 ^= uMessage[16];
		p1 ^= uMessage[17];
		p2 ^= uMessage[18];
		p3 ^= uMessage[19];
		p4 ^= uMessage[20];
		p5 ^= uMessage[21];
		p6 ^= uMessage[22];
		p7 ^= uMessage[23];
		p8 ^= uMessage[24];
		p9 ^= uMessage[25];
		p10 ^= nonce;

////////////////////////////// round 3 /////////////////////////////////////
		h[0] = p0;
		h[1] = p1;
		h[2] = p2;
		h[3] = p3;
		h[4] = p4;
		h[5] = p5;
		h[6] = p6;
		h[7] = p7;
		h[8] = p8;
		h[9] = p9;
		h[10] = p10;
		h[11] = p11;
		h[12] = p12;
		h[13] = p13;
		h[14] = p14;
		h[15] = p15;
		h[16] = 0x5555555555555555;        
		for (int i = 0; i<16; i++) {h[16] ^= h[i];}

		t[0] = t12[6]; 
		t[1] = t12[7]; 
		t[2] = t12[8];

		p13 += t[0];  //p13 already equal h[13] 
		p14 += t[1];

		for (int i = 1; i < 21; i += 2)
		{
			Round1024(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, 0);
			Round1024(p0, p9, p2, p13, p6, p11, p4, p15, p10, p7, p12, p3, p14, p5, p8, p1, 1);
			Round1024(p0, p7, p2, p5, p4, p3, p6, p1, p12, p15, p14, p13, p8, p11, p10, p9, 2);
			Round1024(p0, p15, p2, p11, p6, p13, p4, p9, p14, p1, p8, p5, p10, p3, p12, p7, 3);

			p0 += h[(i + 0) % 17];
			p1 += h[(i + 1) % 17];
			p2 += h[(i + 2) % 17];
			p3 += h[(i + 3) % 17];
			p4 += h[(i + 4) % 17];
			p5 += h[(i + 5) % 17];
			p6 += h[(i + 6) % 17];
			p7 += h[(i + 7) % 17];
			p8 += h[(i + 8) % 17];
			p9 += h[(i + 9) % 17];
			p10 += h[(i + 10) % 17];
			p11 += h[(i + 11) % 17];
			p12 += h[(i + 12) % 17];
			p13 += h[(i + 13) % 17] + t[(i + 0) % 3];
			p14 += h[(i + 14) % 17] + t[(i + 1) % 3];
			p15 += h[(i + 15) % 17] + (ulong)(i);

			Round1024(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, 4);
			Round1024(p0, p9, p2, p13, p6, p11, p4, p15, p10, p7, p12, p3, p14, p5, p8, p1, 5);
			Round1024(p0, p7, p2, p5, p4, p3, p6, p1, p12, p15, p14, p13, p8, p11, p10, p9, 6);
			Round1024(p0, p15, p2, p11, p6, p13, p4, p9, p14, p1, p8, p5, p10, p3, p12, p7, 7);

			p0 += h[(i + 1) % 17];
			p1 += h[(i + 2) % 17];
			p2 += h[(i + 3) % 17];
			p3 += h[(i + 4) % 17];
			p4 += h[(i + 5) % 17];
			p5 += h[(i + 6) % 17];
			p6 += h[(i + 7) % 17];
			p7 += h[(i + 8) % 17];
			p8 += h[(i + 9) % 17];
			p9 += h[(i + 10) % 17];
			p10 += h[(i + 11) % 17];
			p11 += h[(i + 12) % 17];
			p12 += h[(i + 13) % 17];
			p13 += h[(i + 14) % 17] + t[(i + 1) % 3];
			p14 += h[(i + 15) % 17] + t[(i + 2) % 3];
			p15 += h[(i + 16) % 17] + (ulong)(i+1);


		}
		
			outputHash[0] =  p0;
		    outputHash[1] =  p1;
			outputHash[2] =  p2;
			outputHash[3] =  p3;
			outputHash[4] =  p4;
			outputHash[5] =  p5;
			outputHash[6] =  p6;
			outputHash[7] =  p7;
			outputHash[8] =  p8;
			outputHash[9] =  p9;
			outputHash[10] =  p10;
			outputHash[11] =  p11;
			outputHash[12] =  p12;
			outputHash[13] =  p13;
			outputHash[14] =  p14;
			outputHash[15] =  p15;

	


	
	
};
__attribute__((reqd_work_group_size(WORKSIZE, 1, 1)))
__kernel void keccakProcess(__global ulong* hash, __global ulong* output, ulong startNonce, __constant ulong* target)
{

		
      ulong nonce = startNonce + (ulong)get_global_id(0);
    __global ulong* outputHash = &(hash[get_global_id(0)-get_global_offset(0)]);

         ulong state[25];
         ulong comp;
      
	   for (int i = 0; i<9; i++) {state[i]=outputHash[i];}
	   
	   for (int i = 9; i<25; i++) { state[i] = 0;}

	   keccak_block(state);
       
	   for (int i = 0; i<7; i++) { state[i] ^= outputHash[9+i]; }
	   state[7] ^= 0x05;
	   state[8] ^= (ulong)(1ULL << 63);

	   keccak_block(state);
	   keccak_block(state);


	   if (state[6] <= target[15]) { output[0] = nonce; }

};



#endif //SK1024_CL