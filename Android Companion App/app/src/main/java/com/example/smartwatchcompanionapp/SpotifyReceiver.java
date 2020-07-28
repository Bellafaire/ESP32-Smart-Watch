package com.example.smartwatchcompanionapp;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

/* Taken from the spotify developer api example
https://developer.spotify.com/documentation/android/guides/android-media-notifications/
 */


public class SpotifyReceiver extends BroadcastReceiver {
    static final class BroadcastTypes {
        static final String SPOTIFY_PACKAGE = "com.spotify.music";
        static final String PLAYBACK_STATE_CHANGED = SPOTIFY_PACKAGE + ".playbackstatechanged";
        static final String QUEUE_CHANGED = SPOTIFY_PACKAGE + ".queuechanged";
        static final String METADATA_CHANGED = SPOTIFY_PACKAGE + ".metadatachanged";
    }

    private String songData = "";
    private Boolean isPlaying = false;

    public String getSongData() {
        return songData;
    }

    public String isPlaying(){
        if(isPlaying){
            return "true";
        }else{
            return "false";
        }
    }

    @Override
    public void onReceive(Context context, Intent intent) {
        // This is sent with all broadcasts, regardless of type. The value is taken from
        // System.currentTimeMillis(), which you can compare to in order to determine how
        // old the event is.
        long timeSentInMs = intent.getLongExtra("timeSent", 0L);


        String action = intent.getAction();

        if (action.equals(BroadcastTypes.METADATA_CHANGED)) {
            String trackId = intent.getStringExtra("id");
            String artistName = intent.getStringExtra("artist");
            String albumName = intent.getStringExtra("album");
            String trackName = intent.getStringExtra("track");
            int trackLengthInSec = intent.getIntExtra("length", 0);

            if (isPlaying) {
                songData = trackName + "-" + artistName;
            }
            Log.v("spotify", "Meta data changed, current songData: " + songData);

        } else if (action.equals(BroadcastTypes.PLAYBACK_STATE_CHANGED)) {
       isPlaying = intent.getBooleanExtra("playing", false);

            // Do something with extracted information

        } else if (action.equals(BroadcastTypes.QUEUE_CHANGED)) {
            // Sent only as a notification, your app may want to respond accordingly.
        }
    }
}