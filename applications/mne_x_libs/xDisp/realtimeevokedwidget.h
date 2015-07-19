//=============================================================================================================
/**
* @file     realtimeevokedwidget.h
* @author   Christoph Dinh <chdinh@nmr.mgh.harvard.edu>;
*           Matti Hamalainen <msh@nmr.mgh.harvard.edu>
* @version  1.0
* @date     February, 2013
*
* @section  LICENSE
*
* Copyright (C) 2013, Christoph Dinh and Matti Hamalainen. All rights reserved.
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
* @brief    Declaration of the RealTimeEvokedWidget Class.
*
*/

#ifndef REALTIMEEVOKEDWIDGET_H
#define REALTIMEEVOKEDWIDGET_H


//*************************************************************************************************************
//=============================================================================================================
// INCLUDES
//=============================================================================================================

#include "xdisp_global.h"
#include "newmeasurementwidget.h"
#include "helpers/realtimeevokedmodel.h"
#include "helpers/realtimebutterflyplot.h"
#include "helpers/evokedmodalitywidget.h"
#include "helpers/selectionmanagerwindow.h"
#include "helpers/chinfomodel.h"
#include "helpers/quickcontrolwidget.h"
#include "helpers/scalingwidget.h"
#include "helpers/averagescene.h"
#include "helpers/averagesceneitem.h"


//*************************************************************************************************************
//=============================================================================================================
// QT INCLUDES
//=============================================================================================================

#include <QSharedPointer>
#include <QList>
#include <QAction>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QToolBox>


//*************************************************************************************************************
//=============================================================================================================
// FORWARD DECLARATIONS
//=============================================================================================================

class QTime;

namespace XMEASLIB
{
class RealTimeEvoked;
}


//*************************************************************************************************************
//=============================================================================================================
// DEFINE NAMESPACE XDISPLIB
//=============================================================================================================

namespace XDISPLIB
{

//*************************************************************************************************************
//=============================================================================================================
// FORWARD DECLARATIONS
//=============================================================================================================

struct Modality;


//*************************************************************************************************************
//=============================================================================================================
// USED NAMESPACES
//=============================================================================================================

using namespace XMEASLIB;


//*************************************************************************************************************
//=============================================================================================================
// ENUMERATIONS
//=============================================================================================================

////=============================================================================================================
///**
//* Tool enumeration.
//*/
//enum Tool
//{
//    Freeze     = 0,       /**< Freezing tool. */
//    Annotation = 1        /**< Annotation tool. */
//};


//=============================================================================================================
/**
* DECLARE CLASS RealTimeMultiSampleArrayNewWidget
*
* @brief The RealTimeMultiSampleArrayNewWidget class provides a real-time curve display.
*/
class XDISPSHARED_EXPORT RealTimeEvokedWidget : public NewMeasurementWidget
{
    Q_OBJECT

    friend class EvokedModalityWidget;
    friend class ScalingWidget;

public:
    //=========================================================================================================
    /**
    * Constructs a RealTimeEvokedWidget which is a child of parent.
    *
    * @param [in] pRTE          pointer to real-time evoked measurement.
    * @param [in] pTime         pointer to application time.
    * @param [in] parent        pointer to parent widget; If parent is 0, the new NumericWidget becomes a window. If parent is another widget, NumericWidget becomes a child window inside parent. NumericWidget is deleted when its parent is deleted.
    */
    RealTimeEvokedWidget(QSharedPointer<RealTimeEvoked> pRTE, QSharedPointer<QTime> &pTime, QWidget* parent = 0);

    //=========================================================================================================
    /**
    * Destroys the RealTimeEvokedWidget.
    */
    ~RealTimeEvokedWidget();

    //=========================================================================================================
    /**
    * Is called when new data are available.
    *
    * @param [in] pMeasurement  pointer to measurement -> not used because its direct attached to the measurement.
    */
    virtual void update(XMEASLIB::NewMeasurement::SPtr pMeasurement);

    //=========================================================================================================
    /**
    * Is called when new data are available.
    */
    virtual void getData();

    //=========================================================================================================
    /**
    * Initialise the RealTimeEvokedWidget.
    */
    virtual void init();

    //=========================================================================================================
    /**
    * call this whenever the external channel selection manager changes
    *
    * * @param [in] selectedChannelItems list of selected graphic items
    */
    void channelSelectionManagerChanged(const QList<QGraphicsItem *> &selectedChannelItems);

    //=========================================================================================================
    /**
    * Scales the averaged data according to scaleMap
    *
    * @param [in] scaleMap map with all channel types and their current scaling value
    */
    void scaleAveragedData(const QMap<qint32, float> &scaleMap);

private:
    //=========================================================================================================
    /**
    * Shows sensor selection widget
    */
    void showSensorSelectionWidget();

    //=========================================================================================================
    /**
    * Show the modality selection widget
    */
    void showModalitySelectionWidget();

    //=========================================================================================================
    /**
    * Only shows the channels defined in the QStringList selectedChannels
    *
    * @param [in] selectedChannels list of all channel names which are currently selected in the selection manager.
    */
    void showSelectedChannelsOnly(QStringList selectedChannels);

    //=========================================================================================================
    /**
    * Broadcast channel scaling
    *
    * @param [in] scaleMap QMap with scaling values which is to be broadcasted to the model.
    */
    void broadcastScaling(QMap<qint32, float> scaleMap);

    //=========================================================================================================
    /**
    * Broadcast settings to attached widgets
    */
    void broadcastSettings(QList<Modality> modalityList);

    //=========================================================================================================
    /**
    * Show channel scaling widget
    */
    void showChScalingWidget();

    //=========================================================================================================
    /**
    * Shows quick control widget
    */
    void showQuickControlWidget();

    //=========================================================================================================
    /**
    * Apply the in m_qListCurrentSelection stored selection -> hack around C++11 lambda
    */
    void applySelection();

    //=========================================================================================================
    /**
    * Reset the in m_qListCurrentSelection stored selection -> hack around C++11 lambda
    */
    void resetSelection();

    //=========================================================================================================
    /**
    * Reimplemented mouseDoubleClickEvent
    */
    virtual void mouseDoubleClickEvent(QMouseEvent * event);

    //=========================================================================================================
    /**
    * call this function whenever a selection was made in teh evoked data set list
    */
    void onSelectionChanged();

    RealTimeEvokedModel*        m_pRTEModel;                /**< RTE data model */
    RealTimeButterflyPlot*      m_pButterflyPlot;           /**< Butterfly plot */
    AverageScene*               m_pAverageScene;            /**< The pointer to the average scene. */

    bool            m_bInitialized;             /**< Is Initialized */
    bool            m_bHideBadChannels;         /**< hide bad channels flag. */

    FiffInfo::SPtr  m_pFiffInfo;                /**< FiffInfo, which is used insteadd of ListChInfo*/

    QAction*        m_pActionSelectSensors;     /**< show roi select widget */
    QAction*        m_pActionSelectModality;    /**< Modality selection action */
    QAction*        m_pActionChScaling;         /**< Show channel scaling Action. */
    QAction*        m_pActionQuickControl;      /**< Show quick control widget. */

    QVBoxLayout*    m_pRteLayout;               /**< RTE Widget layout */
    QLabel*         m_pLabelInit;               /**< Initialization Label */
    QToolBox*       m_pToolBox;                 /**< The toolbox which holds the butterfly and 2D layout plot */
    QGraphicsView*  m_pAverageLayoutView;       /**< View for 2D average layout scene */

    QSharedPointer<QuickControlWidget>          m_pQuickControlWidget;      /**< Quick control widget. */
    QSharedPointer<SelectionManagerWindow>      m_pSelectionManagerWindow;  /**< SelectionManagerWindow. */
    QSharedPointer<ScalingWidget>               m_pScalingWidget;           /**< Channel scaling widget. */
    QSharedPointer<ChInfoModel>                 m_pChInfoModel;             /**< Channel info model. */
    QSharedPointer<EvokedModalityWidget>        m_pEvokedModalityWidget;    /**< Evoked modality widget. */
    QSharedPointer<RealTimeEvoked>              m_pRTE;                     /**< The real-time evoked measurement. */

    QList<Modality>                     m_qListModalities;
    QList<qint32>                       m_qListCurrentSelection;    /**< Current selection list -> hack around C++11 lambda  */
    QList<RealTimeSampleArrayChInfo>    m_qListChInfo;              /**< Channel info list. ToDo: check if this is obsolete later on*/
    QMap<qint32,float>                  m_qMapChScaling;            /**< Channel scaling values. */
};

} // NAMESPACE XDISPLIB

#endif // REALTIMEEVOKEDWIDGET_H
