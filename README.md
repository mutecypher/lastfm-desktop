# Build Dependencies

* Qt >= 4.8
* Ruby
* Perl
* liblastfm

You will also need depending on your chosen platform:-

## Mac OS X

### Homebrew

We recommend that you use Homebrew to install most of the dependancies:

```
brew tap lastfm/lastfmdesktop
brew install qt kde-phonon
brew install --HEAD vlc
brew install phonon-vlc
```

We install kde-phonon becuase the Phonon distributed with Qt is quite old.

Installing phonon-vlc will remove the QuickTime phonon plugin and replace
it with the VLC phonon plugin so that when we build the app we use .

### Other dependancies

You'll also need the Growl and libsparkle frameworks.

Get the latest Growl SDk from here http://code.google.com/p/growl/downloads/list

Get the latest Sparkle from here http://sparkle.andymatuschak.org/

Unzip both and put their frameworks in /Library/Frameworks/ so the build will find them.

## Windows

You will also need the latest Windows SDK. We build using Visual Studio 2008.

## Linux

* pkg-config
* taglib
* libsamplerate
* fftw3
* libmad

Possibly you'll have to play around to get a phonon that works with KDE4 and 
Qt. I can't help you there I'm afraid. An example Ubuntu installation would 
probably go:

```
sudo apt-get install qt4-dev pkg-config libtag libsamplerate libfftw3 libmad
./configure && make -j2 && make install
```

I wrote that from memory as I'm working on a Mac today. If it's wrong please
write and correct me. KTHXBAI.

Ubuntu install deps line:

```
sudo apt-get install libsqlite3-dev libqt4-sql-sqlite
```

# Build Instructions

```
qmake -r
make
```

# Run Instructions

Apps are styled using stylesheets which are found in the source directory
of the app. By default the executable path is checked for the css file on
Windows and on OSX the bundle Resource directory is checked otherwise you'll
need to tell the app where the stylesheet is, like this: 

```
./Last.fm.exe -stylesheet path/to/Last.fm.css
```

# Build Support

We support developers trying to build the source on any platform. 

Seeing as we don't provide many varieties of binary package on Linux, we also
support non-developers building from source there. However within reason!
Please check around the net a little first. Ask your friends. Demand help
from people standing at bus-stops.

Maybe check the official forum: http://www.last.fm/forum/34905

# Bugs

If you find a bug in the software, please let us know about it.

Michael Coffey<michaelc@last.fm>
Desktop App Lead Developer, Last.fm
