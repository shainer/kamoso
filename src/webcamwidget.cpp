/*************************************************************************************
 *  Copyright (C) 2008 by Aleix Pol <aleixpol@gmail.com>                             *
 *  Copyright (C) 2008 by Alex Fiestas <alex@eyeos.org>                              *
 *                                                                                   *
 *  This program is free software; you can redistribute it and/or                    *
 *  modify it under the terms of the GNU General Public License                      *
 *  as published by the Free Software Foundation; either version 3                   *
 *  of the License, or (at your option) any later version.                           *
 *                                                                                   *
 *  This program is distributed in the hope that it will be useful,                  *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of                   *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                    *
 *  GNU General Public License for more details.                                     *
 *                                                                                   *
 *  You should have received a copy of the GNU General Public License                *
 *  along with this program; if not, write to the Free Software                      *
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA   *
 *************************************************************************************/

#include "webcamwidget.h"

#include <QTimer>
#include <KIcon>
#include <KUrl>
#include "webcamretriever.h"

WebcamWidget::WebcamWidget(QWidget* parent) : QLabel(parent)
{
	this->setMinimumSize(200,200);
	setScaledContents(false);
	setAlignment(Qt::AlignCenter);
	setPixmap(KIcon("camera-web").pixmap(128,128));
	mRetriever = new WebcamRetriever;
	mRetriever->start();
	connect(mRetriever, SIGNAL(imageReady()), SLOT(slotUpdateImage()));
	qDebug("%d", mRetriever->isAvailable());
}

WebcamWidget::~WebcamWidget()
{
	mRetriever->markDone();
}

bool WebcamWidget::takePhoto(const KUrl& dest)
{
	if(!mRetriever->isAvailable())
		return false;
	Q_ASSERT(dest.isLocalFile()); //FIXME: Let it save using KIO when not local
	
	mRetriever->lock().lockForRead();
	bool ret = mRetriever->image().save(dest.toLocalFile());
	mRetriever->lock().unlock();
	
	if(ret)
		emit photoTaken(dest);
	
	return ret;
}

void WebcamWidget::slotUpdateImage()
{
// 	qDebug("pam");
	mRetriever->lock().lockForRead();
	setPixmap(QPixmap::fromImage(mRetriever->image()));
	mRetriever->lock().unlock();
}

QSize WebcamWidget::sizeHint() const
{
	return mRetriever->imageSize();
}