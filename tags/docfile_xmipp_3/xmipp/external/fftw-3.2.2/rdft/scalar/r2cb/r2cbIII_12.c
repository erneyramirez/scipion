/*
 * Copyright (c) 2003, 2007-8 Matteo Frigo
 * Copyright (c) 2003, 2007-8 Massachusetts Institute of Technology
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

/* This file was automatically generated --- DO NOT EDIT */
/* Generated on Sun Jul 12 06:46:24 EDT 2009 */

#include "codelet-rdft.h"

#ifdef HAVE_FMA

/* Generated by: ../../../genfft/gen_r2cb -fma -reorder-insns -schedule-for-pipeline -compact -variables 4 -pipeline-latency 4 -sign 1 -n 12 -name r2cbIII_12 -dft-III -include r2cbIII.h */

/*
 * This function contains 42 FP additions, 20 FP multiplications,
 * (or, 30 additions, 8 multiplications, 12 fused multiply/add),
 * 37 stack variables, 4 constants, and 24 memory accesses
 */
#include "r2cbIII.h"

static void r2cbIII_12(R *R0, R *R1, R *Cr, R *Ci, stride rs, stride csr, stride csi, INT v, INT ivs, INT ovs)
{
     DK(KP707106781, +0.707106781186547524400844362104849039284835938);
     DK(KP1_414213562, +1.414213562373095048801688724209698078569671875);
     DK(KP1_732050807, +1.732050807568877293527446341505872366942805254);
     DK(KP2_000000000, +2.000000000000000000000000000000000000000000000);
     INT i;
     for (i = v; i > 0; i = i - 1, R0 = R0 + ovs, R1 = R1 + ovs, Cr = Cr + ivs, Ci = Ci + ivs, MAKE_VOLATILE_STRIDE(rs), MAKE_VOLATILE_STRIDE(csr), MAKE_VOLATILE_STRIDE(csi)) {
	  E TE, TD, TF, TG;
	  {
	       E Tx, T6, Te, Tb, T5, Tw, Ts, To, Th, Ti, T9, TA;
	       {
		    E T1, Tq, Tc, Td, T4, T2, T3, T7, T8, Tr;
		    T1 = Cr[WS(csr, 1)];
		    T2 = Cr[WS(csr, 5)];
		    T3 = Cr[WS(csr, 2)];
		    Tq = Ci[WS(csi, 1)];
		    Tc = Ci[WS(csi, 5)];
		    Td = Ci[WS(csi, 2)];
		    T4 = T2 + T3;
		    Tx = T2 - T3;
		    T6 = Cr[WS(csr, 4)];
		    Te = Tc + Td;
		    Tr = Td - Tc;
		    Tb = FNMS(KP2_000000000, T1, T4);
		    T5 = T1 + T4;
		    T7 = Cr[0];
		    Tw = FMA(KP2_000000000, Tq, Tr);
		    Ts = Tq - Tr;
		    T8 = Cr[WS(csr, 3)];
		    To = Ci[WS(csi, 4)];
		    Th = Ci[0];
		    Ti = Ci[WS(csi, 3)];
		    T9 = T7 + T8;
		    TA = T7 - T8;
	       }
	       {
		    E Tl, Tm, Tv, TC;
		    {
			 E Tf, Ty, Tk, TB;
			 {
			      E Tj, Tn, Tg, Ta;
			      Tl = FNMS(KP1_732050807, Te, Tb);
			      Tf = FMA(KP1_732050807, Te, Tb);
			      Tj = Th + Ti;
			      Tn = Ti - Th;
			      Tg = FNMS(KP2_000000000, T6, T9);
			      Ta = T6 + T9;
			      {
				   E Tu, Tt, Tz, Tp;
				   Ty = FMA(KP1_732050807, Tx, Tw);
				   TE = FNMS(KP1_732050807, Tx, Tw);
				   Tz = FMA(KP2_000000000, To, Tn);
				   Tp = Tn - To;
				   Tm = FMA(KP1_732050807, Tj, Tg);
				   Tk = FNMS(KP1_732050807, Tj, Tg);
				   Tu = T5 - Ta;
				   R0[0] = KP2_000000000 * (T5 + Ta);
				   Tt = Tp - Ts;
				   R0[WS(rs, 3)] = KP2_000000000 * (Ts + Tp);
				   Tv = Tk - Tf;
				   TD = FMA(KP1_732050807, TA, Tz);
				   TB = FNMS(KP1_732050807, TA, Tz);
				   R1[WS(rs, 4)] = KP1_414213562 * (Tu + Tt);
				   R1[WS(rs, 1)] = KP1_414213562 * (Tt - Tu);
			      }
			 }
			 R0[WS(rs, 2)] = Tf + Tk;
			 TC = Ty + TB;
			 R0[WS(rs, 5)] = TB - Ty;
		    }
		    R1[WS(rs, 3)] = KP707106781 * (Tv + TC);
		    R1[0] = KP707106781 * (Tv - TC);
		    TF = Tl - Tm;
		    R0[WS(rs, 4)] = -(Tl + Tm);
	       }
	  }
	  R0[WS(rs, 1)] = TD - TE;
	  TG = TE + TD;
	  R1[WS(rs, 5)] = KP707106781 * (TF - TG);
	  R1[WS(rs, 2)] = KP707106781 * (TF + TG);
     }
}

static const kr2c_desc desc = { 12, "r2cbIII_12", {30, 8, 12, 0}, &GENUS };

void X(codelet_r2cbIII_12) (planner *p) {
     X(kr2c_register) (p, r2cbIII_12, &desc);
}

#else				/* HAVE_FMA */

/* Generated by: ../../../genfft/gen_r2cb -compact -variables 4 -pipeline-latency 4 -sign 1 -n 12 -name r2cbIII_12 -dft-III -include r2cbIII.h */

/*
 * This function contains 42 FP additions, 20 FP multiplications,
 * (or, 38 additions, 16 multiplications, 4 fused multiply/add),
 * 25 stack variables, 4 constants, and 24 memory accesses
 */
#include "r2cbIII.h"

static void r2cbIII_12(R *R0, R *R1, R *Cr, R *Ci, stride rs, stride csr, stride csi, INT v, INT ivs, INT ovs)
{
     DK(KP1_414213562, +1.414213562373095048801688724209698078569671875);
     DK(KP2_000000000, +2.000000000000000000000000000000000000000000000);
     DK(KP500000000, +0.500000000000000000000000000000000000000000000);
     DK(KP866025403, +0.866025403784438646763723170752936183471402627);
     INT i;
     for (i = v; i > 0; i = i - 1, R0 = R0 + ovs, R1 = R1 + ovs, Cr = Cr + ivs, Ci = Ci + ivs, MAKE_VOLATILE_STRIDE(rs), MAKE_VOLATILE_STRIDE(csr), MAKE_VOLATILE_STRIDE(csi)) {
	  E T5, Tw, Tb, Te, Tx, Ts, Ta, TA, Tg, Tj, Tz, Tp, Tt, Tu;
	  {
	       E T1, T2, T3, T4;
	       T1 = Cr[WS(csr, 1)];
	       T2 = Cr[WS(csr, 5)];
	       T3 = Cr[WS(csr, 2)];
	       T4 = T2 + T3;
	       T5 = T1 + T4;
	       Tw = KP866025403 * (T2 - T3);
	       Tb = FNMS(KP500000000, T4, T1);
	  }
	  {
	       E Tq, Tc, Td, Tr;
	       Tq = Ci[WS(csi, 1)];
	       Tc = Ci[WS(csi, 5)];
	       Td = Ci[WS(csi, 2)];
	       Tr = Td - Tc;
	       Te = KP866025403 * (Tc + Td);
	       Tx = FMA(KP500000000, Tr, Tq);
	       Ts = Tq - Tr;
	  }
	  {
	       E T6, T7, T8, T9;
	       T6 = Cr[WS(csr, 4)];
	       T7 = Cr[0];
	       T8 = Cr[WS(csr, 3)];
	       T9 = T7 + T8;
	       Ta = T6 + T9;
	       TA = KP866025403 * (T7 - T8);
	       Tg = FNMS(KP500000000, T9, T6);
	  }
	  {
	       E To, Th, Ti, Tn;
	       To = Ci[WS(csi, 4)];
	       Th = Ci[0];
	       Ti = Ci[WS(csi, 3)];
	       Tn = Ti - Th;
	       Tj = KP866025403 * (Th + Ti);
	       Tz = FMA(KP500000000, Tn, To);
	       Tp = Tn - To;
	  }
	  R0[0] = KP2_000000000 * (T5 + Ta);
	  R0[WS(rs, 3)] = KP2_000000000 * (Ts + Tp);
	  Tt = Tp - Ts;
	  Tu = T5 - Ta;
	  R1[WS(rs, 1)] = KP1_414213562 * (Tt - Tu);
	  R1[WS(rs, 4)] = KP1_414213562 * (Tu + Tt);
	  {
	       E Tf, Tk, Tv, Ty, TB, TC;
	       Tf = Tb - Te;
	       Tk = Tg + Tj;
	       Tv = Tf - Tk;
	       Ty = Tw + Tx;
	       TB = Tz - TA;
	       TC = Ty + TB;
	       R0[WS(rs, 2)] = -(KP2_000000000 * (Tf + Tk));
	       R0[WS(rs, 5)] = KP2_000000000 * (TB - Ty);
	       R1[0] = KP1_414213562 * (Tv - TC);
	       R1[WS(rs, 3)] = KP1_414213562 * (Tv + TC);
	  }
	  {
	       E Tl, Tm, TF, TD, TE, TG;
	       Tl = Tb + Te;
	       Tm = Tg - Tj;
	       TF = Tm - Tl;
	       TD = TA + Tz;
	       TE = Tx - Tw;
	       TG = TE + TD;
	       R0[WS(rs, 4)] = KP2_000000000 * (Tl + Tm);
	       R1[WS(rs, 2)] = KP1_414213562 * (TF + TG);
	       R0[WS(rs, 1)] = KP2_000000000 * (TD - TE);
	       R1[WS(rs, 5)] = KP1_414213562 * (TF - TG);
	  }
     }
}

static const kr2c_desc desc = { 12, "r2cbIII_12", {38, 16, 4, 0}, &GENUS };

void X(codelet_r2cbIII_12) (planner *p) {
     X(kr2c_register) (p, r2cbIII_12, &desc);
}

#endif				/* HAVE_FMA */
