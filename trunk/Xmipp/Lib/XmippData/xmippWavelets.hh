/***************************************************************************
 *
 * Authors:     Carlos Oscar S. Sorzano (coss@cnb.uam.es)
 *              Antonio Jose Rodriguez Sanchez (ajr@cnb.uam.es)
 *
 * Unidad de  Bioinformatica of Centro Nacional de Biotecnologia , CSIC
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA            
 * 02111-1307  USA                                                     
 *                                                                     
 *  All comments concerning this program package may be sent to the    
 *  e-mail address 'xmipp@cnb.uam.es'                                  
 ***************************************************************************/
/* ------------------------------------------------------------------------- */
/* WAVELETS                                                                  */
/* ------------------------------------------------------------------------- */
#ifndef _XMIPPWAVELETS_HH
   #define _XMIPPWAVELETS_HH

#include <XmippData/xmippMatrices3D.hh>
#include <XmippData/Src/NumericalRecipes.hh>

/**@name Wavelets */
///@{

#define DAUB4   4
#define DAUB12 12
#define DAUB20 20

/** Set DWT type.
    The DWT type should be set before starting making transforms.
    Valid types are: DAUB4, DAUB12, DAUB20*/
    void set_DWT_type(int DWT_type);

/** DWT of a vector.
    The output vector can be the same as the input one. Previously the
    type of DWT must be set with set_DWT_type. If isign=1 the direct DWT
    is performed, if isign=-1 the inverse DWT is done*/
template <class T>
   void DWT(const matrix1D<T> &v, matrix1D<double> &result, int isign=1);

/** DWT of a array.
    The output array can be the same as the input one. Previously the
    type of DWT must be set with set_DWT_type. If isign=1 the direct DWT
    is performed, if isign=-1 the inverse DWT is done*/
template <class T>
   void DWT(const matrix2D<T> &v, matrix2D<double> &result, int isign=1);

/** DWT of a volume.
    The output vector can be the same as the input one. Previously the
    type of DWT must be set with set_DWT_type. If isign=1 the direct DWT
    is performed, if isign=-1 the inverse DWT is done*/
template <class T>
   void DWT(const matrix3D<T> &v, matrix3D<double> &result, int isign=1);

/** IDWT of a vector.
    The output vector can be the
    same as the input one. Previously the
    type of DWT must be set with set_DWT_type. */
   void IDWT(const matrix1D<double> &v,
   matrix1D<double> &result);

/** IDWT of an array.
    The output vector can be the
    same as the input one. Previously the
    type of DWT must be set with set_DWT_type.  */
   void IDWT(const matrix2D<double> &v,
   matrix2D<double> &result);

/** IDWT of a volume.
    The output volume can be the
    same as the input one. Previously the
    type of DWT must be set with set_DWT_type. */
   void IDWT(const matrix3D<double> &v,
   matrix3D<double> &result);

/** Select Block 1D.
    Given the scale (s=0 is the finest) and the quadrant
    "0"(Lower frequencies) or "1"(Higher frequencies)
    this routine returns the indices that should be explored
    for this block (x1 and x2 should be included in the for). */
    void SelectDWTBlock(int scale, int size_x,
       const string &quadrant,
       int &x1, int &x2);

/** Select Block 2D.
    Given the scale (s=0 is the finest) and the quadrant
    "xy"="00" (Upper left), "01" (Upper right), "10" (Lower left),
    "11" (Lower right).
    this routine returns the indices that should be explored
    for this block (the extremes should be included in the for). */
    void SelectDWTBlock(int scale, int size_x, int size_y,
       const string &quadrant,
       int &x1, int &x2, int &y1, int &y2);

/** Select Block 3D.
    Given the scale (s=0 is the finest) and the quadrant
    "xyz"="000", "001", "010", "011", "100", "101", "110", "111".
    this routine returns the indices that should be explored
    for this block (the extremes should be included in the for). */
    void SelectDWTBlock(int scale,
       int size_x, int size_y, int size_z,
       const string &quadrant,
       int &x1, int &x2, int &y1, int &y2, int &z1, int &z2);

   /** Get scale and quadrant 1D.
       Given a point and the maximum size of the image,
       this routine returns the scale and quadrant it belongs. */
   void Get_Scale_Quadrant(int size_x, int x,
      int &scale, string &quadrant);

   /** Get scale and quadrant 2D.
       Given a point and the maximum size of the image,
       this routine returns the scale and quadrant it belongs. */
   void Get_Scale_Quadrant(int size_x, int size_y, int x, int y,
      int &scale, string &quadrant);

   /** Get scale and quadrant 3D.
       Given a point and the maximum size of the image,
       this routine returns the scale and quadrant it belongs. */
   void Get_Scale_Quadrant(int size_x, int size_y, int size_z,
      int x, int y, int z,
      int &scale, string &quadrant);

   /**@name Denoising */
   ///@{
   /** Remove all information within a quadrant and scale */
   void clean_quadrant(matrix2D<double> &I, int scale, const string &quadrant);

   /** Remove all information within a quadrant and scale */
   void clean_quadrant(matrix3D<double> &I, int scale, const string &quadrant);

   /** Soft thresholding 2D.
       Substract a value from all coefficients, if the the value is
       greater than the absolute value of the coefficient, that coefficient
       is set to 0. */
   void soft_thresholding(matrix2D<double> &I, double th);

   /** Soft thresholding 3D.
       Substract a value from all coefficients, if the the value is
       greater than the absolute value of the coefficient, that coefficient
       is set to 0. */
   void soft_thresholding(matrix3D<double> &I, double th);
   ///@}
///@}
#endif
