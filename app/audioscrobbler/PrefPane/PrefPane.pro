TEMPLATE = app

debug {
    pp.commands = xcodebuild -project PrefPane.xcodeproj -configuration Debug
}

release {
    pp.commands = xcodebuild -project PrefPane.xcodeproj -configuration Release
}

pp.target=PrefPane.prefpane

QMAKE_CLEAN = -r build

# Disable the linker:
QMAKE_LINK=@\\$$LITERAL_HASH

CONFIG-=app_bundle
CONFIG+=lib_bundle
OBJECTS_DIR=.

QMAKE_BUNDLE_EXTENSION=.prefpane
debug:TARGET=Scrobbler_debug.prefpane
release:TARGET=Scrobbler.prefpane

QMAKE_EXTRA_TARGETS = pp
PRE_TARGETDEPS = $$pp.target

