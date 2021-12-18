// //checks wakeup conditions and determines whether or not to activate the watch.
// boolean wakeupCheck()
// {
//     //get the wakeup reason, if it's a touch interrupt we'll know from this
//     wakeup_reason = esp_sleep_get_wakeup_cause();

//     //read accelerometer once, less ADC reads will speed things up slightly. (not polling the ADC for every if statement)
//     int yAccel = readYAccel();
//     int zAccel = readZAccel();

//     /*accelerometer wakeup algorithim implements a state machine in order to determine when to wakeup
//     S0 -> S1 - watch is perpendicular to the ground
//     S1 -> S2 - watch is perpendicular to the ground
//     S2 -> S3 - watch is perpendicular to the ground (Stay in state 3 as long as watch is in this position)
//     S3 -> S4 - either watch screen is facing up or watch screen is neither up or down.
//     S4 -> S5 - watch is facing up
//     S5 -> S6 - watch is facing up
//     S6 -> S7 - watch is facing up
//     S7 - activation of watch from accelerometer data.
//     State machine goes to S0 if the conditions to move to the next state are not met.
//   */
//     //check for dip (if user is holding wrist out so screen is perpendicular to the ground)
//     if (accelWakeupState < 3 &&
//         isDown(yAccel, zAccel))
//     {
//         accelWakeupState++;
//     }
//     else if (isDown(yAccel, zAccel))
//     {
//         //do nothing, we want to stay in this state for now.
//     }
//     //basically we're trying to catch if the user flicks their wrist up, sometimes the watch
//     //can come out of sleep in a way that it misinterprets the flick motion
//     else if (!isDown(yAccel, zAccel) && !isUp(yAccel, zAccel) && accelWakeupState == 3)
//     {
//         accelWakeupState++;
//     }
//     //check if watch screen is facing straight up
//     else if (accelWakeupState >= 3 &&
//              isUp(yAccel, zAccel))
//     {
//         accelWakeupState++;
//     }
//     else
//     {
//         accelWakeupState = 0;
//     }

//     boolean accelerometer_wakeup = accelWakeupState > 6;
//     if (accelerometer_wakeup)
//     {
//         accelWakeupState = 0;
//         return true;
//     }
//     //debug spam, use if working on this algorithim
//     //    printDebug("X: " + String(readXAccel()) + " Y: " + String(readYAccel()) + " Z: " + String(readZAccel()) + " millis(): " + String(millis()) + " state:" + String(accelWakeupState) );

//     return false;
// }
