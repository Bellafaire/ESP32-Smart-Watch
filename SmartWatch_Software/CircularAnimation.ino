/*  This is just a small animation using some arcs. intended mostly to make the background a bit more lively */
//draws an arc originiating from the bottom left corner of the screen
void drawArc(int x, int y, int outerRadius, int thickness, int thetaStart, int arcLength,  int color) {
  //we thetaStart % 360 so that any value over 360 will be cut down to the acceptable range this way rotation can be achieved just by iterating a variable
  for (int t = thetaStart % 360; t < (thetaStart % 360) + arcLength; t++) {
    //using floats in a for loop kind of annoys me but otherwise we get gaps in the arc
    for (float l = outerRadius - thickness; l < outerRadius; l += .5) {
      //we start from thetaStart then rotate around drawing every pixel that is within our bounds
      int _x = floor((float)(l) * (sin((float)t * PI / 180.0))) + x;
      int _y = floor((float)(l) * (cos((float)t * PI / 180.0))) + y;
      frameBuffer-> drawPixel(_x, _y, color);
    }
  }
}


#define animationRadius 80
#define maxVel 15
#define arcNum 8
#define maxLength 220

boolean firstArcDraw = true;

int arc_colors[] = {0xB596, 0x7BEF, 0x4A69, 0xB596, 0x7BEF, 0x4A69,0xB596, 0x7BEF, 0x4A69, 0x4A69};
int arc_thickness[] = {6, 4, 2, 2, 2, 1 , 1, 1, 1, 1};
int arc_radius[] = {30, 45, 60, 30, 45, 60, 30, 45, 60, 60};
int arc_length[] = {60, 120, 180, 60, 60, 60, 60, 60, 60, 60};
float arc_positions[] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
float arc_velocities[] = {10, 8, 4, 10, 8, 4, 4, 10, 8, 10};
/* A complete self-running animation of arcs to give the homescreen it's own feel

*/
void drawCircularAnimation1(int x, int y) {
  long startAnimation = millis();
  //if its the first time this function has been called then we need to generate random position values
  if (firstArcDraw) {
    for (int a = 0; a < arcNum; a++) {
      randomSeed(89751); //i kind of don't care how random this is just an animation
      arc_positions[a] = random(360000) / 1000;
      arc_radius[a] = (animationRadius / arcNum) * (a + 1);
      arc_length[a] = (maxLength / arcNum) * (a + 1);
      arc_velocities[a] = (maxVel / arcNum) * (a + 1);
    }
    firstArcDraw = false;
  }

  for (int a = 0; a < arcNum; a++) {
    drawArc(x, y, arc_radius[a], arc_thickness[a], round(arc_positions[a]), arc_length[a],  arc_colors[a]);
    arc_positions[a] += arc_velocities[a];
    //    if(millis() - startAnimation > 60){
    //      break;
    //    }
  }
#ifdef DEBUG
  Serial.println("Required " + String(millis() - startAnimation) + "ms to draw circles animation");
#endif

}
