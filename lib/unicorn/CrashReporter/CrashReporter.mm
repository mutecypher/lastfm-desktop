/*
   Copyright 2012 Last.fm Ltd.
      - Primarily authored by Michael Coffey

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

#include <Cocoa/Cocoa.h>

#include <Breakpad/Breakpad.h>

#include "CrashReporter.h"

@interface Breakpad : NSObject {
   BreakpadRef breakpad;
}

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender;
@end

@implementation Breakpad
static BreakpadRef InitBreakpad(void) {
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
  BreakpadRef breakpad = 0;
  NSDictionary *plist = [[NSBundle mainBundle] infoDictionary];
  if (plist) {
    // Note: version 1.0.0.4 of the framework changed the type of the argument
    // from CFDictionaryRef to NSDictionary * on the next line:
    breakpad = BreakpadCreate(plist);
  }
  [pool release];
  return breakpad;
}

- (void)awakeFromNib {
  breakpad = InitBreakpad();
}

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender {
  BreakpadRelease(breakpad);
  return NSTerminateNow;
}
@end

Breakpad* gBreakpad;

unicorn::CrashReporter::CrashReporter(QObject *parent) :
    QObject(parent)
{
    gBreakpad = [Breakpad new];
    [[NSApplication sharedApplication] setDelegate: gBreakpad];

    [gBreakpad awakeFromNib];

}

unicorn::CrashReporter::~CrashReporter()
{

}
