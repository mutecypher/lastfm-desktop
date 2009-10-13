#include <iostream>
#include <QGLWidget>
#include <QTimer>
#include <lastfm/Artist>
#include <QNetworkReply>
#include <QQueue>
#include <lastfm/ws.h>

class FullScreenWidget : public QGLWidget
{
    Q_OBJECT
public:
    FullScreenWidget( QWidget* parent = 0 )
    :QGLWidget(parent),
     m_transZ(-20.0f),
     m_transX(0.0f) {
        connect( Artist("Scarlett Johansson").getImages(), SIGNAL( finished()), SLOT( onFetchedImageUrls()));
     }

protected:
    void initializeGL()
    {
        glClearColor( 0.0, 0.0, 0.0, 0.0 );
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glEnable(GL_TEXTURE_2D);
        glShadeModel( GL_SMOOTH );
        QTimer* t = new QTimer();
        connect( t, SIGNAL( timeout()), SLOT(update()));
        t->start( 0 );

        t = new QTimer();
        connect( t, SIGNAL( timeout()), SLOT(changeImage()));
        t->start( 50 );
    }

    void resizeGL( int w, int h )
    {
        glViewport( 0, 0, (GLint)w, (GLint)h);
        glMatrixMode( GL_PROJECTION );
        glLoadIdentity();
        glFrustum( -1.0, 1.0, -1.0, 1.0, 10.0, 20.0 );
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        float colors[] = {1.0f, 1.0f, 1.0f, 1.0f};
        glLightfv( GL_LIGHT0, GL_DIFFUSE, colors );
        glMatrixMode( GL_MODELVIEW );
        glPolygonMode( GL_BACK, GL_LINE );
    }


    void paintGL()
    {
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
                 GL_ACCUM_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
        glLoadIdentity();
        float pos[] = {0.3f, -0.4f, 0.3f, 0.0f};
        glLightfv( GL_LIGHT0, GL_POSITION, pos);
        int count = 0;
        int shown = 0;
        foreach( int tex, m_textureList ) {
            float z = m_transZ - (3.0f * count);
            float x = (1.0f * count) - m_transX;
            if( z > -10.0f || shown > 3 ) {
                count++;
                continue;
            }
            shown++;
            glPushMatrix();
            glLoadIdentity();
            glTranslatef( x, 0.0f, z );
            glBindTexture( GL_TEXTURE_2D, tex );
            glBegin(GL_TRIANGLE_STRIP);
            float col[] = { 1.0f, 1.0f, 1.0f, 1.0f };
            glMaterialfv( GL_FRONT, GL_DIFFUSE, col );
            glNormal3f( 0.0f, 0.0f, 1.0f );
            glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f, 1.0f, 0.0f);
            glNormal3f( 0.0f, 0.0f, 1.0f );
            glTexCoord2f(0.0f, 1.0f); glVertex3f( -1.0f, 1.0f, 0.0f);
            glNormal3f( 0.0f, 0.0f, 1.0f );
            glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, 0.0f);
            glNormal3f( 0.0f, 0.0f, 1.0f );
            glTexCoord2f(0.0f, 0.0f); glVertex3f( -1.0f, -1.0f, 0.0f);
            glEnd();
            glPopMatrix();
            count++;
        }
    }

    float m_transZ;
    float m_transX;
    bool out;
    GLuint m_tex;
    QList<int> m_textureList;

protected slots:
    void update() 
    {
    }

    void onFetchedImageUrls()
    {
        QNetworkReply* reply = qobject_cast<QNetworkReply*>( sender());
        QList<QUrl> urls = Artist::getImages( reply );
        foreach( QUrl url, urls ) {
            QNetworkReply* r = lastfm::nam()->get( QNetworkRequest(url) );
            connect( r, SIGNAL( finished()), SLOT( onFetchedImage()));
        }
    }

    void onFetchedImage()
    {
        QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
        QImage img = QImage::fromData( reply->readAll() );
        m_textureList << bindTexture( img );
    }

    void changeImage()
    {
        m_transZ += 0.1f;
        m_transX += 0.025f;
        updateGL();
    }
};
