#pragma config(Hubs,  S1, HTMotor,  HTMotor,  HTServo,  HTMotor)
#pragma config(Hubs,  S4, HTMotor,  none,     none,     none)
#pragma config(Sensor, S1,     ,               sensorI2CMuxController)
#pragma config(Sensor, S2,     HTSMUX1,        sensorI2CCustom)
#pragma config(Sensor, S3,     HTSMUX2,        sensorI2CCustom)
#pragma config(Sensor, S4,     ,               sensorI2CMuxController)
#pragma config(Motor,  mtr_S1_C1_1,     BL,            tmotorTetrix, openLoop, reversed)
#pragma config(Motor,  mtr_S1_C1_2,     FL,            tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C2_1,     collector,     tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C2_2,     elevatorL,     tmotorTetrix, PIDControl, encoder)
#pragma config(Motor,  mtr_S1_C4_1,     FR,            tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C4_2,     BR,            tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S4_C1_1,     elevatorR,     tmotorTetrix, PIDControl, encoder)
#pragma config(Motor,  mtr_S4_C1_2,     motorK,        tmotorTetrix, openLoop)
#pragma config(Servo,  srvo_S1_C3_1,    irServo,              tServoStandard)
#pragma config(Servo,  srvo_S1_C3_2,    armHatch,             tServoNone)
#pragma config(Servo,  srvo_S1_C3_3,    servo3,               tServoNone)
#pragma config(Servo,  srvo_S1_C3_4,    servo4,               tServoNone)
#pragma config(Servo,  srvo_S1_C3_5,    lServo,               tServoStandard)
#pragma config(Servo,  srvo_S1_C3_6,    rServo,               tServoStandard)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

//TODO: ADD TIMEOUTS TO EVERYTHING. NO JOKE. EVERYTHING.

#include "JoystickDriver.c"

#include "/ref/auto_ref.h"

/* 3rd Party Drivers */
#include "drivers/hitechnic-sensormux.h"
#include "drivers/hitechnic-gyro.h"
#include "drivers/hitechnic-irseeker-v2.h"
#include "drivers/hitechnic-colour-v2.h"
#include "drivers/lego-touch.h"
#include "drivers/lego-ultrasound.h"

/* Sensors on multiplexor */
//Multiplexor 1
const tMUXSensor gyroX = msensor_S2_1;
const tMUXSensor elevatorTouch = msensor_S2_2;
const tMUXSensor sonar = msensor_S2_3;
const tMUXSensor ir = msensor_S2_4;

//Multiplexor 2
const tMUXSensor gyroZ = msensor_S3_1;
const tMUXSensor color = msensor_S3_2;

/* Constants */
//General correction factor for auto
const int CF = 2.5;

const int DISTCENTER = 20;

/* Typedefs */
typedef enum {
	BLUE1 = 2,
	BLUE2 = 3,
	RED1 = 8,
	RED2 = 9,
	GRAY1 = 14,
	GRAY2 = 17
} Colors;

typedef enum {
	BLUE,
	RED
} TeamColors;

/* Global variables */
//headingX will store heading in the x plane while headingZ stores heading in the z plane
float headingX = 0;
float initial_rotSpeedX = 0;

///Global Variables set with prompts
TeamColors teamColor;

/* Function Prototypes */

//Overload our setMotor function to allow two different ways to input the motor value
void setMotor(mVals *m);
void setMotor(float fl, float fr, float bl, float br);

void depositBall(elevatorPositions position);

void approachCenter();

//Elevator
void elevatorMove(elevatorPositions position);
void moveElevatorDown();
void moveElevatorDist(elevatorPositions position);

//Easily set value of grabber servo
void setGrabberServo(int val);

//Quickly reset heading so we can turn and stuff using gyro
void resetHeadingX();
void updateHeadingX();

//Make sure bot goes straight
mVals *gyroFixHeading(mVals *m);

void gyroTurnLeft(int degrees);
void gyroTurnRight(int degrees);

/* Function Prototypes for actual autonomous objectives */
//Off ramp objectives
void ir0();
void ir45();

//Ramp Objectives

task initialize() {
	//Initialize components
	print("Initializing...");

	PlaySound(soundBeepBeep);

	setMotor(stopMotors());

	//Initialize the tube grabber servos up
	setGrabberServo(GRABBERDOWN);

	//Let stuff init for a second
	wait1Msec(1000);

	//Init. Gyros
	HTGYROstartCal(gyroX);

	print("Ready!");
}

/* TASKS FOR OFF RAMP AUTO */

task main()
{
	//Initialize
	//This will also start a task to asynchronously update the heading
	StartTask(initialize);

	#ifdef wait_for_start
		waitForStart();
	#endif

	ir45();
}

void elevatorMove(elevatorPositions position) {
	servo[armHatch] = ARMHATCHUP;

	switch (position) {
		case elevatorDown:
			moveElevatorDown();
			break;
		case elevatorIR:
			moveElevatorDown();
			moveElevatorDist(elevatorIR);
			break;
		case elevator120:
			moveElevatorDown();
			moveElevatorDist(elevator120);
		default:
			break;
	}
}

void moveElevatorDown() {
	while(!TSreadState(elevatorTouch)) {
		motor[elevator] = -100;
	}

	motor[elevator] = 0;
	nMotorEncoder[elevator] = 0;
}

void moveElevatorDist(elevatorPositions position) {
	while(nMotorEncoder[elevator] < (float)position) {
		motor[elevator] = 100;
	}

	motor[elevator] = 0;
}


/* FUNCTIONS FOR OFF RAMP AUTO */
void depositBall(elevatorPositions position) {
	//Raise elevator to 120
	elevatorMove(position);

	//Deposit ball
	servo[armHatch] = ARMHATCHDOWN;
	wait1Msec(2000);
	servo[armHatch] = ARMHATCHUP;
}

void approachCenter() {
	while (USreadDist(sonar) > DISTCENTER) {
		setMotor(backward(30));
	}

	//Ensure its not too close
	while (USreadDist(sonar) < DISTCENTER) {
		setMotor(forward(30));
	}

	setMotor(stopMotors());

	//Set elevator to IR level
	elevatorMove(elevatorIR);

	while(HTIRS2readDCDir(ir) > 5) {
		setMotor(strafeL(30));
	}

	setMotor(stopMotors());

	//Move back a lil to give it room
	setMotor(forward(30));
	wait1Msec(200);
	setMotor(stopMotors());
}

//When beacon is 0Deg from robot
void ir0() {
	approachCenter();
	depositBall(elevator120);
}

void ir45() {
	//Become parallel to base
	setMotor(backward(30));
	wait1Msec(300);
	setMotor(stopMotors());

	//Rotate 40 degrees
	gyroTurnRight(40);

	//Orient to center tube
	while(USreadDist(sonar) > 30) {
		setMotor(strafeL(30));
	}

	setMotor(stopMotors());

	while(USreadDist(sonar) > DISTCENTER) {
		setMotor(backward(30));
	}

	setMotor(stopMotor());
}

/* FUNCTIONS FOR RAMP AUTO */

/*
void alignToLine(Colors color, int timesToAlign) {

	int speed = 20;

	Colors color1 = (color == RED) ? (short)RED1 : (short)BLUE1;
	Colors color2 = (color == RED) ? (short)RED2 : (short)BLUE2;

	bool front_aligned = false;
	bool back_aligned = false;

	mVals motorVals;

	float timesAligned = 0;

	while (timesAligned < timesToAlign) {

		if(HTCS2readColor(colorL) == (short)GRAY1 || HTCS2readColor(colorR) == (short)GRAY2) {
			motorVals.fl = -speed;
			motorVals.fr = speed;
		}
		else if(HTCS2readColor(colorL) == color1 || HTCS2readColor(colorR) == color2) {
			motorVals.fl = speed;
			motorVals.fr = -speed;
		}
		else {
			front_aligned = true;
			motorVals.fl = 0;
			motorVals.fr = 0;
		}

		if(HTCS2readColor(colorL) == (short)GRAY1 || HTCS2readColor(colorR) == (short)GRAY2) {
			motorVals.bl = speed;
			motorVals.br = -speed;
		}
		else if(HTCS2readColor(colorL) == color1 || HTCS2readColor(colorR) == color2) {
			motorVals.bl = -speed;
			motorVals.br = speed;
		}
		else {
			back_aligned = true;
			motorVals.bl = 0;
			motorVals.br = 0;
		}

		setMotor(motorVals);

		//Repeat the function again because the robot hasn't corrected enough times yet [?: wait before second correct?]
		if(front_aligned == true && back_aligned == true) {
			front_aligned = false;
			back_aligned = false;
			timesAligned += 1;
		}

	}
} */

/* Misc. Functions */
mVals *gyroFixHeading(mVals *m) {
	//Update heading
	updateHeadingX();

	//Motor powers are adjusted by this value (based on heading and correction factor)
	float adjusted_val = headingX * CF;

	//Store corrected motor vals
	mVals newM;

  newM.fl = m->fl - adjusted_val;
	newM.fr = m->fr + adjusted_val;
	newM.bl = m->bl - adjusted_val;
	newM.br = m->br + adjusted_val;

	return newM;
}

void setMotor(mVals *m) {
	//Pass along to func with the four motor vals
	setMotor(m->fl, m->fr, m->bl, m->br);
}

void setMotor(float fl, float fr, float bl, float br) {
	motor[FL] = fl;
	motor[FR] = fr;
	motor[BL] = bl;
	motor[BR] = br;
}

void setGrabberServo(int val) {
	servo[lServo] = val;
	servo[rServo] = 255-val;
}

void resetHeadingX()
{
	time1[T1] = 0;
	initial_rotSpeedX = HTGYROreadRot(gyroX);
}

void updateHeadingX()
{
	float rotSpeed = HTGYROreadRot(gyroX);
	float dt = time1[T1];//Update time that has passed
	if((rotSpeed != initial_rotSpeedX) || dt >= 20)
	{
		print(headingX);
		headingX += (initial_rotSpeedX * (dt/1000)); //updates the heading
		resetHeadingX();
	}
}

void gyroTurnLeft(int degrees) {
	headingX = 0;
	while(abs(headingX) < degrees) {
		updateHeadingX();

		setMotor(turnLeft(30));
	}

	setMotor(stopMotors());
}

void gyroTurnRight(int degrees) {
	headingX = 0;
	while(abs(headingX) < degrees) {
		updateHeadingX();

		setMotor(turnRight(30));
	}

	setMotor(stopMotors());
}
