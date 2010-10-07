#!/usr/bin/perl
#
# brief: This script builds a release build from scratch and packages it
#        into an installer. You need %QTDIR% and %VSDIR% set in your environment
#
#        Must be run from trunk.
#
# pre-requisites: perl, svn, tar, bzip2, 
#                 Inno Setup 5 (install to default location or set %ISDIR%)
#

use File::Path;
use File::Copy;
use File::Find;


########################################################################

my $VERSION = getVersion();

# copy the input file to a temp location and use that instead
copy('Last.fm.iss', 'Last.fm.tmp.iss') or die;
my $ISSFILE = 'Last.fm.tmp.iss';

my $QTDIR = quotemeta( $ENV{'QTDIR'} or die $! );
my $VSDIR = quotemeta( $ENV{'VSDIR'} or die $! );
$QTDIR =~ s/\\/\\\\/g; #double escape \s for the shell :(
$VSDIR =~ s/\\/\\\\/g;

########################################################################
header( "Building release and installer for Last.fm-$VERSION" );

########################################################################
print( "\nDon't forget the change log! ([Enter] to continue, [Ctrl-C] to abort)\n" );
$dummy = <STDIN>;

header( "Substituting strings in various files" );
    #sub findVersionFiles()
    #{
    #    if ($_ =~ /\.rc$/i || $_ =~ /\.manifest$/i || $_ =~ /\.iss$/i)
    #    {
    #        push( @versionFiles, $File::Find::name );
    #    }
    #}
	#find( \&findVersionFiles, "." );
	
	push( @versionFiles, $ISSFILE );
	
    updateVersion( @versionFiles );

    system( 'perl -pi".bak" -e "s/\%QTDIR\%/' . $QTDIR . '/g" ' . $ISSFILE );
    system( 'perl -pi".bak" -e "s/\%VSDIR\%/' . $VSDIR . '/g" ' . $ISSFILE );

#header( "Translations" );
#    if ($ARGV[0] eq "--jp")
#    {
#        mkdir ( "bin/data/i18n" );
#        system( "lrelease i18n/lastfm_jp.ts -qm bin/data/i18n/lastfm_jp.qm" );
#        system( "lrelease i18n/qt_jp.ts -qm bin/data/i18n/qt_jp.qm" );
#    }
#    else
#    {
#        run( "perl", "dist\\i18n.pl" ) or die $!;
#    }

header( "Installer" );
    #my $ISDIR = $ENV{'ISDIR'} or "c:\\Program Files\\Inno Setup 5";
    #$ISDIR =~ s/\\/\//g;
    #run( "$ISDIR\\iscc.exe", "$ISSFILE" ) or die $!;
	run( "c:/Program Files (x86)/Inno Setup 5/iscc.exe", $ISSFILE ) or die $!;

#header( "Building symbolstore" );
#    dumpSyms( "bin" );
#    dumpSyms( "$ENV{QTDIR}/lib" );
#    chdir( "build/syms" );
#    run( "tar", "cjf", "../../dist/Last.fm-$VERSION-win.symbols.tar.bz2", "." );

header( "done!" );
    print "To upload the symbols, issue the following command:\n";
    print "       perl dist\\build-release-win.pl --upload";



########################################################################
sub header
{
    print "\n==> $_[0]\n";
}

sub getVersion
{
    my $revision = getGitRevision();

    # Due to a MS quirk, no part of the version number can be bigger than 16 bits, so we mod it
    $revision = $revision % 0xFFFF;
    
    open DATA, '../../../_version.h' or die;
    my @lines = <DATA>;
    close( DATA );

    foreach my $line (@lines)
    {
        if ( $line =~ m/^#define VERSION "(\d+\.\d+\.\d+)"/ )
        {
            #my $oldver = $1;
            my $newver = $1 . "." . $revision;

            #if ( !( $oldver eq $newver) )
            #{
            #    system( "perl -pi\".bak\" -e \"s/$oldver/$newver/g\" src/version.h" );
            #}
            return $newver;
        }
    }

    die;
}

sub getGitRevision
{
    @output = `git log -n 1 --oneline`;

    foreach my $line (@output)
    {
      if ( $line =~ m/^([\da-fA-F]+)\s.*$/ )
      {
          return $1;
      }
    }

    die;
}

sub updateVersion
{
    my @list = @_;
    foreach $file ( @list )
    {
        system( "perl -pi\".bak\" -e \"s/0\\.0\\.0\\.0/$VERSION/g\" $file" );
        my $versionCommas = $VERSION;
        $versionCommas =~ s/\./,/g;
        system( "perl -pi\".bak\" -e \"s/0,0,0,0/$versionCommas/g\" $file" );
        print $file . "\n";
    }
}

sub run
{
    return system( @_ ) == 0;
}
########################################################################
