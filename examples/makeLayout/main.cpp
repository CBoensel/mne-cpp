//=============================================================================================================
/**
* @file     main.cpp
* @author   Lorenz Esc <Lorenz.Esch@tu-ilmenau.de>;
*           Matti Hamalainen <msh@nmr.mgh.harvard.edu>
* @version  1.0
* @date     January, 2015
*
* @section  LICENSE
*
* Copyright (C) 2015, Lorenz Esch and Matti Hamalainen. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that
* the following conditions are met:
*     * Redistributions of source code must retain the above copyright notice, this list of conditions and the
*       following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
*       the following disclaimer in the documentation and/or other materials provided with the distribution.
*     * Neither the name of MNE-CPP authors nor the names of its contributors may be used
*       to endorse or promote products derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
* PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
*
* @brief    Builds example for making a 2D layout from 3D points
*
*/

//*************************************************************************************************************
//=============================================================================================================
// INCLUDES
//=============================================================================================================

#include <iostream>
#include <vector>
#include <math.h>


#include <utils/layoutmaker.h>
#include <fiff/fiff.h>
#include <mne/mne.h>


//*************************************************************************************************************
//=============================================================================================================
// QT INCLUDES
//=============================================================================================================

#include <QtCore/QCoreApplication>


//*************************************************************************************************************
//=============================================================================================================
// USED NAMESPACES
//=============================================================================================================

using namespace FIFFLIB;
using namespace MNELIB;


//*************************************************************************************************************
//=============================================================================================================
// MAIN
//=============================================================================================================

//=============================================================================================================
/**
* The function main marks the entry point of the program.
* By default, main has the storage class extern.
*
* @param [in] argc (argument count) is an integer that indicates how many arguments were entered on the command line when the program was started.
* @param [in] argv (argument vector) is an array of pointers to arrays of character objects. The array objects are null-terminated strings, representing the arguments that were entered on the command line when the program was started.
* @return the value that was set to exit() (which is 0 if exit() is called via quit()).
*/
int main(int argc, char *argv[])
{
    //
    // Please notice that this example only works in release mode.
    // Debug mode somehow corrupts the simplex coder. ToDo: Fix this!
    //
    QCoreApplication a(argc, argv);

    // Get fiff info
    QFile t_fileRaw("./MNE-sample-data/baby_meg/150401_121310_TestSubject_raw.fif");

    FiffRawData raw(t_fileRaw);

    FiffInfo fiffInfo = raw.info;

    //get 3d locations from fiff file
    QList<QVector<double> > inputPoints;
    QList<QVector<double> > outputPoints;
    QStringList names;
    QFile out("./MNE_Browse_Raw_Resources/Templates/Layouts/babymeg-mag-inner-layer-x-mirrored.lout");
    QFile out_file_3d("./MNE_Browse_Raw_Resources/Templates/Layouts/3D_points_all.lout");
    QTextStream out3D(&out_file_3d);

    if (!out_file_3d.open(QIODevice::WriteOnly)) {
        std::cout<<"could not open out_file_3d file";
        qDebug()<<"could not open out_file_3d file";
    }

    for(int i = 0; i<fiffInfo.ch_names.size(); i++) {
        int type = fiffInfo.chs.at(i).coil_type;

        if(type == FIFFV_COIL_BABY_MAG) { //FIFFV_MEG_CH FIFFV_EEG_CH FIFFV_REF_MEG_CH FIFFV_COIL_BABY_REF_MAG FIFFV_COIL_BABY_MAG
            QVector<double> temp;
            double x = fiffInfo.chs.at(i).loc(0,0) * 100;
            double y = fiffInfo.chs.at(i).loc(1,0) * 100;
            double z = fiffInfo.chs.at(i).loc(2,0) * 100;

            temp.append(x);
            temp.append(y);
            temp.append(-z);
            inputPoints.append(temp);

            std::cout << x << " " << y << " " << z <<std::endl;

            out3D << i+1 << " " << fiffInfo.ch_names.at(i) << " " << x << " " << y << " " << z << endl;
            names<<fiffInfo.ch_names.at(i);
        }
    }

    std::cout<<"could not open out_file_3d file";
    out_file_3d.close();

    float prad = 60.0;
    float width = 5.0;
    float height = 4.0;

    // convert 3d points to layout
    if(inputPoints.size()>0)
        LayoutMaker::makeLayout(inputPoints,
                                outputPoints,
                                names,
                                out,
                                true,
                                prad,
                                width,
                                height,
                                true,
                                true,
                                true);

    return a.exec();
}
