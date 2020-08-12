package com.example.smartwatchcompanionapp;

import android.app.Notification;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.service.notification.NotificationListenerService;
import android.service.notification.StatusBarNotification;
import android.util.Log;

/* Notification listener service
handles obtaining the notifications from the android device and formats the results
into a string which can be read in MainActivity
 */
public class NLService extends NotificationListenerService {

    //maximum amount of text from the BigText field to convert to a string
    //lets say you have a gmail email on your notification bar, this will show the below
    //number of characters from the body of the email.
    final static int maxBigTextLength = 240;

    //intents
    public final static String NOTIFICATION_ACTION = "com.companionApp.NOTIFICATION_LISTENER_EXAMPLE";
    public final static String GET_NOTIFICATION_INTENT = "com.companionApp.NOTIFICATION_LISTENER_SERVICE_EXAMPLE";

    private String TAG = this.getClass().getSimpleName();
    private NLServiceReceiver nlservicereciver;

    @Override
    public void onCreate() {
        super.onCreate();
        nlservicereciver = new NLServiceReceiver();
        IntentFilter filter = new IntentFilter();
        filter.addAction(GET_NOTIFICATION_INTENT);
        registerReceiver(nlservicereciver, filter);
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        unregisterReceiver(nlservicereciver);
    }

    @Override
    public void onNotificationPosted(StatusBarNotification sbn) {
        Log.i(TAG, "**********  onNotificationPosted");
        Log.i(TAG, "ID :" + sbn.getId() + "t" + sbn.getNotification().tickerText + "t" + sbn.getPackageName());
        Intent i = new Intent(NOTIFICATION_ACTION);
//        i.putExtra("notification_event", "onNotificationPosted :" + sbn.getPackageName() + "n");
        i.putExtra("notification_event", "");
        sendBroadcast(i);

    }

    @Override
    public void onNotificationRemoved(StatusBarNotification sbn) {
        Log.i(TAG, "********** onNotificationRemoved");
        Log.i(TAG, "ID :" + sbn.getId() + "t" + sbn.getNotification().tickerText + "t" + sbn.getPackageName());
        Intent i = new Intent(NOTIFICATION_ACTION);
//        i.putExtra("notification_event", "onNotificationRemoved :" + sbn.getPackageName() + "n");
        i.putExtra("notification_event", "");
        sendBroadcast(i);
    }


    //NLServiceReceiver is where the actual notification data is received,
    //we broadcast an intent to obtain notifications, the results are received here and then
    //some formatting is done so that the results are more pleasant to look at
    class NLServiceReceiver extends BroadcastReceiver {

        @Override
        public void onReceive(Context context, Intent intent) {
            Log.i("inform", "NLServiceReciever has received a broadcast");
            if (intent.getStringExtra("command").equals("clearall")) {
                NLService.this.cancelAllNotifications();
            } else if (intent.getStringExtra("command").equals("list")) {
                Log.i("inform", "Processing Request to list notifications");
                Intent i1 = new Intent(NOTIFICATION_ACTION);
                i1.putExtra("notification_event", "");
                sendBroadcast(i1);
                for (StatusBarNotification sbn : NLService.this.getActiveNotifications()) {
                    Intent i2 = new Intent(NOTIFICATION_ACTION);
                    //reference for pulling information out of notification objects http://gmariotti.blogspot.com/2013/11/notificationlistenerservice-and-kitkat.html
                    if (!getAppNameFromPkgName(context, sbn.getPackageName()).equals("Spotify")) {
                        try {
                            //parse the data out of the statusbar notification object and format it into a string
                            String data = ifNotNull(getAppNameFromPkgName(context, sbn.getPackageName())) + "," //this comma is a feature
                                    + ifNotNull(sbn.getNotification().extras.getString(Notification.EXTRA_TITLE)).replace("\n", "").replace(";", ",") + ";"
                                    + ifNotNull(sbn.getNotification().extras.getString(Notification.EXTRA_TEXT)).replace("\n", "").replace(";", ",") + ";"
                                    + ifNotNull(sbn.getNotification().extras.getString(Notification.EXTRA_INFO_TEXT)).replace("\n", "").replace(";", ",") + ";"
                                    + ifNotNull(sbn.getNotification().extras.getString(Notification.EXTRA_SUB_TEXT)).replace("\n", "").replace(";", ",") + ";"
                                    + ifNotNull(sbn.getNotification().extras.getString(Notification.EXTRA_TITLE_BIG)).replace("\n", "").replace(";", ",") + ";";

                            //remove non-ascii characters, i guess if you want emoji on your other device then keep this
                            data = data.replaceAll("[^\\p{ASCII}]", "");
                            try {
                                if (sbn.getNotification().category.equals(Notification.CATEGORY_EMAIL)) {
                                    data += shortenString(sbn.getNotification().extras.getCharSequence("android.bigText")).replace("\n", "").replace(";", ",");
                                } else if (sbn.getNotification().category.equals(Notification.CATEGORY_MESSAGE)) {
                                    data += ifNotNull(sbn.getNotification().extras.getString(Notification.EXTRA_MESSAGES));
                                }
                            } catch (Exception e) {

                            }
                            i2.putExtra("notification_event", data + "\n");
                            sendBroadcast(i2);
                        } catch (Exception e) {
                            Log.e("inform", "Could not parse data for: " + getAppNameFromPkgName(context, sbn.getPackageName()) + " due to " + e.getMessage());
                        }
                    }
                }
                Intent i3 = new Intent(NOTIFICATION_ACTION);
                i3.putExtra("notification_event", "");
                sendBroadcast(i3);
            }
        }

    }

    public static String shortenString(CharSequence s) {
        if (s.length() > maxBigTextLength) {
            return s.toString().substring(0, maxBigTextLength) + "...";
        } else {
            return s.toString();
        }
    }


    public static String convToString(CharSequence[] c) {
        String ret = "";
        if (c != null) {
            for (int a = 0; a < c.length; a++) {
                ret += c[a].toString();
            }
        }
        return ret;
    }

    //this just cleans code above a bit. i don't need to send a bunch of "null" over bluetooth
    public static String ifNotNull(String str) {
        if (str != null) {
            return str;
        } else {
            return "";
        }

    }


    //https://stackoverflow.com/questions/5841161/get-application-name-from-package-name
    public static String getAppNameFromPkgName(Context context, String Packagename) {
        try {
            PackageManager packageManager = context.getPackageManager();
            ApplicationInfo info = packageManager.getApplicationInfo(Packagename, PackageManager.GET_META_DATA);
            String appName = (String) packageManager.getApplicationLabel(info);
            return appName;
        } catch (PackageManager.NameNotFoundException e) {
            e.printStackTrace();
            return "";
        }
    }

}