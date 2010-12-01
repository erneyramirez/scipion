/***************************************************************************
 *
 * Authors:     Sjors H.W. Scheres (scheres@cnb.csic.es)
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

#include <data/progs.h>
#include <data/args.h>
#include <data/metadata.h>
#include <data/ctf.h>

#include <data/program.h>

class ProgCtfCreateCtfdat : public XmippProgram
{

public:

    FileName fn_sel, fn_doc, fn_ctf, fn_param, fn_out;
    bool do_mode2;

public:

    void readParams()
    {
        fn_sel = getParam("-i");
        fn_out = getParam("-o");
        do_mode2 = checkParam("--ctfs");
        if (do_mode2)
        {
            fn_ctf = getParam("--ctfs");
        }
        else
        {
            fn_doc = getParam("--doc");
            fn_param = getParam("--param");
        }
    }

    void defineParams()
    {
        addUsageLine("Create CTFdat files from a selfile that contains image selfiles for each micrograph.");
        addUsageLine("Example of use: Sample at MODE 1");
        addUsageLine("   ctf_create_ctfdat -i input.sel --param input.ctfparam --doc defocus.doc");
        addUsageLine("Example of use: Sample at MODE 2");
        addUsageLine("   ctf_create_ctfdat -i imput.sel --ctfs ctfparams.sel");

        addParamsLine("  -i <selfile>           : Input selfile of selfiles for each micrograph");
        addParamsLine("  -o <rootname=\"out\">  : Root name for output files ");
        addParamsLine(" == MODE 1: == ");
        addParamsLine("  [--param <param_file>] : CTFparam file with common parameters ");
        addParamsLine("  [--doc <docfile>]      : Docfile with defocus values for each micrograph ");
        addParamsLine("                         : this file may have either a single column (defocus)");
        addParamsLine("                         : or three columns (defocusU, defocusV, azimuth_angle)");
        addParamsLine(" == MODE 2: == ");
        addParamsLine("  [--ctfs <selfile>]     : Selfile of CTF param files for each micropgrah ");
    }


    void run()
    {
        MetaData SFsel, SFind, SFctf, ctfdat;
        FileName fnsel, fnimg, fnctf;

        SFsel.read(fn_sel);

        if (do_mode2)
        {
            SFctf.read(fn_ctf);
        }
        else
        {
            // Write param files for each micrograph to disc and make an internal SFctf
            CTFDescription ctf;
            MetaData DFdef;
            double defU, defV, azi;
            int ii = 0;
            DFdef.read(fn_doc);
            ctf.read(fn_param);
            ctf.enable_CTF = true;
            ctf.enable_CTFnoise = false;

            if (SFsel.size() != DFdef.size())
                REPORT_ERROR(ERR_MD_OBJECTNUMBER,
                             "Selfile -1 and docfile -doc have unequal number of entries! ");

            FOR_ALL_OBJECTS_IN_METADATA(DFdef)
            {
                ii++;
                DFdef.getValue(MDL_CTF_DEFOCUSU,defU);
                if (!DFdef.getValue(MDL_CTF_DEFOCUSU,defV))
                    defV=defU;
                if (!DFdef.getValue(MDL_CTF_DEFOCUS_ANGLE,azi))
                    azi=0;

                ctf.DeltafU=defU;
                ctf.DeltafV=defV;
                ctf.azimuthal_angle=azi;
                fnctf.compose(fn_out,ii,"ctfparam");
                ctf.write(fnctf);
                SFctf.addObject();
                SFctf.setValue(MDL_CTFMODEL,fnctf);
                std::cerr<<" Saved CTF parameter file "<<fnctf<<" for micrograph number "
                <<ii<<std::endl;
            }
        }

        // For both modes
        if (SFsel.size() != SFctf.size())
            REPORT_ERROR(ERR_MD_OBJECTNUMBER, "Selfiles of options -i and -ctfs have unequal number of entries! ");
        SFctf.firstObject();
        FOR_ALL_OBJECTS_IN_METADATA(SFsel)
        {
            SFsel.getValue(MDL_SELFILE,fnsel);
            SFctf.getValue(MDL_IMAGE,fnctf);
            SFind.read(fnsel);
            FOR_ALL_OBJECTS_IN_METADATA(SFind)
            {
                SFind.getValue(MDL_IMAGE,fnimg);
                ctfdat.addObject();
                ctfdat.setValue(MDL_IMAGE,fnimg);
                ctfdat.setValue(MDL_CTFMODEL,fnctf);
            }
        }

        ctfdat.write(fn_out+".ctfdat");
        std::cerr<<" Saved CTFdat file as "<<fn_out+".ctfdat"<<std::endl;
        std::cerr<< " Done! "<<std::endl;
    }
};


int main(int argc, char **argv)
{

    ProgCtfCreateCtfdat program;
    try
    {
        program.read(argc, argv);
        program.run();
    }
    catch (XmippError xe)
    {
        std::cerr << xe;
        return 1;
    }
    return 0;
}

