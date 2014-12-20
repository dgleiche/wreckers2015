#ifndef TELEOP_REF
#define TELEOP_REF

#include "common_ref.h"

//Function prototypes
//This function uses data from the joystick to determine the motion of the robot wheels, and by extension the robot itself. It consists of three input values:
//v_x = requested motion in the X direction
//v_y = requested motion in the Y direction
//rov_x = requested rotational motion
//mVals *calcJoyMove(float v_x, float v_y, float w_input, float w_observed);

//mVals *calcJoyMove(float tX, float tY, float rotX);

float find_ang(float x, float y);
float adjust_ang(float ang, float x, float y);

mVals *calcJoyY(float x, float y) { //In short calc the vertical movement
	bool moveRight = (x > 0) ? true : false; //if true motors should be positive
	mVals *mVals;
	mVals = setMVals(y, y, y, y); //Default value guard

	x = abs(x);
  mVals->fr += x; //since right will be greater than left (this assumes moveRight is true -- if false will be dealt w/ later
  mVals->br += x;
  mVals->fl -= x; //make movement proportional. Dont ask me why/how this works, lots of trial and error
  mVals->bl -= x;

	if (moveRight) { //Inverse 'em to move opposite direction
		float oldFrLeft = mVals->fl;
		float oldFrRight = mVals->fr;
		float oldBkLeft = mVals->bl;
		float oldBkRight = mVals->br;

		mVals = setMVals(oldFrRight, oldFrLeft, oldBkRight, oldBkLeft); //Setting the left right
	}

	//Guard the motor maximums (they should be no greater than 100)
	if (mVals->fr > 100) mVals->fr = 100;
  if (mVals->fr < -100) mVals->fr = -100;

  if (mVals->fl < -100) mVals->fl = -100;
  if (mVals->fl > 100) mVals->fl = 100;

  if (mVals->bl < -100) mVals->bl = -100;
  if (mVals->bl > 100) mVals->bl = 100;

  if (mVals->br < -100) mVals->br = -100;
  if (mVals->br > 100) mVals->br = 100;

	return mVals;
}

mVals *calcJoyX(float x, float y) { //In short calculate the lateral movement
	bool moveRight = (x > 0) ? true : false; //if true motors should be positive
	mVals *mVals;
	//Set a default val
	mVals = setMVals(y, y, y, y);

	//So we dont get negative vals
	x = abs(x);
  mVals->fr += x; //since right will be greater than left (this assumes moveRight is true -- if false will be dealt w/ later
  if (mVals->fr > 100) mVals->fr = 100;
  if (mVals->fr < -100) mVals->fr = -100;
  mVals->fl -= x*(x/100); //make movement proportional
	if (moveRight) { //Inverse 'em
		float oldFrLeft = mVals->fl;
		float oldFrRight = mVals->fr;
		mVals->fl = oldFrRight;
		mVals->fr = oldFrLeft;
	}

	//Makes the back equal to front
	mVals->bl = mVals->fl;
	mVals->br = mVals->fr;

	return mVals;
}

mVals *joyMove(float x, float y) {
	if (abs(x) > THRESHOLD || abs(y) > THRESHOLD) {
		float degX = 128;
		float degY = 128;
		//Make proportional to the 100 scale that the motors use
		float perX = (x/degX)*100;
		float perY = (y/degY)*100;

		//Initialize an instance of mVals
		mVals *mVals;

		//If the Y value is greater than THRESHOLD than calculate movement vertically
		if (abs(perY) > THRESHOLD) mVals = calcJoyY(perX, perY);
		else mVals = calcJoyX(perX, 0);

		return mVals;
	}
	else {
		return setMVals(0, 0, 0, 0); //Stop the robot
	}
}



/*
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
*/

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
