/***************************************************************************
 *
 * Authors:    Amaya Jimenez      ajimenez@cnb.csic.es (2002)
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
 *  e-mail address 'xmipp@cnb.csic.es'
 ***************************************************************************/
#ifndef _PROG_GPU_ROTATE_IMAGE
#define _PROG_GPU_ROTATE_IMAGE

#include <data/xmipp_program.h>
#include <data/xmipp_image.h>
#include <data/xmipp_program.h>
#include <data/transformations.h>
#include <data/args.h>
#include <data/matrix2d.h>

#include <stdio.h>
#include <time.h>
#include <sys/time.h>

#include "../reconstruction_cuda/cuda_gpu_rotate_image_bspline.h"
#include "../reconstruction_cuda/cuda_gpu_rotate_image_linear_nearest.h"
#include "../reconstruction_cuda/cuda_xmipp_utils.h"

#define CUDA_NEAREST 0 //xmipp NEAREST
#define CUDA_LINEAR  1 //xmipp linear
#define CUDA_BSPLINE 2 //xmipp BSPLINE3

#define CUDA_BORDER 3 //cudaAddressModeBorder =  3
#define CUDA_CLAMP  1 //cudaAddressModeClamp =  1
#define CUDA_WRAP 0 //cudaAddressModeWrap =  0

#define USE_MD 0
#define USE_COMMAND 1
#define USE_BOTH 2

#define TIME



void transformationMatrixCuda(const MDRow &imageGeo, Matrix2D<double> &A, bool isVol);

//CUDA functions
void cuda_rotate_image(float *image, float *rotated_image, size_t Xdim,
		size_t Ydim, size_t Zdim, double* ang, int interp, int wrap, int first_call, struct ioTime* times);


class ProgGpuRotateImage: public XmippMetadataProgram
{
private:
    /** Filename angle doc 1 */
	//FileName fnRef;
    //FileName fnOut;
    int splineDegree, dim, wrapMode, use_md;
    Matrix2D<double> R, A;
    bool isVol, isMd, flip;
    int first_call;

public:
    /// Read argument from command line
    void readParams();

    /// Show
    //void show();

    /// Define parameters
    void defineParams();

    void preProcess();

    /** processImage */
    void processImage(const FileName &fnImg, const FileName &fnImgOut, const MDRow &rowIn, MDRow &rowOut);

};
//@}
#endif
