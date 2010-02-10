/*
   Copyright 2005-2009 Last.fm Ltd. 

   This file is part of the Last.fm Desktop Application Suite.

   lastfm-desktop is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   lastfm-desktop is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with lastfm-desktop.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedLayout>
#include <QStackedWidget>
#include <QGroupBox>
#include <QComboBox>
#include <QLabel>
#include <QSpacerItem>
#include <QListWidgetItem>
#include "SourceListWidget.h"
#include "SourceItemWidget.h"
#include "lib/unicorn/widgets/ImageButton.h"
#include "lastfm/ws.h"


SourceListWidget::SourceListWidget(QWidget* parent)
: QWidget(parent)
, m_layout(0)
{
    //m_layout = new QVBoxLayout(this);
}

void
SourceListWidget::setModel(SourceListModel* model)
{
    if (model) {
        m_model = model;
        // safe to throw away the layout like this?
        if (m_layout)
            m_layout->deleteLater();
        m_layout = new QVBoxLayout(this);
        addPlaceholders();
        connect(m_model, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)), SLOT(onRowsAboutToBeRemoved(QModelIndex, int, int)));
        connect(m_model, SIGNAL(rowsInserted(QModelIndex, int, int)), SLOT(onRowsInserted(QModelIndex, int, int)));
    }
}

void
SourceListWidget::addPlaceholders()
{
    while (m_layout->count() < m_model->getMaxSize()) {
        addPlaceholder();
    }
}

void
SourceListWidget::addPlaceholder()
{
    QGroupBox* box = new QGroupBox("");
    m_layout->addWidget(box);
}

void 
SourceListWidget::onRowsInserted(const QModelIndex& parent, int start, int end)
{
    Q_UNUSED(parent);
    for (int idx = start; idx <= end; idx++) {
        QWidget* old = m_layout->itemAt(idx)->widget();
        m_layout->removeWidget(old);
        old->deleteLater();
        m_layout->insertWidget(idx, createWidget(idx));
    }
}

void
SourceListWidget::onRowsAboutToBeRemoved(const QModelIndex& parent, int start, int end)
{
    Q_UNUSED(parent);
    for (int idx = start; idx <= end; idx++) {
        m_layout->takeAt(idx)->widget()->deleteLater();
        addPlaceholders();
    }
}

SourceItemWidget*
SourceListWidget::createWidget(int idx)
{
    QModelIndex qidx = m_model->index(idx);
    QString label = m_model->data(qidx, Qt::DisplayRole).toString();
    QString imgUrl = m_model->data(qidx, SourceListModel::ImageUrl).toString();
    SourceItemWidget* result = new SourceItemWidget(label);
    if (imgUrl.length()) {
        result->getImage(QUrl(imgUrl));
    }
    connect(result, SIGNAL(deleteClicked()), SLOT(onDeleteClicked()));
    return result;
}

void
SourceListWidget::onDeleteClicked()
{
    SourceItemWidget* source = qobject_cast<SourceItemWidget*>(sender());
    if (source) {
        int idx = m_layout->indexOf(source);
        if (idx != -1) {
            m_model->removeSource(idx);
        }
    }
}

//static
SourceListWidget::Operator
SourceListWidget::defaultOp(RqlSource::Type first, RqlSource::Type second)
{
    return (first == RqlSource::Tag && second == RqlSource::Tag) ? And : Or;
}

QString
SourceListWidget::rql()
{
    int count = 0;
    QString result;
    foreach (const QString& src, m_model->rql()) {
        if (count) {
            // get operator
            switch (RqlSource::Tag) {
                case RqlSource::User:
                case RqlSource::Art: result += " or "; break;
                case RqlSource::Tag: result += " and "; break;
            }
        }
        result += src;
        count++;
    }
    return result;
}

QString
SourceListWidget::stationDescription()
{
    int count = 0;
    QString result;
    foreach (const QString& src, m_model->descriptions()) {
        if (count) {
            // get operator
            //SourceItemWidget* item = qobject_cast<SourceItemWidget*>(m_layout->itemAt(count)->widget());

            switch (RqlSource::Tag) {
                case RqlSource::User:
                case RqlSource::Art: result += " or "; break;
                case RqlSource::Tag: result += " and "; break;
            }
        }
        result += src;
        count++;
    }
    return result;
}
