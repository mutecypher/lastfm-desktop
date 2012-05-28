#!/bin/bash
# Author: Jono Cole <jono@last.fm>
#
# A tool for distributing Mac OSX bundles.
#
# Finds and copies dependant frameworks and local dylibs 
# not installed on stardard systems.
# Also ensures that all binaries have the correct paths 
# installed relative to the bundle's @executable_path

function getBundleBin {
    if echo $1|grep -q framework; then
        echo $1/`cat "$1/Contents/Info.plist" | sed -n '/CFBundleExecutable<\/key>/,/<\/string>/ s/.*<string>\(.*\)<.*/\1/p'|sed s/_debug//`
    else
        echo $1/Contents/MacOS/`cat "$1/Contents/Info.plist" | sed -n '/CFBundleExecutable<\/key>/,/<\/string>/ s/.*<string>\(.*\)<.*/\1/p'`
    fi
}

function fixFrameworks {
    echo -n F
    if [ -d "$1" ]; then
        local bin="`getBundleBin "$1"`"
    else
        local bin="$1"
    fi

    # echo Fixing Frameworks for $bin

    libs=`otool -L "$bin"|sed -n '/\/usr\/local.*/ s/^[^\/]*\([^(]*\) [^(]*([^)]*)/\1/p'`
    
    mkdir -p "$bundlePath/Contents/Frameworks"

    local lib
    for lib in $libs; do
        #ignore non-frameworks
        if echo $lib | grep -vq framework; then continue; fi

        #examples for /opt/qt/lib/QtXml.framework/Contents/QtXml

        #framework=/opt/qt/qt.git/lib/QtXml.framework
        framework=`echo $lib |sed -n 's/\(\.framework\).*/\1/p'`
        
        #frameworkLib=/Contents/QtXml
        frameworkLib=`echo $lib |sed -n 's/^.*\.framework//p'`

        #frameworkName=QtXml.framework
        frameworkName=`basename $framework`
        
        destFramework=$bundlePath/Contents/Frameworks/$frameworkName
        installFramework=@executable_path/../Frameworks/$frameworkName
        
        if [ "`basename $lib`" == "`basename $bin`" ]; then continue; fi

        if [ ! -e "$bundlePath/Contents/Frameworks/$frameworkName" ]; then 
            #cp -Rf -P /opt/qt/qt.git/lib/QtXml.framework (app name.app)/Contents/Frameworks
            cp -Rf -p $framework "$bundlePath/Contents/Frameworks"

            chmod -R u+w "$bundlePath/Contents/Frameworks"

            #install_name_tool -id /opt/qt/qt.git/lib/QtXml.framework/Contents/QtXml
            install_name_tool -id $installFramework$frameworkLib "$destFramework$frameworkLib"
        fi
        #install_name_tool -change /opt/qt/qt.git/lib/QtXml.framework/Contents/QtXml @executable_path/../Frameworks/QtXml.framework/Contents/QtXml (bin)
        install_name_tool -change $lib $installFramework$frameworkLib "$bin"
        fixLocalLibs "$destFramework"
        fixFrameworks "$destFramework"
    done
}

function fixLocalLibs {
    echo -n L
    if [ -d "$1" ]; then
        local bin=`getBundleBin "$1"`
    else
        local bin="$1"
    fi
    echo Fixing Local Lib for $bin

    local libs=`otool -L "$bin" | sed -n '/^[^\/]*$/ s/^[[:space:]]*\(.*\) (com.*/\1/p'`
    local extralibs=`otool -L "$bin" | sed -n '/\/usr\/local.*/ s/^[^\/]*\([^(]*\) [^(]*([^)]*)/\1/p'|grep -v framework`
    local moreExtralibs=`otool -L "$bin" | sed -n '/\/usr\/X11.*/ s/^[^\/]*\([^(]*\) [^(]*([^)]*)/\1/p'|grep -v framework`
    local libs="$libs $extralibs $moreExtralibs"
    local lib
    local cpPath
    for lib in $libs; do
        local libPath=$lib
        if [ ! -e $lib ]; then
            cpPath=`locateLib $lib`
        else
            cpPath=$lib
        fi
        lib=`basename $lib`

		echo $cpPath "$bundlePath/Contents/MacOS"

        cp -L -R -f -p $cpPath "$bundlePath/Contents/MacOS"

        chmod -R u+w "$bundlePath/Contents/MacOS"

        install_name_tool -id @executable_path/../MacOS/$lib "$bundlePath/Contents/MacOS/$lib"
        install_name_tool -change $libPath @executable_path/../MacOS/$lib "$bin"
        
        fixFrameworks "$bundlePath/Contents/MacOS/$lib"
        fixLocalLibs "$bundlePath/Contents/MacOS/$lib"
    done
}

function locateLib {
    for p in {$rootdir,/usr/local/lib}; do
        if [ -e $p/$1 ]; then
            echo $p/$1
            return 0
        fi
    done
    return 1
}

if [ -d '$1' ]; then
    bundlePath="$1"
else
    bundlePath=$(echo $1 | sed -E "s|^(.*)\.app.*$|\1\.app|g")
fi

rootdir=`dirname "$bundlePath"`
binPath=$bundlePath/Contents/MacOS

echo =========== Fix Local Libs ==============
if [ -d '$1' ]; then
    fixLocalLibs "$bundlePath"
else
    fixLocalLibs "$1"
fi

echo

echo =========== Fix Frameworks ==============
if [ -d '$1' ]; then
    fixFrameworks "$bundlePath"
else
    fixFrameworks "$1"
fi

echo

echo ======= Copying 3rd party frameworks ===========

cp -Rf /Library/Frameworks/Growl.framework "$bundlePath/Contents/Frameworks"
cp -Rf /Library/Frameworks/Sparkle.framework "$bundlePath/Contents/Frameworks"
cp -Rf /Library/Frameworks/Breakpad.framework "$bundlePath/Contents/Frameworks"

echo

echo ======= Copying Qt plugins ===========
mkdir -p "$bundlePath/Contents/plugins"

plugins="imageformats phonon_backend sqldrivers"

for plugin in $plugins; do
    if [ -d /Developer/Applications/Qt/plugins/ ]; then
        pluginDir=/Developer/Applications/Qt/plugins
    else
        pluginDir=`qmake --version |sed -n 's/^.*in \(\/.*$\)/\1/p'`/../plugins
    fi
    cp -R $pluginDir/$plugin "$bundlePath/Contents/plugins"
    for i in "$bundlePath"/Contents/plugins/$plugin/*; do
        fixFrameworks "$i"
        fixLocalLibs "$i"
        echo -n P
    done
    echo
done

echo ======= Copying vlc plugins ===========

mkdir -p "$bundlePath/Contents/plugins"

accessPlugins="libaccess_http_plugin.dylib"

for plugin in $accessPlugins; do
    pluginDir="/usr/local/lib/vlc/plugins/access"

    mkdir -p "$bundlePath/Contents/plugins/access"
    cp -RL $pluginDir/$plugin "$bundlePath/Contents/plugins/access"
    chmod -R u+w "$bundlePath/Contents/plugins/access"
    
    fixFrameworks "$bundlePath/Contents/plugins/access/$plugin"
    fixLocalLibs "$bundlePath/Contents/plugins/access/$plugin"
    echo
done

audioFilterPlugins='liba52tofloat32_plugin.dylib liba52tospdif_plugin.dylib libaudio_format_plugin.dylib libaudiobargraph_a_plugin.dylib libchorus_flanger_plugin.dylib libconverter_fixed_plugin.dylib libdolby_surround_decoder_plugin.dylib libdtstofloat32_plugin.dylib libdtstospdif_plugin.dylib libequalizer_plugin.dylib libheadphone_channel_mixer_plugin.dylib libmono_plugin.dylib libmpgatofixed32_plugin.dylib libnormvol_plugin.dylib libparam_eq_plugin.dylib libscaletempo_plugin.dylib libsimple_channel_mixer_plugin.dylib libspatializer_plugin.dylib libtrivial_channel_mixer_plugin.dylib libugly_resampler_plugin.dylib'

for plugin in $audioFilterPlugins; do
    pluginDir="/usr/local/lib/vlc/plugins/audio_filter"

    mkdir -p "$bundlePath/Contents/plugins/audio_filter"
    cp -RL $pluginDir/$plugin "$bundlePath/Contents/plugins/audio_filter"
    chmod -R u+w "$bundlePath/Contents/plugins/audio_filter"
    
    fixFrameworks "$bundlePath/Contents/plugins/audio_filter/$plugin"
    fixLocalLibs "$bundlePath/Contents/plugins/audio_filter/$plugin"
    echo
done

audioOutputPlugins='libauhal_plugin.dylib'

for plugin in $audioOutputPlugins; do
    pluginDir="/usr/local/lib/vlc/plugins/audio_output"

    mkdir -p "$bundlePath/Contents/plugins/audio_output"
    cp -RL $pluginDir/$plugin "$bundlePath/Contents/plugins/audio_output"
    chmod -R u+w "$bundlePath/Contents/plugins/audio_output"
    
    fixFrameworks "$bundlePath/Contents/plugins/audio_output/$plugin"
    fixLocalLibs "$bundlePath/Contents/plugins/audio_output/$plugin"
    echo
done

audioMixerPlugins='libfloat32_mixer_plugin.dylib libspdif_mixer_plugin.dylib libtrivial_mixer_plugin.dylib'

for plugin in $audioMixerPlugins; do
    pluginDir="/usr/local/lib/vlc/plugins/audio_mixer"

    mkdir -p "$bundlePath/Contents/plugins/audio_mixer"
    cp -RL $pluginDir/$plugin "$bundlePath/Contents/plugins/audio_mixer"
    chmod -R u+w "$bundlePath/Contents/plugins/audio_mixer"
    
    fixFrameworks "$bundlePath/Contents/plugins/audio_mixer/$plugin"
    fixLocalLibs "$bundlePath/Contents/plugins/audio_mixer/$plugin"
    echo
done

codecPlugins="libavcodec_plugin.dylib libmpeg_audio_plugin.dylib"

for plugin in $codecPlugins; do
    pluginDir="/usr/local/lib/vlc/plugins/codec"

    mkdir -p "$bundlePath/Contents/plugins/codec"
    cp -RL $pluginDir/$plugin "$bundlePath/Contents/plugins/codec"
    chmod -R u+w "$bundlePath/Contents/plugins/codec"
    
    fixFrameworks "$bundlePath/Contents/plugins/codec/$plugin"
    fixLocalLibs "$bundlePath/Contents/plugins/codec/$plugin"
    echo
done

echo ======= creating qt.conf ===========
qtconf=$bundlePath/Contents/Resources/qt.conf
echo [Paths] > "$qtconf"
echo Plugins = ../plugins >> "$qtconf"
