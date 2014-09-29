lastfm <- function(x){
        library(httr)
        library(jsonlite)
        myapp <- oauth_app("Last.fm", key="6aa4b6807474c8675a9cd4e6ef128557", secret = "300105ab6b4a78f0dc71fee075f20cde")
        sig <- sign_oauth1.0(myapp, token = "18440355-Jvr9mzGXGPCDa40l95YoPdjIzpAzUstfcpIywHSz5", token_secret = "NcSp1XiI2FrOaE6Rf7Z4CtQ2RA6SPmci66TQBJc2PtR6J")
        homeTL <- GET("https://api.twitter.com/1.1/statuses/home_timeline.json", sig)
        x <- jsonlite::fromJSON(toJSON(content(homeTL)))
        x[1,1:5]
}