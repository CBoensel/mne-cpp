//=============================================================================================================
/**
* @file     cluststcworker.cpp
* @author   Christoph Dinh <chdinh@nmr.mgh.harvard.edu>;
*           Matti Hamalainen <msh@nmr.mgh.harvard.edu>;
* @version  1.0
* @date     June, 2014
*
* @section  LICENSE
*
* Copyright (C) 2014, Christoph Dinh and Matti Hamalainen. All rights reserved.
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
* @brief    Implementation of the ClustStcWorker class.
*
*/

//*************************************************************************************************************
//=============================================================================================================
// INCLUDES
//=============================================================================================================

#include "cluststcworker.h"


//*************************************************************************************************************
//=============================================================================================================
// Qt INCLUDES
//=============================================================================================================

#include <QDebug>


//*************************************************************************************************************
//=============================================================================================================
// USED NAMESPACES
//=============================================================================================================

using namespace DISP3DLIB;


//*************************************************************************************************************
//=============================================================================================================
// DEFINE MEMBER METHODS
//=============================================================================================================

ClustStcWorker::ClustStcWorker(QObject *parent)
: QThread(parent)
, m_bIsRunning(false)
, m_bIsLooping(false)
, m_iAverageSamples(10)
, m_iCurrentSample(0)
, m_iUSecIntervall(100)
{
    m_data.clear();
}


//*************************************************************************************************************

ClustStcWorker::~ClustStcWorker()
{
    if(this->isRunning())
        stop();
}


//*************************************************************************************************************

void ClustStcWorker::addData(QList<VectorXd> &data)
{
    QMutexLocker locker(&m_qMutex);
    if(data.size() == 0)
        return;

    m_data.append(data);
}


//*************************************************************************************************************

void ClustStcWorker::clear()
{
    QMutexLocker locker(&m_qMutex);
    m_data.clear();
}


//*************************************************************************************************************

void ClustStcWorker::run()
{
    VectorXd m_vecAverage;

    m_bIsRunning = true;

    qDebug() << "Run worker";
    while(true)
    {
        {
            QMutexLocker locker(&m_qMutex);
            if(!m_bIsRunning)
                break;
        }

        bool doProcessing = false;
        {
            QMutexLocker locker(&m_qMutex);
            if(!m_data.isEmpty() && m_data.size() > 0)
                doProcessing = true;
        }

        if(doProcessing)
        {
            if(m_bIsLooping)
            {
                m_qMutex.lock();
                //Down sampling in loop mode
                if(m_vecAverage.rows() != m_data[0].rows())
                    m_vecAverage = m_data[m_iCurrentSample%m_data.size()];
                else
                    m_vecAverage += m_data[m_iCurrentSample%m_data.size()];
                m_qMutex.unlock();
            }
            else
            {
                m_qMutex.lock();
                //Down sampling in stream mode
                if(m_vecAverage.rows() != m_data[0].rows())
                    m_vecAverage = m_data.front();
                else
                    m_vecAverage += m_data.front();

                m_data.pop_front();
                m_qMutex.unlock();
            }

            m_qMutex.lock();
            ++m_iCurrentSample;

            if(m_iCurrentSample%m_iAverageSamples == 0)
            {
                m_vecAverage /= (double)m_iAverageSamples;

                emit stcSample(m_vecAverage);
                m_vecAverage = VectorXd::Zero(m_vecAverage.rows());
            }
            m_qMutex.unlock();
        }

        QThread::usleep(m_iUSecIntervall);
    }
}


//*************************************************************************************************************

void ClustStcWorker::setAverage(qint32 samples)
{
    QMutexLocker locker(&m_qMutex);
    m_iAverageSamples = samples;
}


//*************************************************************************************************************

void ClustStcWorker::setInterval(int usec)
{
    QMutexLocker locker(&m_qMutex);
    m_iUSecIntervall = usec;
}


//*************************************************************************************************************

void ClustStcWorker::setLoop(bool looping)
{
    QMutexLocker locker(&m_qMutex);
    m_bIsLooping = looping;
}


//*************************************************************************************************************

void ClustStcWorker::stop()
{
    m_qMutex.lock();
    m_bIsRunning = false;
    m_qMutex.unlock();

    QThread::wait();
}
