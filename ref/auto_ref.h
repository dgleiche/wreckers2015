#ifndef AUTO_REF
#define AUTO_REF

#include "common_ref.h"

//Resets the initial rot speed
//Returns the current rotation
float resetGyro() {
	time1[T1] = 0;
	return HTGYROreadRot(gyro);
}

//Updates the heading
//Returns the updated heading
float updateGyro(float heading, float initialRotSpeed) {
	float rotSpeed = HTGYROreadRot(gyro);
	float dt = time1[T1];//Update time that has passed

	if((rotSpeed != initialRotSpeed) || dt >= 20) {
		print(heading);
		heading += (initialRotSpeed * (dt/1000)); //updates the heading
	}

	return heading;
}

#endif
