/*
   Copyright 2005-2010 Last.fm Ltd. 
      - Primarily authored by Jono Cole and Michael Coffey

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

#include <QApplication>
#include <QDir>
#include <QMessageBox>
#include <QDebug>
#include <QProcess>

/* This function is unused but it works so why not keep it! */
void recursiveCopy( const QDir& src, const QDir& dest )
{
    QString destDirName = dest.absolutePath() + "/" + src.dirName();
    dest.mkdir( destDirName );
    QDir destDir( destDirName );
    
    foreach( QString entry, src.entryList( QDir::AllEntries | QDir::NoDotAndDotDot, QDir::DirsLast )) {
        if( QFileInfo( src.filePath( entry )).isDir()) {
            recursiveCopy( QDir( src.filePath( entry )), destDir );
        } else {
            QFile( src.filePath( entry )).copy( destDir.filePath( entry ));
        }
    }

}

/** Equivalent of rm -rf dir */
void recursiveRm( const QDir& dir )
{
    foreach( QString entry, dir.entryList( QDir::AllEntries | QDir::NoDotAndDotDot, QDir::DirsLast )) {
        if( QFileInfo( dir.filePath( entry )).isDir()) {
            recursiveRm( QDir( dir.filePath( entry )));
        } else {
            QFile( dir.filePath( entry )).remove();
        }
    }
    QDir( dir.path() + "/.." ).rmdir( dir.dirName());
}


int main( int argc, char* argv[] )
{
    QApplication app( argc, argv );
    QDir bundleDir( QCoreApplication::applicationDirPath() + "/../../" );
    bundleDir.makeAbsolute();

    if( !bundleDir.dirName().endsWith( ".app" ))
        return 1;

    QDir resourceDir( bundleDir.path() + "/Contents/Resources" );
    resourceDir.makeAbsolute();

    if( !resourceDir.exists() )
        return 1;

    if( QDir( bundleDir.path() + "/.." ).absolutePath() != "/Applications" ) {
       int button = QMessageBox( QMessageBox::Information, 
                                 QObject::tr( "Installing Last.fm" ), 
                                 QObject::tr( "Last.fm has not been moved to your application folder. "
                                              "Clicking Ok will do this for you." ), 
                                 QMessageBox::Ok| QMessageBox::Cancel
                               ).exec(); 
       qDebug() << "Button: " << button;
       if( QMessageBox::Cancel == button ) 
            return 0;
    }

    QStringList appList = resourceDir.entryList( QStringList() << "*.app", QDir::Dirs );
    if( appList.isEmpty())
        return 1;

    QDir destDir( "/Applications/Last.fm" );
    destDir.mkdir( "/Applications/Last.fm" );

    foreach( QString app, appList ) {
        QDir appDir(  resourceDir.filePath( app ) );
        resourceDir.rename( appDir.dirName(), destDir.path() + "/" + appDir.dirName()); 
    }
    
 
    recursiveRm( bundleDir );

    QProcess::startDetached( "/Applications/Last.fm/Audioscrobbler.app/Contents/MacOS/audioscrobbler" );

    return 0;
}
