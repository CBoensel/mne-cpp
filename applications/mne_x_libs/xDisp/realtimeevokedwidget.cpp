//=============================================================================================================
/**
* @file     realtimeevokedwidget.cpp
* @author   Christoph Dinh <chdinh@nmr.mgh.harvard.edu>;
*           Matti Hamalainen <msh@nmr.mgh.harvard.edu>
* @version  1.0
* @date     July, 2014
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
* @brief    Implementation of the RealTimeEvokedWidget Class.
*
*/

//ToDo Paint to render area

//*************************************************************************************************************
//=============================================================================================================
// INCLUDES
//=============================================================================================================

#include "realtimeevokedwidget.h"
//#include "annotationwindow.h"

#include <xMeas/realtimeevoked.h>


//*************************************************************************************************************
//=============================================================================================================
// Eigen INCLUDES
//=============================================================================================================

#include <Eigen/Core>


//*************************************************************************************************************
//=============================================================================================================
// STL INCLUDES
//=============================================================================================================

#include <math.h>


//*************************************************************************************************************
//=============================================================================================================
// QT INCLUDES
//=============================================================================================================

#include <QPaintEvent>
#include <QPainter>
#include <QHeaderView>
#include <QMenu>
#include <QMessageBox>
#include <QScroller>
#include <QSettings>

#include <QDebug>


//*************************************************************************************************************
//=============================================================================================================
// USED NAMESPACES
//=============================================================================================================

using namespace XDISPLIB;
using namespace XMEASLIB;


//=============================================================================================================
/**
* Tool enumeration.
*/
enum Tool
{
    Freeze     = 0,     /**< Freezing tool. */
    Annotation = 1      /**< Annotation tool. */
};


//*************************************************************************************************************
//=============================================================================================================
// DEFINE MEMBER METHODS
//=============================================================================================================

RealTimeEvokedWidget::RealTimeEvokedWidget(QSharedPointer<RealTimeEvoked> pRTE, QSharedPointer<QTime> &pTime, QWidget* parent)
: NewMeasurementWidget(parent)
, m_pRTEModel(Q_NULLPTR)
, m_pButterflyPlot(Q_NULLPTR)
, m_pRTE(pRTE)
, m_bInitialized(false)
{
    Q_UNUSED(pTime)

    m_pActionSelectModality = new QAction(QIcon(":/images/evokedSettings.png"), tr("Shows the modality selection widget (F12)"),this);
    m_pActionSelectModality->setShortcut(tr("F12"));
    m_pActionSelectModality->setStatusTip(tr("Show the modality selection widget (F12)"));
    connect(m_pActionSelectModality, &QAction::triggered,
            this, &RealTimeEvokedWidget::showModalitySelectionWidget);
    addDisplayAction(m_pActionSelectModality);
    m_pActionSelectModality->setVisible(false);

    m_pActionSelectSensors = new QAction(QIcon(":/images/selectSensors.png"), tr("Show the region selection widget (F11)"),this);
    m_pActionSelectSensors->setShortcut(tr("F11"));
    m_pActionSelectSensors->setStatusTip(tr("Show the region selection widget (F11)"));
    connect(m_pActionSelectSensors, &QAction::triggered,
            this, &RealTimeEvokedWidget::showSensorSelectionWidget);
    addDisplayAction(m_pActionSelectSensors);
    m_pActionSelectSensors->setVisible(false);

    m_pActionChScaling = new QAction(QIcon(":/images/channelScaling.png"), tr("Show the channel scaling widget (F10)"),this);
    m_pActionChScaling->setShortcut(tr("F10"));
    m_pActionChScaling->setStatusTip(tr("Show the channel scaling widget (F10)"));
    connect(m_pActionChScaling, &QAction::triggered,
            this, &RealTimeEvokedWidget::showChScalingWidget);
    addDisplayAction(m_pActionChScaling);
    m_pActionChScaling->setVisible(false);

    m_pActionQuickControl = new QAction(QIcon(":/images/quickControl.png"), tr("Show quick control widget (F9)"),this);
    m_pActionQuickControl->setShortcut(tr("F9"));
    m_pActionQuickControl->setStatusTip(tr("Show quick control widget (F9)"));
    connect(m_pActionQuickControl, &QAction::triggered,
            this, &RealTimeEvokedWidget::showQuickControlWidget);
    addDisplayAction(m_pActionQuickControl);
    m_pActionQuickControl->setVisible(false);

    //set vertical layout
    m_pRteLayout = new QVBoxLayout(this);

    //Acquire label
    m_pLabelInit= new QLabel;
    m_pLabelInit->setText("Acquiring Data");
    m_pLabelInit->setAlignment(Qt::AlignCenter);
    QFont font;font.setBold(true);font.setPointSize(20);
    m_pLabelInit->setFont(font);
    m_pRteLayout->addWidget(m_pLabelInit);

    //Create toolboxes with butterfly and 2D layout plot
    m_pToolBox = new QToolBox(this);
    m_pToolBox->hide();

    //Butterfly
    if(m_pButterflyPlot)
        delete m_pButterflyPlot;
    m_pButterflyPlot = new RealTimeButterflyPlot;

    m_pToolBox->insertItem(0, m_pButterflyPlot, QIcon(), "Butterfly plot");

    //2D layout plot
    m_pAverageLayoutView = new QGraphicsView;

    m_pToolBox->insertItem(0, m_pAverageLayoutView, QIcon(), "2D Layout plot");

    m_pRteLayout->addWidget(m_pToolBox);

    //set layouts
    this->setLayout(m_pRteLayout);

    getData();
}


//*************************************************************************************************************

RealTimeEvokedWidget::~RealTimeEvokedWidget()
{
    //
    // Store Settings
    //
    if(!m_pRTE->getName().isEmpty())
    {
        QString t_sRTEWName = m_pRTE->getName();

        QSettings settings;

        for(qint32 i = 0; i < m_qListModalities.size(); ++i)
        {
            settings.setValue(QString("RTEW/%1/%2/active").arg(t_sRTEWName).arg(m_qListModalities[i].m_sName), m_qListModalities[i].m_bActive);
            settings.setValue(QString("RTEW/%1/%2/norm").arg(t_sRTEWName).arg(m_qListModalities[i].m_sName), m_qListModalities[i].m_fNorm);
        }

        if(m_qMapChScaling.contains(FIFF_UNIT_T))
            settings.setValue(QString("RTEW/%1/scaleMAG").arg(t_sRTEWName), m_qMapChScaling[FIFF_UNIT_T]);

        if(m_qMapChScaling.contains(FIFF_UNIT_T_M))
            settings.setValue(QString("RTEW/%1/scaleGRAD").arg(t_sRTEWName), m_qMapChScaling[FIFF_UNIT_T_M]);

        if(m_qMapChScaling.contains(FIFFV_EEG_CH))
            settings.setValue(QString("RTEW/%1/scaleEEG").arg(t_sRTEWName), m_qMapChScaling[FIFFV_EEG_CH]);

        if(m_qMapChScaling.contains(FIFFV_EOG_CH))
            settings.setValue(QString("RTEW/%1/scaleEOG").arg(t_sRTEWName), m_qMapChScaling[FIFFV_EOG_CH]);

        if(m_qMapChScaling.contains(FIFFV_STIM_CH))
            settings.setValue(QString("RTEW/%1/scaleSTIM").arg(t_sRTEWName), m_qMapChScaling[FIFFV_STIM_CH]);

        if(m_qMapChScaling.contains(FIFFV_MISC_CH))
            settings.setValue(QString("RTEW/%1/scaleMISC").arg(t_sRTEWName), m_qMapChScaling[FIFFV_MISC_CH]);
    }
}


//*************************************************************************************************************

void RealTimeEvokedWidget::update(XMEASLIB::NewMeasurement::SPtr)
{
    getData();
}


//*************************************************************************************************************

void RealTimeEvokedWidget::getData()
{
    if(!m_bInitialized)
    {
        if(m_pRTE->isInitialized())
        {
//            QFile file(m_pRTE->getXMLLayoutFile());
//            if (!file.open(QFile::ReadOnly | QFile::Text))
//            {
//                qDebug() << QString("Cannot read file %1:\n%2.").arg(m_pRTE->getXMLLayoutFile()).arg(file.errorString());
//                m_pSensorModel = new SensorModel(this);
//                m_pSensorModel->mapChannelInfo(m_qListChInfo);
//            }
//            else
//            {
//                m_pSensorModel = new SensorModel(&file, this);
//                m_pSensorModel->mapChannelInfo(m_qListChInfo);
//                m_pActionSelectSensors->setVisible(true);
//            }

            m_qListChInfo = m_pRTE->chInfo();
            m_pFiffInfo = FiffInfo::SPtr(&m_pRTE->info());

            init();

            m_pRTEModel->updateData();
        }
    }
    else
        m_pRTEModel->updateData();
}


//*************************************************************************************************************

void RealTimeEvokedWidget::init()
{
    if(m_pRTE->isInitialized())
    {
        QString t_sRTEWName = m_pRTE->getName();
        m_pRteLayout->removeWidget(m_pLabelInit);
        m_pLabelInit->hide();

        m_pToolBox->show();
        //m_pButterflyPlot->show();

        if(m_pRTEModel)
            delete m_pRTEModel;
        m_pRTEModel = new RealTimeEvokedModel(this);

        m_pRTEModel->setRTE(m_pRTE);

        m_pButterflyPlot->setModel(m_pRTEModel);

        m_qListModalities.clear();
        bool hasMag = false;
        bool hasGrad = false;
        bool hasEEG = false;
        bool hasEOG = false;
        bool hasMISC = false;
        for(qint32 i = 0; i < m_pRTE->info().nchan; ++i)
        {
            if(m_pRTE->info().chs[i].kind == FIFFV_MEG_CH)
            {
                if(!hasMag && m_pRTE->info().chs[i].unit == FIFF_UNIT_T)
                    hasMag = true;
                else if(!hasGrad &&  m_pRTE->info().chs[i].unit == FIFF_UNIT_T_M)
                    hasGrad = true;
            }
            else if(!hasEEG && m_pRTE->info().chs[i].kind == FIFFV_EEG_CH)
                hasEEG = true;
            else if(!hasEOG && m_pRTE->info().chs[i].kind == FIFFV_EOG_CH)
                hasEOG = true;
            else if(!hasMISC && m_pRTE->info().chs[i].kind == FIFFV_MISC_CH)
                hasMISC = true;
        }
        QSettings settings;
        bool sel = false;
        float val = 1e-11f;
        if(hasMag)
        {
            sel = settings.value(QString("RTEW/%1/MAG/active").arg(t_sRTEWName), true).toBool();
            val = settings.value(QString("RTEW/%1/MAG/norm").arg(t_sRTEWName), 1e-11f).toFloat();
            m_qListModalities.append(Modality("MAG",sel,val));
        }
        if(hasGrad)
        {
            sel = settings.value(QString("RTEW/%1/GRAD/active").arg(t_sRTEWName), true).toBool();
            val = settings.value(QString("RTEW/%1/GRAD/norm").arg(t_sRTEWName), 1e-10f).toFloat();
            m_qListModalities.append(Modality("GRAD",sel,val));
        }
        if(hasEEG)
        {
            sel = settings.value(QString("RTEW/%1/EEG/active").arg(t_sRTEWName), true).toBool();
            val = settings.value(QString("RTEW/%1/EEG/norm").arg(t_sRTEWName), 1e-4f).toFloat();
            m_qListModalities.append(Modality("EEG",sel,val));
        }
        if(hasEOG)
        {
            sel = settings.value(QString("RTEW/%1/EOG/active").arg(t_sRTEWName), true).toBool();
            val = settings.value(QString("RTEW/%1/EOG/norm").arg(t_sRTEWName), 1e-3f).toFloat();
            m_qListModalities.append(Modality("EOG",sel,val));
        }
        if(hasMISC)
        {
            sel = settings.value(QString("RTEW/%1/MISC/active").arg(t_sRTEWName), true).toBool();
            val = settings.value(QString("RTEW/%1/MISC/norm").arg(t_sRTEWName), 1e-3f).toFloat();
            m_qListModalities.append(Modality("MISC",sel,val));
        }

        m_pButterflyPlot->setSettings(m_qListModalities);

        //Set up scaling
        //Show only spin boxes and labels which type are present in the current loaded fiffinfo
        QList<FiffChInfo> channelList = m_pFiffInfo->chs;
        QList<int> availabeChannelTypes;

        for(int i = 0; i<channelList.size(); i++) {
            int unit = channelList.at(i).unit;
            int type = channelList.at(i).kind;

            if(!availabeChannelTypes.contains(unit))
                availabeChannelTypes.append(unit);

            if(!availabeChannelTypes.contains(type))
                availabeChannelTypes.append(type);
        }

        QString t_sRTEName = m_pRTE->getName();

        if(!t_sRTEName.isEmpty())
        {
            m_qMapChScaling.clear();

            QSettings settings;
            float val = 0.0f;
            if(availabeChannelTypes.contains(FIFF_UNIT_T)) {
                val = settings.value(QString("RTEW/%1/scaleMAG").arg(t_sRTEWName), 1e-11f).toFloat();
                m_qMapChScaling.insert(FIFF_UNIT_T, val);
            }

            if(availabeChannelTypes.contains(FIFF_UNIT_T_M)) {
                val = settings.value(QString("RTEW/%1/scaleGRAD").arg(t_sRTEWName), 1e-10f).toFloat();
                m_qMapChScaling.insert(FIFF_UNIT_T_M, val);
            }

            if(availabeChannelTypes.contains(FIFFV_EEG_CH)) {
                val = settings.value(QString("RTEW/%1/scaleEEG").arg(t_sRTEWName), 1e-4f).toFloat();
                m_qMapChScaling.insert(FIFFV_EEG_CH, val);
            }

            if(availabeChannelTypes.contains(FIFFV_EOG_CH)) {
                val = settings.value(QString("RTEW/%1/scaleEOG").arg(t_sRTEWName), 1e-3f).toFloat();
                m_qMapChScaling.insert(FIFFV_EOG_CH, val);
            }

            if(availabeChannelTypes.contains(FIFFV_STIM_CH)) {
                val = settings.value(QString("RTEW/%1/scaleSTIM").arg(t_sRTEWName), 1e-3f).toFloat();
                m_qMapChScaling.insert(FIFFV_STIM_CH, val);
            }

            if(availabeChannelTypes.contains(FIFFV_MISC_CH)) {
                val = settings.value(QString("RTEW/%1/scaleMISC").arg(t_sRTEWName), 1e-3f).toFloat();
                m_qMapChScaling.insert(FIFFV_MISC_CH, val);
            }

            m_pRTEModel->setScaling(m_qMapChScaling);
        }

        if(!m_pScalingWidget)
        {
            m_pScalingWidget = QSharedPointer<ScalingWidget>(new ScalingWidget(m_qMapChScaling));

            //m_pScalingWidget->setWindowFlags(Qt::WindowStaysOnTopHint);

            connect(m_pScalingWidget.data(), &ScalingWidget::scalingChanged,
                    this, &RealTimeEvokedWidget::broadcastScaling);
        }

        m_pActionChScaling->setVisible(false);

        //Quick control widget
        if(!m_pQuickControlWidget) {
            m_pQuickControlWidget = QSharedPointer<QuickControlWidget>(new QuickControlWidget(m_qMapChScaling, m_pFiffInfo, "RT Averaging", 0, true, true, false, false, true));
            m_pQuickControlWidget->setWindowFlags(Qt::WindowStaysOnTopHint);

            //Handle scaling
            connect(m_pQuickControlWidget.data(), &QuickControlWidget::scalingChanged,
                    this, &RealTimeEvokedWidget::broadcastScaling);

            //Handle projections
            connect(m_pQuickControlWidget.data(), &QuickControlWidget::projSelectionChanged,
                    m_pRTEModel, &RealTimeEvokedModel::updateProjection);

            connect(m_pQuickControlWidget.data(), &QuickControlWidget::settingsChanged,
                    this, &RealTimeEvokedWidget::broadcastSettings);
        }

        m_pActionQuickControl->setVisible(true);

        //Set up selection manager
        m_pChInfoModel = QSharedPointer<ChInfoModel>(new ChInfoModel(this, m_pFiffInfo));
        m_pSelectionManagerWindow = QSharedPointer<SelectionManagerWindow>(new SelectionManagerWindow(this, m_pChInfoModel.data()));

        connect(m_pSelectionManagerWindow.data(), &SelectionManagerWindow::showSelectedChannelsOnly,
                this, &RealTimeEvokedWidget::showSelectedChannelsOnly);

        //Connect channel info model
        connect(m_pSelectionManagerWindow.data(), &SelectionManagerWindow::loadedLayoutMap,
                m_pChInfoModel.data(), &ChInfoModel::layoutChanged);

        connect(m_pChInfoModel.data(), &ChInfoModel::channelsMappedToLayout,
                m_pSelectionManagerWindow.data(), &SelectionManagerWindow::setCurrentlyMappedFiffChannels);

        m_pActionSelectSensors->setVisible(true);

        //Set up modality widget
        if(!m_pEvokedModalityWidget)
        {
            m_pEvokedModalityWidget = QSharedPointer<EvokedModalityWidget>(new EvokedModalityWidget(this, this));

            m_pEvokedModalityWidget->setWindowTitle("Modality Selection");

//            connect(m_pEvokedModalityWidget.data(), &EvokedModalityWidget::settingsChanged,
//                    this, &RealTimeEvokedWidget::broadcastSettings);
        }

        m_pActionSelectModality->setVisible(false);

        //Init average scene
        m_pAverageScene = new AverageScene(m_pAverageLayoutView, this);
        m_pAverageLayoutView->setScene(m_pAverageScene);

        //Connect selection manager with average manager
        connect(m_pSelectionManagerWindow.data(), &SelectionManagerWindow::selectionChanged,
                this, &RealTimeEvokedWidget::channelSelectionManagerChanged);

        connect(m_pRTEModel, &RealTimeEvokedModel::dataChanged,
                this, &RealTimeEvokedWidget::onSelectionChanged);

        connect(m_pQuickControlWidget.data(), &QuickControlWidget::scalingChanged,
                this, &RealTimeEvokedWidget::scaleAveragedData);

        // Initialized
        m_bInitialized = true;
    }
}


//*************************************************************************************************************

void RealTimeEvokedWidget::channelSelectionManagerChanged(const QList<QGraphicsItem*> &selectedChannelItems)
{
    //Repaint the average items in the average scene based on the input parameter
    m_pAverageScene->repaintItems(selectedChannelItems);

    //call the onSelection function manually to replot the data for the givven average items
    onSelectionChanged();

    //fit everything in the view and update the scene
    m_pAverageLayoutView->fitInView(m_pAverageScene->sceneRect(), Qt::KeepAspectRatio);
    m_pAverageScene->update(m_pAverageScene->sceneRect());
}


//*************************************************************************************************************

void RealTimeEvokedWidget::showModalitySelectionWidget()
{
    m_pEvokedModalityWidget->show();
}


//*************************************************************************************************************

void RealTimeEvokedWidget::showSensorSelectionWidget()
{
    if(!m_pSelectionManagerWindow) {
        m_pSelectionManagerWindow = QSharedPointer<SelectionManagerWindow>(new SelectionManagerWindow);
    }

    m_pSelectionManagerWindow->show();
}


//*************************************************************************************************************

void RealTimeEvokedWidget::applySelection()
{
//    m_pRTMSAModel->selectRows(m_qListCurrentSelection);

//    m_pSensorModel->silentUpdateSelection(m_qListCurrentSelection);
}


//*************************************************************************************************************

void RealTimeEvokedWidget::resetSelection()
{
//    // non C++11 alternative
//    m_qListCurrentSelection.clear();
//    for(qint32 i = 0; i < m_qListChInfo.size(); ++i)
//        m_qListCurrentSelection.append(i);

//    applySelection();
}


//*************************************************************************************************************

void RealTimeEvokedWidget::showSelectedChannelsOnly(QStringList selectedChannels)
{
    QList<int> selectedChannelsIndexes;

    for(int i = 0; i<selectedChannels.size(); i++)
        selectedChannelsIndexes<<m_pChInfoModel->getIndexFromOrigChName(selectedChannels.at(i));

    m_pButterflyPlot->setSelectedChannels(selectedChannelsIndexes);
}


//*************************************************************************************************************

void RealTimeEvokedWidget::broadcastScaling(QMap<qint32,float> scaleMap)
{
    m_pRTEModel->setScaling(scaleMap);
}


//*************************************************************************************************************

void RealTimeEvokedWidget::broadcastSettings(QList<Modality> modalityList)
{
    m_qListModalities = modalityList;
    m_pButterflyPlot->setSettings(modalityList);
}


//*************************************************************************************************************

void RealTimeEvokedWidget::showChScalingWidget()
{
    if(m_pScalingWidget->isActiveWindow())
        m_pScalingWidget->hide();
    else {
        m_pScalingWidget->activateWindow();
        m_pScalingWidget->show();
    }
}


//*************************************************************************************************************

void RealTimeEvokedWidget::showQuickControlWidget()
{
    m_pQuickControlWidget->show();
}


//*************************************************************************************************************

void RealTimeEvokedWidget::mouseDoubleClickEvent(QMouseEvent * event)
{
    if(event->button() == Qt::LeftButton)
        m_pRTEModel->toggleFreeze();
}


//*************************************************************************************************************

void RealTimeEvokedWidget::onSelectionChanged()
{
    //Get current items from the average scene
    QList<QGraphicsItem *> currentAverageSceneItems = m_pAverageScene->items();

    //Set new data for all averageSceneItems
    for(int i = 0; i<currentAverageSceneItems.size(); i++) {
        AverageSceneItem* averageSceneItemTemp = static_cast<AverageSceneItem*>(currentAverageSceneItems.at(i));

        averageSceneItemTemp->m_lAverageData.clear();

        //Get only the necessary data from the average model (use column 2)
        RowVectorPair averageData = m_pRTEModel->data(0, 2, RealTimeEvokedModelRoles::GetAverageData).value<RowVectorPair>();

        //Get the averageScenItem specific data row
        QStringList chNames = m_pFiffInfo->ch_names;

        int channelNumber = chNames.indexOf(averageSceneItemTemp->m_sChannelName);
        if(channelNumber != -1) {
            averageSceneItemTemp->m_iChannelKind = m_pFiffInfo->chs.at(channelNumber).kind;
            averageSceneItemTemp->m_iChannelUnit = m_pFiffInfo->chs.at(channelNumber).unit;;
            averageSceneItemTemp->m_iChannelNumber = channelNumber;
            averageSceneItemTemp->m_iTotalNumberChannels = chNames.size();
            averageSceneItemTemp->m_lAverageData.append(averageData);
        }
    }

    m_pAverageScene->update();
}


//*************************************************************************************************************

void RealTimeEvokedWidget::scaleAveragedData(const QMap<qint32, float> &scaleMap)
{
    qDebug()<<"scaleAveragedData";
    //Set the scale map received from the scale window
    m_pAverageScene->setScaleMap(scaleMap);
}
