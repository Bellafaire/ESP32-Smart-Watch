package com.example.esp32_companion_app;

import android.app.job.JobInfo;
import android.app.job.JobScheduler;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.text.method.ScrollingMovementMethod;
import android.util.Log;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;

import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

/*  Using Harry1453's android-bluetooth serial library to simplify bluetooth communication
    https://github.com/harry1453/android-bluetooth-serial */

public class MainActivity extends AppCompatActivity {
    private long lastSendTime = 0;

    public static TextView currentNotification;
    public static String currentNotificationString = "";
    private NotificationReceiver nReceiver;

    public ArrayList<String> notificationHeaders, notificationText;

    //scheduling objects
    ComponentName serviceComponent;
    JobInfo.Builder builder;
    JobScheduler jobScheduler;
    private final ScheduledExecutorService scheduler = Executors.newScheduledThreadPool(2);

    private Runnable periodicSend = new Runnable() {
        @Override
        public void run() {

            builder = new JobInfo.Builder(11, serviceComponent);

            builder.setRequiredNetworkType(JobInfo.NETWORK_TYPE_ANY);

            jobScheduler = getSystemService(JobScheduler.class);
            jobScheduler.schedule(builder.build());
        }
    };

    private Runnable notificationUpdater = new Runnable() {
        @Override
        public void run() {
            getCurrentNotifications();
        }
    };



    //creation method
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        notificationHeaders = new ArrayList();
        notificationText = new ArrayList();

        setContentView(R.layout.activity_main);
        currentNotification = (TextView) findViewById(R.id.CurrentNotificationString);
        currentNotification.setMovementMethod(new ScrollingMovementMethod());

        nReceiver = new NotificationReceiver();
        IntentFilter filter = new IntentFilter();
        filter.addAction("com.kpbird.nlsexample.NOTIFICATION_LISTENER_EXAMPLE");
        registerReceiver(nReceiver, filter);
        getCurrentNotifications();

        Log.i("inform", "App starting....");


//        JobScheduler jobScheduler = (JobScheduler) getSystemService(Context.JOB_SCHEDULER_SERVICE);
        serviceComponent = new ComponentName(this, BluetoothNotificationSender.class);
        scheduler.scheduleAtFixedRate(periodicSend, 1, 5, TimeUnit.SECONDS);
        scheduler.scheduleAtFixedRate(notificationUpdater, 0, 5, TimeUnit.SECONDS);
    }


    @Override
    public void onDestroy() {
        super.onDestroy();
    }

    //menu items
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        MenuInflater inflater = getMenuInflater();
        inflater.inflate(R.menu.main_menu, menu);
        return true;
    }

    //sends notification data over bluetooth
    public void sendNotificationData() {
        //send data in callback of getting the current notifications
        if (lastSendTime + 1000 < System.currentTimeMillis()) {
            lastSendTime = System.currentTimeMillis();
//            deviceInterface.sendMessage(currentNotification.getText().toString());
        }
    }

    //updates the onscreen text view with the current notifications active on the device
    public void getCurrentNotifications() {
        currentNotification.setText("");
        currentNotificationString = "";
        notificationHeaders = new ArrayList();
        notificationText = new ArrayList();
        Intent i = new Intent("com.kpbird.nlsexample.NOTIFICATION_LISTENER_SERVICE_EXAMPLE");
        i.putExtra("command", "list");
        sendBroadcast(i);
    }

    //triggered when an item is selected from the drop down menu
    public boolean onOptionsItemSelected(MenuItem item) {

        switch (item.getItemId()) {
            case R.id.bluetooth:
                // getCurrentNotifications();
                sendNotificationData();
                return true;
            case R.id.update:
                getCurrentNotifications();
                return true;
            case R.id.format:
                currentNotification.setText(reformatNotifications(currentNotification.getText().toString()));
                return true;
            default:
                return super.onOptionsItemSelected(item);
        }


    }

    @Deprecated
    public static String reformatNotifications(String ns) {

        //alright so first we split up the strings and add them to an array list, since we're going to be filtering and sorting these in a kind of weird way
        ArrayList<String> ss = new ArrayList(); //split string

        String mod = ns;
        mod = mod.replace("Notification List", "");
        mod = mod.replace("=", "");
        String[] split = mod.split("\n");

        for (int a = 0; a < split.length; a++) {
            while (split[a].charAt(0) == ' ') {
                split[a] = split[a].substring(1);
            }
        }

        mod = "";
        for (int a = 0; a < split.length; a++) {
            mod += split[a] + "\n";
        }

        //lets see if we can count the indexes
        int highestIndex = 0;
        for (int a = 0; a < split.length; a++) {
            try {
                int currentIndex = Integer.parseInt(split[a].split(" ")[0]);
                if (currentIndex > highestIndex) {
                    highestIndex = currentIndex;
                }
            } catch (Exception e) {

            }
        }

        ArrayList<String> sl = new ArrayList();
        for (int a = 0; a < split.length; a++) {
            sl.add(split[a]);
        }

        String[] formattedNotifications = new String[highestIndex + 1];

        for (int a = 1; a < formattedNotifications.length; a++) {
            //now we need to sort out all the com. objects since these are the "app names"
            int b = 0;
            while (b < sl.size()) {
                String[] currentItem = sl.get(b).split(" ");
                if (Integer.parseInt(currentItem[0]) == a && (currentItem[1].startsWith("com") || currentItem[1].equals("android"))) {
                    formattedNotifications[a] = currentItem[1] + " - ";
                    sl.remove(b);
                }
                b++;
            }
        }

        //before we add the notification text lets get rid of the com. stuff
        for (int a = 0; a < formattedNotifications.length; a++) {
            try {
                formattedNotifications[a] = formattedNotifications[a].replace("com.google.android.gm", "Gmail");
                formattedNotifications[a] = formattedNotifications[a].replace("com.", "");
                //  formattedNotifications[a] = formattedNotifications[a].replace("android", "");
                formattedNotifications[a] = formattedNotifications[a].replace(".", "");
                formattedNotifications[a] = formattedNotifications[a].substring(0, 1).toUpperCase() + formattedNotifications[a].substring(1);
            } catch (Exception e) {
            }
        }

        for (int a = 1; a < formattedNotifications.length; a++) {
            //now we need to sort out all the com. objects since these are the "app names"
            int b = 0;
            while (b < sl.size()) {
                String[] currentItem = sl.get(b).split(" ");
                if (Integer.parseInt(currentItem[0]) == a) {
                    formattedNotifications[a] += sl.get(b).replace(currentItem[0] + " ", "");
                    sl.remove(b);
                }
                b++;
            }
        }


        String finalOutput = "";
        for (int a = 1; a < formattedNotifications.length; a++) {
            finalOutput += formattedNotifications[a] + "\n";
        }


//        return ns + "\n\n-----------------------------------------------------------------------------------------------------------------------\n\n" +
//                mod + "\n\n-----------------------------------------------------------------------------------------------------------------------\n\n"
//                + finalOutput;
        return finalOutput;

    }

    //returns a string representing the date and time
    public static String getDateAndTime() {
//        DateTimeFormatter dtf = DateTimeFormatter.ofPattern("MM/dd/yyyy HH:mm:ss");
//        LocalDateTime now = LocalDateTime.now();
//        return dtf.format(now);

        Date c = Calendar.getInstance().getTime();
        System.out.println("Current time => " + c);

        SimpleDateFormat df = new SimpleDateFormat("hh:mm:ssa dd-MM-yyyy");
        String formattedDate = df.format(c);
        return formattedDate;

    }

    //class to handle reading notifications from the device.
    class NotificationReceiver extends BroadcastReceiver {

        @Override
        public void onReceive(Context context, Intent intent) {
            MainActivity.currentNotification.setText("");

            //first we need to read the number at the start of the message
            String temp = intent.getStringExtra("notification_event");

            try {
                String[] str = temp.split(" ");

                if (str[1].contains("com.") || str[1].contains("android.")) {
                    //so its a header..

                    //figure out it's intended index
                    int index = Integer.parseInt(str[0]);

                    //if the index doesn't exist then make it exist
                    while (notificationHeaders.size() - 1 < index) {
                        notificationHeaders.add("");
                    }

                    notificationHeaders.set(index, temp.substring(str[0].length() + 1));

                } else {

                    //figure out it's intended index
                    int index = Integer.parseInt(str[0]);

                    //if the index doesn't exist then make it exist
                    while (notificationText.size() - 1 < index) {
                        notificationText.add("");
                    }

                    notificationText.set(index, temp.substring(str[0].length() + 1));

                }

                //   String temp = intent.getStringExtra("notification_event") + currentNotification.getText();
                MainActivity.currentNotification.setText(temp);
            } catch (Exception e) {
                //so some notifications won't work. usually they come along with a number that we can use as an index.
                e.printStackTrace();
            }

            String disp = getDateAndTime() + "\n";
            for (int a = 1; a < notificationHeaders.size(); a++) {
                try {
                    if (!notificationHeaders.get(a).equals("") && !notificationText.get(a).equals("")) {
                        try {
                            disp += notificationHeaders.get(a).replace("\n", "");
                            disp += " - ";
                        } catch (Exception e) {
                        }
                        try {
                            disp += notificationText.get(a).replace("\n", "");
                        } catch (Exception e) {
                        }
                        disp += '\n';
                    }
                } catch (IndexOutOfBoundsException ea) {
                }
            }

            //finally replace the titles with the actual app name for some common apps

            disp = disp.replace("com.spotify.music", "Spotify");
            disp = disp.replace("com.google.android.gm", "Gmail");
            disp = disp.replace("com.evernote", "Evernote");
            disp = disp.replace("com.dropbox.android", "Dropbox");
            disp = disp.replace("com.android.systemui", "System");


            MainActivity.currentNotification.setText(disp);
            currentNotificationString = disp;
        }
    }


}
