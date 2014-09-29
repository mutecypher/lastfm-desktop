== Your Last.fm archive ==
This archive contains musical data you sent to Last.fm, conveniently packaged as easy-to-process formats. Use them to discover your listening habits, to bootstrap your own apps or to sync them with other data.

We're always interested to see what you're doing with your data, so make sure to let us know on the forum: http://www.last.fm/forum/21717

== Types of data ==
There are 6 different types of data in this archive, some of them less obvious. Here's a quick overview over where we got that data from and what it actually means. Some of these might be missing in your archive, which most likely means that we don't have any entries of that kind for you.

=== Scrobbles ===
Scrobbles should be quite obvious we hope. We collect them via different internal sources like the the scrobbler, our Xbox app, our mobile apps but also via third party apps like Spotify, Rdio or ThisIsMyJam.com. 

Unfortunately some of those clients used (or in some cases still use) old versions of our API, so some fields are not always filled out. Especially the albums are not always reliably set or the "application" field is empty. 

If lots of scrobbles "happened" at the same time you're most likely looking at iPod scrobbles.
 
=== Bootstraps ===
Bootstraps only exists for those who bootstrappped their Last.fm account by importing their iTunes listening history via the client. Bootstraps don't have timestamps (because we don't know when you listened to them), but they have a "count" field that shows how often you had listened to that track at the point you bootstrapped your profile.

=== Loved tracks ===
Loved tracks are those where you clicked the love button, either through the client, the web player, on track pages, in your recent tracks list or through some third party app. If you unloved a track at some point without loving it again afterwards it won't show up in here.

=== Banned tracks ===
Banned tracks are similar to loved tracks, they are created when you click the "ban" button in the scrobbler or on the website. As for all data types in this archive if you haven't banned any tracks there won't be any file of this type.

=== Skipped tracks ===
These are all the tracks you skipped whilst listening to Last.fm radio. Not all clients provide this information, so please take this with a pinch of salt.

== Formats ==
We're trying to provide as much information as possible with your data. Therefore we're not only exporting artist, track and album name but also MusicBrainz Identifiers and both corrected and uncorrected tracks. Most likely you can just ignore uncorrected tracks and artists (they're mostly mistagged or spelled incorrectly), but in the interest of returning you your data, we’ve included them.

This archive provides the same data in two different file formats:

=== TSV ===
TSV stands for "tab-separated values" and is a format that can easily be read by Microsoft Excel or Open Office. It's basically one line per entry with fields separated with a tab-character. Open any *.tsv file in this archive with your favourite spreadsheet editor and you should be good to go. Separate fields are always wrapped in quotes to avoid confusion, something most editors pick up themselves but will sometimes require you to play around with the import settings.

=== JSON ===
We also provide the same data as JSON. JSON stands for "JavaScript Object Notation" and is quite different to TSV. It's harder to read by humans (well, in it's compressed form anyway), but much more convenient for programming. 

JSON for scrobbles are split by month to avoid them becoming too big (some of you have a LOT of scrobbles!) and to make it easier to play around with.

== FAQ ==

=== Some of the characters look strange. How can I fix this? ===
Please check that you're using UTF-8 (that’s the encoding we're using) when opening the file. 

=== What does "unixtime" mean? ===
"unixtime" is the number of seconds that have passed since Thursday, 1 January 1970, 00:00:00 UTC. We've included it because it's easier to calculate with and easier to parse by computers. You can always use the ISO representation provided, it the same information in a different representation. 

=== What are "mbid"s? ===
"MBID" stands for "MusicBrainz Identifiers". It's a universal ID that is understood by different companies and applications and therefore makes it easier to match your data against other services. You can find more information at http://musicbrainz.org/.

Please keep in mind that one artist, track or album can have more than one MBID. This archive only shows one of them for every entry and they might even change between different exports, so don't use them for comparisons. 
