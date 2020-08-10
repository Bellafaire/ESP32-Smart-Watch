/*  This is just a small animation using some arcs. intended mostly to make the background a bit more lively */
//draws an arc originiating from the bottom left corner of the screen
void drawArc(int x, int y, int outerRadius, int thickness, int thetaStart, int arcLength,  int color) {
  //we thetaStart % 360 so that any value over 360 will be cut down to the acceptable range this way rotation can be achieved just by iterating a variable
  for (int t = thetaStart % 360; t < (thetaStart % 360) + arcLength; t++) {
    //using floats in a for loop kind of annoys me but otherwise we get gaps in the arc
    for (float l = outerRadius - thickness; l < outerRadius; l += 1) {
      //we start from thetaStart then rotate around drawing every pixel that is within our bounds
      int _x = floor((float)(l) * (sin((float)t * PI / 180.0))) + x;
      if (_x > 0 && _x < SCREEN_WIDTH) {
        int _y = floor((float)(l) * (cos((float)t * PI / 180.0))) + y;
        if (_y > 0 && _y < SCREEN_HEIGHT) {
          frameBuffer-> drawPixel(_x, _y, color);
        }
      }
    }
  }
}


#define arc_animationRadius 80
#define arc_maxVel 10
#define arcNum 24
#define arc_maxLength 220

boolean firstArcDraw = true;

int arc_colors[arcNum];
int arc_thickness[arcNum];
int arc_radius[arcNum];
int arc_length[arcNum];
float arc_positions[arcNum];
float arc_velocities[arcNum];
/* A complete self-running animation of arcs to give the homescreen it's own feel

*/
void drawCircularAnimation1(int x, int y) {
  long startAnimation = millis();
  //if its the first time this function has been called then we need to generate random position values
  if (firstArcDraw) {
    for (int a = 0; a < arcNum; a++) {
      randomSeed(89751); //i kind of don't care how random this is just an animation
      arc_positions[a] = random(360000) / 1000;
      arc_thickness[a] = 1;
      arc_radius[a] = (arc_animationRadius / arcNum) * (a + 1);
      arc_length[a] = (arc_maxLength / arcNum) * (a + 1);
      arc_velocities[a] = ((float)arc_maxVel / (float)arcNum) * (float)(a + 1);
      //generate shades of red (BGR) for the arcs
      arc_colors[a] = (0x001F / arcNum) * (arcNum - a);
    }
    firstArcDraw = false;
  }

  for (int a = 0; a < arcNum; a++) {
    drawArc(x, y, arc_radius[a], arc_thickness[a], round(arc_positions[a]), arc_length[a],  arc_colors[a]);
    arc_positions[a] += arc_velocities[a];
  }

}
