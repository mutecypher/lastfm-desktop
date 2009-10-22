/***************************************************************************
*   Copyright (C) 2005 - 2007 by                                          *
*      Jono Cole, Last.fm Ltd <jono@last.fm>                              *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.          *
***************************************************************************/

#include "AbstractBootstrapper.h"

#include <lastfm/ws.h>
#include <QNetworkRequest>
#include <QUrl>
#include <QFile>
#include <QDir>
#include <QTextStream>

#include <time.h>

#include "zlib.h"

static const QString k_host = "bootstrap.last.fm";


AbstractBootstrapper::AbstractBootstrapper( QObject* parent )
                     :QObject( parent )
{
}


AbstractBootstrapper::~AbstractBootstrapper(void)
{
}


bool
AbstractBootstrapper::zipFiles( const QString& inFileName, const QString& outFileName ) const
{
    QDir temp = QDir::temp();

    temp.remove( outFileName );

    gzFile outFile = gzopen( outFileName.toLocal8Bit(), "wb" );
    if ( !outFile )
        return false;

    QFile inFile( inFileName );
    if ( !inFile.open( QIODevice::ReadOnly | QIODevice::Text ) )
        return false;

    if ( gzputs( outFile, inFile.readAll().data() ) < 1 )
        return false;

    gzclose( outFile );
    inFile.close();

    return true;
}


void
AbstractBootstrapper::sendZip( const QString& inFile )
{
    QString username = lastfm::ws::Username;

    // Get Unix time
    time_t now;
    time( &now );
    QString time = QString::number( now );

    QString path = QString( "http://" + k_host + "/bootstrap/index.php?user=%1&time=%2" )
        .arg( username, time );

    QFile* zipFile = new QFile( this );
    zipFile->setFileName( inFile );
    zipFile->open( QIODevice::ReadOnly );

    QNetworkRequest request;
    request.setUrl( path );
    request.setRawHeader( "Content-type", "multipart/form-data, boundary=AaB03x" );
    request.setRawHeader( "Cache-Control", "no-cache" );
    request.setRawHeader( "Accept", "*/*" );

    QByteArray bytes;
    bytes.append( "--AaB03x\r\n" );
    bytes.append( "content-disposition: " );
    bytes.append( "form-data; name=\"agency\"\r\n" );
    bytes.append( "\r\n" );
    bytes.append( "0\r\n" );
    bytes.append( "--AaB03x\r\n" );
    bytes.append( "content-disposition: " );
    bytes.append( "form-data; name=\"bootstrap\"; filename=\"" + zipFile->fileName() + "\"\r\n" );
    bytes.append( "Content-Transfer-Encoding: binary\r\n" );
    bytes.append( "\r\n" );

    bytes.append( zipFile->readAll() );
    zipFile->close();

    bytes.append( "\r\n" );
    bytes.append( "--AaB03x--" );
    request.setHeader( QNetworkRequest::ContentLengthHeader, 
                       bytes.length() );

    qDebug() << "Sending " << path;

    emit percentageUploaded( 0 );

    QNetworkReply* reply = lastfm::nam()->post( request, bytes );
    //connect( reply, SIGNAL(uploadProgress(qint64,qint64)), SLOT( onUploadProgress(qint64,qint64)));
    connect( reply, SIGNAL(finished()), SLOT(onUploadDone()));
}


void
AbstractBootstrapper::onUploadProgress( qint64 done, qint64 total )
{
    emit percentageUploaded( int( float(done / total) * 100.0 ) );
}


void
AbstractBootstrapper::onUploadDone()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>( sender());
    if( reply->error() == QNetworkReply::ContentAccessDenied ||
        reply->error() == QNetworkReply::ContentOperationNotPermittedError )
    {
        emit done( Bootstrap_Denied );
        return;
    }

    if( reply->error() != QNetworkReply::NoError )
    {
        qDebug() << reply->errorString();
        emit done( Bootstrap_UploadError );
        return;
    }

    qDebug() << "Bootstrap.zip sent to last.fm!";
    emit done( Bootstrap_Ok );
}
