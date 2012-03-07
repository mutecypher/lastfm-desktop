
### Usage build-release.rb <version> --deltas <version> <version>
# the deltas should be in a zip in the filder under bin with the expected filename format
# example /_bin/<version>/Last.fm-<version>

# find the current version from the plist.info?
#version = ARGV[0]

$version = '2.1.16'
$deltas = ['2.1.14', '2.1.15']

## Check that we are running from the root of the lastfm-desktop project
# ?

def clean
	## clean everything
	system 'make -C ../liblastfm clean'
	system 'make -C ../liblastfm distclean'
	system 'make clean'
	system 'make distclean'
end

def build
	## build everything
	system 'qmake -o ../liblastfm/Makefile  ../liblastfm/lastfm.pro CONFIG+=release'
	system 'sudo make -C ../liblastfm install'

	system 'qmake -r CONFIG+=release'
	system 'make'
end

def copy_plugin
	## copy the iTunes plugin into the bundle
	system 'cp -R _bin/Audioscrobbler.bundle _bin/Last.fm.app/Contents/MacOS/'
end

def create_zip
	## create a zip file
	Dir.chdir("_bin")
	system "rm -rf #{$version}"
	system "mkdir #{$version}"
	system "tar cjf #{$version}/Last.fm-#{$version}.tar.bz2 Last.fm.app"
	Dir.chdir("..")
end

def create_deltas
	## create any deltas
	Dir.chdir("_bin")

	# unzip the new app
	system "tar xjf #{$version}/Last.fm-#{$version}.tar.bz2 -C #{$version}"

	$deltas.each do |delta|
		# unzip the old version (try both compression formats)
		#system 'tar -xjf Last.fm-#{delta}.tar.bz2 -C #{delta}'
		system "unzip -q #{delta}/Last.fm-#{delta}.zip -d #{delta}"
		# create the delta
		system "./BinaryDelta create #{delta}/Last.fm.app #{$version}/Last.fm.app #{$version}/Last.fm-#{$version}-#{delta}.delta"
		# check that it worked
		system "./BinaryDelta apply #{delta}/Last.fm.app #{$version}/Last.fm.app #{$version}/Last.fm-#{$version}-#{delta}.delta"
		# remove the unzipped old version
		system "rm -rf #{delta}/Last.fm.app"
	end

	# delete the unzipped new app
	system "rm -rf #{$version}/Last.fm.app"

	Dir.chdir("..")
end

def generate_appcast_xml
	## sign the zip file and deltas
	puts version_sig = `ruby admin/dist/mac/sign_update.rb _bin/#{$version}/Last.fm-#{$version}.tar.bz2 admin/dist/mac/dsa_priv.pem`
	puts version_size = `du _bin/#{$version}/Last.fm-#{$version}.tar.bz2`

	$deltas.each do |delta|
		puts delta_sig = `ruby admin/dist/mac/sign_update.rb _bin/#{$version}/Last.fm-#{$version}-#{delta}.delta admin/dist/mac/dsa_priv.pem`
		puts delta_du = `du _bin/#{$version}/Last.fm-#{$version}-#{delta}.delta`
	end
end

# run all the things
#clean
build
copy_plugin
create_zip
create_deltas
generate_appcast_xml
