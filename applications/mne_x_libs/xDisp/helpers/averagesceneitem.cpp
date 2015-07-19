//=============================================================================================================
/**
* @file     averagesceneitem.cpp
* @author   Lorenz Esch <lorenz.esch@tu-ilmenau.de>;
*           Christoph Dinh <chdinh@nmr.mgh.harvard.edu>;
*           Matti Hamalainen <msh@nmr.mgh.harvard.edu>;
* @version  1.0
* @date     October, 2014
*
* @section  LICENSE
*
* Copyright (C) 2014, Lorenz Esch, Christoph Dinh and Matti Hamalainen. All rights reserved.
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
* @brief    Contains the implementation of the AverageSceneItem class.
*
*/

//*************************************************************************************************************
//=============================================================================================================
// INCLUDES
//=============================================================================================================

#include "averagesceneitem.h"


//*************************************************************************************************************
//=============================================================================================================
// USED NAMESPACES
//=============================================================================================================

using namespace XDISPLIB;
using namespace std;


//*************************************************************************************************************
//=============================================================================================================
// DEFINE MEMBER METHODS
//=============================================================================================================

AverageSceneItem::AverageSceneItem(QString channelName, int channelNumber, QPointF channelPosition, int channelKind, int channelUnit, QColor defaultColors)
: m_sChannelName(channelName)
, m_iChannelNumber(channelNumber)
, m_qpChannelPosition(channelPosition)
, m_iChannelKind(channelKind)
, m_iChannelUnit(channelUnit)
{
}


//*************************************************************************************************************

QRectF AverageSceneItem::boundingRect() const
{
    int height = 80;
    int width = 500;
    return QRectF(-width/2, -height/2, width, height);
}


//*************************************************************************************************************

void AverageSceneItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    //set posistion
    this->setPos(60*m_qpChannelPosition.x(), -60*m_qpChannelPosition.y());

    // Plot channel name
    QStaticText staticElectrodeName = QStaticText(m_sChannelName);
    QSizeF sizeText = staticElectrodeName.size();
    painter->drawStaticText(-15+((30-sizeText.width())/2), -32, staticElectrodeName);

//    //Plot bounding rect / drawing region of this item
//    painter->drawRect(this->boundingRect());

    //Plot average data
    painter->save();
    paintAveragePath(painter);
    painter->restore();
}


//*************************************************************************************************************

void AverageSceneItem::paintAveragePath(QPainter *painter)
{
    //get maximum range of respective channel type (range value in FiffChInfo does not seem to contain a reasonable value)
    float dMaxValue = 1e-9f;

    switch(m_iChannelKind) {
        case FIFFV_MEG_CH: {
            if(m_iChannelUnit == FIFF_UNIT_T_M) { //gradiometers
                dMaxValue = 1e-10f;
                if(m_scaleMap.contains(FIFF_UNIT_T_M))
                    dMaxValue = m_scaleMap[FIFF_UNIT_T_M];
            }
            else if(m_iChannelUnit == FIFF_UNIT_T) //magnitometers
            {
                dMaxValue = 1e-11f;

                if(m_scaleMap.contains(FIFF_UNIT_T))
                    dMaxValue = m_scaleMap[FIFF_UNIT_T];
            }
            break;
        }

        case FIFFV_REF_MEG_CH: {  /*11/04/14 Added by Limin: MEG reference channel */
            dMaxValue = 1e-11f;
            if(m_scaleMap.contains(FIFF_UNIT_T))
                dMaxValue = m_scaleMap[FIFF_UNIT_T];
            break;
        }
        case FIFFV_EEG_CH: {
            dMaxValue = 1e-4f;
            if(m_scaleMap.contains(FIFFV_EEG_CH))
                dMaxValue = m_scaleMap[FIFFV_EEG_CH];
            break;
        }
        case FIFFV_EOG_CH: {
            dMaxValue = 1e-3f;
            if(m_scaleMap.contains(FIFFV_EOG_CH))
                dMaxValue = m_scaleMap[FIFFV_EOG_CH];
            break;
        }
        case FIFFV_STIM_CH: {
            dMaxValue = 5;
            if(m_scaleMap.contains(FIFFV_STIM_CH))
                dMaxValue = m_scaleMap[FIFFV_STIM_CH];
            break;
        }
        case FIFFV_MISC_CH: {
            dMaxValue = 1e-3f;
            if(m_scaleMap.contains(FIFFV_MISC_CH))
                dMaxValue = m_scaleMap[FIFFV_MISC_CH];
            break;
        }
    }

    //Plot averaged data
    QRectF boundingRect = this->boundingRect();
    double dScaleY = (boundingRect.height()*10)/(2*dMaxValue);
    QPointF qSamplePosition;

    //do for all currently stored evoked set data
    for(int dataIndex = 0; dataIndex<m_lAverageData.size(); dataIndex++) {
        //plot data from averaged data m_lAverageData with the calulacted downsample factor
        const double* averageData = m_lAverageData.at(dataIndex).first;
        int totalCols =  m_lAverageData.at(dataIndex).second;

        //Calculate downsampling factor of averaged data in respect to the items width
        int dsFactor;
        totalCols / boundingRect.width()<1 ? dsFactor = 1 : dsFactor = totalCols / boundingRect.width();

        //Create path
        QPainterPath path = QPainterPath(QPointF(boundingRect.x(), boundingRect.y() + boundingRect.height()/2));
        QPen pen;
        pen.setStyle(Qt::SolidLine);
        pen.setColor(Qt::black);
        if(!m_cAverageColors.isEmpty() && !(dataIndex<m_cAverageColors.size()))
            pen.setColor(m_cAverageColors.at(dataIndex));
        pen.setWidthF(2);
        painter->setPen(pen);

        for(int i = 0; i < totalCols && path.elementCount() <= boundingRect.width(); i += dsFactor) {
            //evoked matrix is stored in column major
            double val = (*(averageData+(i*m_iTotalNumberChannels)+m_iChannelNumber) * dScaleY);

            qSamplePosition.setY(-val);
            qSamplePosition.setX(path.currentPosition().x()+1);

            path.lineTo(qSamplePosition);
        }

        painter->drawPath(path);
    }
}










