### Usage build-release.rb <version> --deltas <version> <version>
# the deltas should be in a zip in the filder under bin with the expected filename format
# example /_bin/<version>/Last.fm-<version>

# find the current version from the plist.info?
#version = ARGV[0]

# TODO: find out the app version from the app's plist.info
$version = '2.1.16'

# TODO: get the version numbers from the argument list
$deltas = ['2.1.14', '2.1.15']

if ( ARGV.include?( "--release" ) )
	$upload_folder = '/web/site/static.last.fm/client/Mac'
	$download_folder = 'http://cdn.last.fm/client/Mac'
else
	$upload_folder = '/userhome/michael/www/client/Mac'
	$download_folder = 'http://users.last.fm/~michael/client/Mac'
end


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
	puts "unzipping #{$version}"
	system "tar xjf #{$version}/Last.fm-#{$version}.tar.bz2 -C #{$version}"

	$deltas.each do |delta|
		# unzip the old version (try both compression formats)
		puts "unzipping #{delta}"
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

def upload_files
	# scp the main zip file
	# scp all the deltas
	# put them in my userhome if we are doing a test update
	system "scp _bin/#{$version}/Last.fm-#{$version}.tar.bz2 badger:#{$upload_folder}"

	$deltas.each do |delta|
		system "scp _bin/#{$version}/Last.fm-#{$version}-#{delta}.delta badger:#{$upload_folder}"
	end
end

def generate_appcast_xml
	## sign the zip file and deltas
	item = "<item>\n"
	item << "\t<title>#{$version}</title>\n"
	item << "\t<pubDate></pubDate>\n"

	version_sig = `ruby admin/dist/mac/sign_update.rb _bin/#{$version}/Last.fm-#{$version}.tar.bz2 admin/dist/mac/dsa_priv.pem`
	version_size = `du _bin/#{$version}/Last.fm-#{$version}.tar.bz2`.split[0]

	item << "\t<enclosure sparkle:version=\"#{$version}\" url=\"#{$download_folder}/Last.fm-#{$version}.tar.bz2\" length=\"#{version_size}\" type=\"application/octet-stream\" sparkle:dsaSignature=\"#{version_sig}\"/>\n"
	item << "\t<sparkle:deltas>\n"

	$deltas.each do |delta|
		delta_sig = `ruby admin/dist/mac/sign_update.rb _bin/#{$version}/Last.fm-#{$version}-#{delta}.delta admin/dist/mac/dsa_priv.pem`
		delta_du = `du _bin/#{$version}/Last.fm-#{$version}-#{delta}.delta`.split[0]
		item << "\t<enclosure url=\"#{$download_folder}/Last.fm-#{$version}-#{delta}.delta\" sparkle:version=\"#{$version}\" sparkle:deltaFrom=\"#{delta}\" length=\"#{delta_du}\" type=\"application/octet-stream\" sparkle:dsaSignature=\"#{delta_sig}\"/>\n"
	end

	item << "\t</sparkle:deltas>\n"
	item << "\t<description></description>\n"
	item << "</item>\n"

	puts item
end


# run all the things
#clean
build
copy_plugin
create_zip
create_deltas
upload_files
generate_appcast_xml

