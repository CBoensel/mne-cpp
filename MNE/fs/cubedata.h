//=============================================================================================================
/**
* @file     cubedata.h
* @author   Carsten Boensel <carsten.boensel@tu-ilmenau.de>;
*           Matti Hamalainen <msh@nmr.mgh.harvard.edu>
*           Bruce Fischl <fischl@nmr.mgh.harvard.edu>
* @version  1.0
* @date     July, 2015
*
* @section  LICENSE
*
* Copyright (C) July, 2015 Carsten Boensel and Matti Hamalainen. All rights reserved.
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
* @brief     CubeData class declaration.
*
*/

#ifndef CUBEDATA_H
#define CUBEDATA_H

//*************************************************************************************************************
//=============================================================================================================
// INCLUDES
//=============================================================================================================

#include "fs_global.h"
#include <Eigen/Core>
#include "slicedata.h"

//*************************************************************************************************************
//=============================================================================================================
// Qt INCLUDES
//=============================================================================================================

#include <QString>
#include <QList>

//*************************************************************************************************************
//=============================================================================================================
// DEFINE NAMESPACE FSLIB
//=============================================================================================================

namespace FSLIB
{

//*************************************************************************************************************

class CubeData
{
public:
    CubeData();
    ~CubeData();
    QList<FSLIB::SliceData> getData();
    void setData(QList<FSLIB::SliceData> slices);

    FSLIB::SliceData getZSlice(quint16 idx);
    void setZSlice(FSLIB::SliceData slice, quint16 idx);

    //Eigen::MatrixXd getXSlice(quint16 idx);
    //Eigen::MatrixXd getYSlice(quint16 idx);

    void setDataAsMatrix(Eigen::MatrixXd slice);
    void setSliceIdx(quint32 idx);
    double getVoxel(int x, int y);

private:
    QList<FSLIB::SliceData> m_cube;

};

} // NAMESPACE

#endif // CUBEDATA_H
