//=============================================================================================================
/**
* @file     projectionwindow.cpp
* @author   Lorenz Esch <Lorenz.Esch@tu-ilmenau.de>
*           Christoph Dinh <chdinh@nmr.mgh.harvard.edu>;
*           Matti Hamalainen <msh@nmr.mgh.harvard.edu>
* @version  1.0
* @date     December, 2014
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
* @brief    Contains the implementation of the ProjectionWindow class.
*
*/

//*************************************************************************************************************
//=============================================================================================================
// INCLUDES
//=============================================================================================================

#include "projectionwindow.h"


//*************************************************************************************************************
//=============================================================================================================
// USED NAMESPACES
//=============================================================================================================

using namespace MNEBrowseRawQt;


//*************************************************************************************************************
//=============================================================================================================
// DEFINE MEMBER METHODS
//=============================================================================================================

ProjectionWindow::ProjectionWindow(QWidget *parent)
: QDockWidget(parent)
, ui(new Ui::ProjectionWindow)
, m_pProjectionModel(new ProjectionModel(this))
{
    ui->setupUi(this);

    initTableViewWidgets();
}


//*************************************************************************************************************

ProjectionWindow::ProjectionWindow(QWidget *parent, QFile& qFile)
: QDockWidget(parent)
, ui(new Ui::ProjectionWindow)
, m_pProjectionModel(new ProjectionModel(this, qFile))
{
    ui->setupUi(this);

    initTableViewWidgets();
}


//*************************************************************************************************************

ProjectionWindow::ProjectionWindow(QWidget *parent, QList<FiffProj>& dataProjs)
: QDockWidget(parent)
, ui(new Ui::ProjectionWindow)
, m_pProjectionModel(new ProjectionModel(this, dataProjs))
{
    ui->setupUi(this);

    initTableViewWidgets();
}


//*************************************************************************************************************

void ProjectionWindow::initTableViewWidgets()
{
    //Set model
    ui->m_tableView_availableProjections->setModel(m_pProjectionModel);

    connect(m_pProjectionModel, &ProjectionModel::dataChanged,
            ui->m_tableView_availableProjections, &QTableView::resizeColumnsToContents);

    //Hide data column in view
    ui->m_tableView_availableProjections->setColumnHidden(3, true);
}


//*************************************************************************************************************

ProjectionModel* ProjectionWindow::getDataModel()
{
    return m_pProjectionModel;
}
