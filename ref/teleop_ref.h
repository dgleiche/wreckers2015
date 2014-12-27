#ifndef TELEOP_REF
#define TELEOP_REF

#include "common_ref.h"

//Function prototypes
//This function uses data from the joystick to determine the motion of the robot wheels, and by extension the robot itself. It consists of three input values:
//v_x = requested motion in the X direction
//v_y = requested motion in the Y direction
//rov_x = requested rotational motion
//mVals *calcJoyMove(float v_x, float v_y, float w_input, float w_observed);

mVals *calcJoyMove(float tX, float tY, float rotX);

float find_ang(float x, float y);
float adjust_ang(float ang, float x, float y);

//Yes I know Funcs in the H are weird but that's RobotC for ya
//yooz proper gramma foo
//Functions to calc move code
//Joystick Move


//float theta_net = 0; //the net angular displacement of the robot measured from the beginning of the field-referential rotation
//float theta_0 = 0; //theta_0 and 1 are used to store values for numerical integration
//float t_ms = 0; //counter of number of times the function is run (it is updated every millisecond)

//float maxFour(float a, float b, float c, float d) {
//	//Max a b
//float maxVal = (a > b) ? a : b;

//	//c
//maxVal = (maxVal > c) ? maxVal : c;

//	//d
//maxVal = (maxVal > d) ? maxVal : d;

//	return maxVal;
//}

//v_x = translate x
//v_y = translate y
//w_input = rotation
//w_observed = gyro
//mVals *calcJoyMove(float v_x, float v_y, float w_input, float w_observed) {
//	float w = (w_input / 128) * 100; v_x = (v_x / 128) * 100; v_y = (v_y / 128) * 100; //This rescales the input to a 0-100 scale.

//	float FLwheel, FRwheel, BLwheel, BRwheel; //FLwheel referes to the front left wheel, and likewise for the rest.
//	float theta_robot; //The desired angle of motion, from the perspective of the robot – used for rotations which keep the robot moving with a constant velocity
//	float v_theta; float v_abs; //The desired angle of motion, from the perspective of the field – calculated from v_x and v_y; and the magnitude of the velocity

//	if (w_input == 0) {

//		if (v_x == 0 && v_y == 0) {
//			FLwheel = 0;
//			FRwheel = 0;
//			BLwheel = 0;
//			BRwheel = 0;
//		}
//		else {
//			FLwheel = (v_y + v_x) / sqrt(2);
//			FRwheel = (v_y - v_x) / sqrt(2);
//			BLwheel = (v_y - v_x) / sqrt(2);
//			BRwheel = (v_y + v_x) / sqrt(2);
//			}
//  theta_net = 0; t_ms = 0;
//	}
//	else {
//		if (v_x == 0 && v_y == 0) {
//			FLwheel = w;
//			FRwheel = -w;
//			BLwheel = w;
//			BRwheel = -w;
//      theta_net = 0; t_ms = 0;
//		}
//		else {

//			if (t_ms == 0) {
//				theta_net = 0;
//				theta_0 = w_observed;
//			}
//			else {
//				theta_net += (theta_0 + w_observed)/2000;
//				theta_0 = w_observed;
//			}

//			v_theta = atan(-v_x/v_y);
//			v_abs = sqrt(pow(v_x,2) + pow(v_y, 2));

//			theta_robot = v_theta - theta_net;

//			FLwheel = w + v_abs * (cos(theta_robot) - sin(theta_robot));
//			FRwheel = -w + v_abs * (cos(theta_robot) + sin(theta_robot));
//			BLwheel = w + v_abs * (cos(theta_robot) - sin(theta_robot));
//			BRwheel = -w + v_abs * (cos(theta_robot) + sin(theta_robot));

//			if (FLwheel > 100 || FRwheel > 100 || BLwheel > 100 || BRwheel > 100) {
//				float max = maxFour(FLwheel,FRwheel,BLwheel,BRwheel);
//				FLwheel /= max; FRwheel /= max; BLwheel /= max; BRwheel /= max;
//			}
//		}
//	}

//	return setMVals(FLwheel, FRwheel, BLwheel, BRwheel);
//}


mVals *calcJoyMove(float tX, float tY, float rotX) {
	int rotMax = (rotX < 0) ? -128 : 127;
	float magRot = (rotX / rotMax) * 100;

	//rVals *rot;
	float rFL, rFR, rBL, rBR;
	if (rotX < 0) {
		//rot = setRVals(-magRot, magRot, -magRot, magRot);
		rFL = -magRot; rFR = magRot; rBL = -magRot; rBR = magRot;
	} else if (rotX > 0) {
		//rot = setRVals(magRot, -magRot, magRot, -magRot);
		rFL = magRot; rFR = -magRot; rBL = magRot; rBR = -magRot;
	} else {
		rFL = 0; rFR = 0; rBL = 0; rBR = 0;
	}

	//Find max vals for tX and tY
	float max_x, max_y;

	max_x = (tX > 0) ? 127 : -128;

	max_y = (tY > 0) ? 128 : -127;

	float ang = find_ang(tX, tY);
	ang = adjust_ang(ang, tX, tY);

	float max_hyp;
	float real_hyp = sqrt(pow(tX,2) + pow(tY,2));



	if (ang != PI/2 && ang != 3*PI/2)
	{
		if ((ang <= PI/4 && ang >= 0) || (ang >= 3*PI/4 && ang <= PI) || ( ang > PI && ang <= 5*PI/4 ) || (ang > 7*PI/4 && ang < 2*PI))
		{
			max_hyp = (max_x / cos(ang));
		}
		else if ((ang >= PI/4 && ang < PI/2) || (ang > PI/2 && ang < 3*PI/4 ) || ( ang > 5*PI/4 && ang <= 3*PI/2 ) || (ang >= 3*PI/2 && ang < 7*PI/4))
		{
			max_hyp = (max_y / sin(ang));
		}
	}
	else
	{
		max_hyp = max_y;
	}

	float mag_power_translational = abs((real_hyp / max_hyp) * 100);

	mVals *m;

 	if ( ang >= 0 && ang <= (PI/2) )
 	{
 		//angle is in Q1
 		m = setMVals(mag_power_translational, ((4/PI)*(ang) - 1) * mag_power_translational, ((4/PI)*(ang) - 1) * mag_power_translational, mag_power_translational);
 	}
 	else if ( ang > (PI/2) && ang <= PI )
 	{
 		//angle is in Q2
 		m = setMVals(((-4/PI)*(ang) + 3)* mag_power_translational, mag_power_translational, mag_power_translational, ((-4/PI)*(ang) + 3)* mag_power_translational);
 	}
 	else if ( ang > PI && ang <= ((3*PI) / 2) )
 	{
 		//angle is in Q3
 		m = setMVals(-mag_power_translational, ((-4/PI)*(ang) + 5) * mag_power_translational, ((-4/PI)*(ang) + 5) * mag_power_translational, -mag_power_translational);
 	}
 	else if ( ang > ((3*PI) / 2) )
 	{
 		//angle is in Q4
 		m = setMVals(((4/PI)*(ang) - 7) * mag_power_translational, -mag_power_translational, -mag_power_translational, ((4/PI)*(ang) - 7) * mag_power_translational);
 	}

 	//Add the SWERVE!
 	if(real_hyp > 20 && abs(rotX) < 25)
	{
		return m;
  }
  else if(real_hyp <= 20 && abs(rotX) >= 25)
	{
		return setMVals(rFL, rFR, rBL, rBR);
		//return setMVals(rot->fl, rot->fr, rot->bl, rot->br);
	}
 	if(real_hyp > 20 && abs(rotX) > 25)
	{
	  float mag_power_total = (magRot + mag_power_translational);
	  float trans_ratio = (mag_power_translational/mag_power_total);
	  float rot_ratio = (magRot/mag_power_total);

	 // m->fl = (m->fl * trans_ratio) + (rot->fl * rot_ratio);
	 // m->fr = (m->fr * trans_ratio) + (rot->fr * rot_ratio);
	 // m->bl = (m->bl * trans_ratio) + (rot->bl * rot_ratio);
	 // m->br = (m->br * trans_ratio) + (rot->br * rot_ratio);

	   m->fl = (m->fl * trans_ratio) + (rFL * rot_ratio);
	   m->fr = (m->fr * trans_ratio) + (rFR * rot_ratio);
	   m->bl = (m->bl * trans_ratio) + (rBL * rot_ratio);
	   m->br = (m->br * trans_ratio) + (rBR * rot_ratio);


	 	return m;
	 }

	 return m;

}

float find_ang(float x, float y)
{
	float ang;

	//////////////////////////////////////////////////
	//if joy directly to the right, make angle 0
  if( y == 0 && x >= 0 )
	{
		ang = 0;
	}
	//if joy directly to the left make angle pi
	else if ( y == 0 && x < 0)
	{
		ang = PI;
	}
	//if joy directly up make angle pi/2
	else if ( x == 0 && y > 0)
	{
		ang = PI/2;
	}
	//if joy directly down make angle 3pi/2
	else if ( x == 0 && y < 0)
	{
		ang = (3*PI)/2;
	}
	///////////////////////////////////////////////////

	//else actually find the angle of the joystick in its quadrant using trig -- never greater than pi/2
	else
	{
		ang = atan(y/x);
	}

	return ang;
}

float adjust_ang(float ang, float x, float y)
{
  ///////////////////////////////////////////////////
	//if joy is in first quadrant, angle is fine as is

	//if joy is in second or third quadrant add pi
	if (x < 0 && y != 0)
	{
		ang += PI;
	}
	//if joy is in fourth quadrant add 2 pi
	else if ( x > 0 && y < 0)
	{
		ang += (2*PI);
	}

	return ang;
}

#endif
