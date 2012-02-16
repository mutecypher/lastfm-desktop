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

#include <QCoreApplication>

#ifdef Q_OS_WIN32
#include <client/windows/handler/exception_handler.h>
#include <client/windows/sender/crash_report_sender.h>
#endif

#include <lastfm/misc.h>

#include "CrashReporter.h"

#ifdef Q_OS_WIN32
bool
FilterCallback(void* context, EXCEPTION_POINTERS* exinfo, MDRawAssertionInfo* assertion)
{
    return true;
}


bool
MinidumpCallback(const wchar_t* dump_path, const wchar_t* minidump_id, void* context, EXCEPTION_POINTERS* exinfo, MDRawAssertionInfo* assertion, bool succeeded)
{
    google_breakpad::CrashReportSender* crashReportSender = new google_breakpad::CrashReportSender( QString( "" ).toStdWString() );

    std::map<std::wstring, std::wstring> parameters;

    std::wstring yeah;

    google_breakpad::ReportResult result = crashReportSender->SendCrashReport( QString( "http://oops.last.fm/report/add" ).toStdWString(), parameters, dump_path, &yeah);

    return true;
}
#endif

unicorn::CrashReporter::CrashReporter(QObject *parent) :
    QObject(parent)
{
#ifdef Q_OS_WIN32
    google_breakpad::ExceptionHandler* handler = new google_breakpad::ExceptionHandler( lastfm::dir::logs().absolutePath().toStdWString(),
                                                                                       FilterCallback,
                                                                                       MinidumpCallback,
                                                                                       this,
                                                                                       google_breakpad::ExceptionHandler::HANDLER_ALL);
#endif
}


unicorn::CrashReporter::~CrashReporter()
{

}
