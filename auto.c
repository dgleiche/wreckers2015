#pragma config(Hubs,  S1, HTMotor,  HTMotor,  HTServo,  HTMotor)
#pragma config(Hubs,  S4, HTMotor,  none,     none,     none)
#pragma config(Sensor, S2,     HTSMUX1,        sensorI2CCustom)
#pragma config(Sensor, S3,     color,          sensorI2CCustom)
#pragma config(Motor,  mtr_S1_C1_1,     BL,            tmotorTetrix, openLoop, reversed)
#pragma config(Motor,  mtr_S1_C1_2,     FL,            tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C2_1,     collector,     tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C2_2,     elevatorL,     tmotorTetrix, openLoop, encoder)
#pragma config(Motor,  mtr_S1_C4_1,     FR,            tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C4_2,     BR,            tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S4_C1_1,     elevatorR,     tmotorTetrix, openLoop, reversed, encoder)
#pragma config(Motor,  mtr_S4_C1_2,     motorK,        tmotorTetrix, openLoop)
#pragma config(Servo,  srvo_S1_C3_1,    irServo,              tServoStandard)
#pragma config(Servo,  srvo_S1_C3_2,    armHatch,             tServoStandard)
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

/* Constants */
//General correction factor for auto
const int CF = 2.5;

const int DISTCENTER = 33;

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

void knockDownKickstand();

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

void gyroTurnLeft(int degrees, int power);
void gyroTurnRight(int degrees, int power);

/* Function Prototypes for actual autonomous objectives */
//Off ramp objectives
void ir0();
void ir45();
void ir90();

void elevatorMotors(int power);
void resetElevatorEncoders();

void parkAuto();

//Ramp Objectives

//Task to move elevator asynchronously
//Var to store whether elevator is moving
bool elevatorMovingAsync = false;

//Global to store proper elevator position
elevatorPositions elevatorPosition;

task moveElevatorAsync() {
	elevatorMovingAsync = true;

	servo[armHatch] = ARMHATCHUP;

	switch (elevatorPosition) {
		case elevatorDown:
			moveElevatorDown();
			break;
		case elevator30:
			moveElevatorDown();
			moveElevatorDist(elevator30);
			break;
		case elevator60:
			moveElevatorDown();
			moveElevatorDist(elevator60);
			break;
		case elevator120:
			moveElevatorDown();
			moveElevatorDist(elevator120);
			break;
		default:
			break;
	}

	elevatorMovingAsync = false;
}

task initialize() {
	//Initialize components
	print("Initializing...");

	PlaySound(soundBeepBeep);

	setMotor(stopMotors());

	//Initialize the tube grabber servos up
	setGrabberServo(GRABBERDOWN);

	servo[irServo] = IRDOWN;

	servo[armHatch] = ARMHATCHUP;

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

	parkAuto();
}

void parkAuto() {
	//Determine the orientation of the center goal
	int lowestDistanceReading = USreadDist(sonar);

	time1[T2] = 0;
	while(time1[T2] < 1500) {
		lowestDistanceReading = (USreadDist(sonar) < lowestDistanceReading) ? USreadDist(sonar) : lowestDistanceReading;
	}

	//Run the proper auto
	if (lowestDistanceReading < 115)
		ir0();
	else if (lowestDistanceReading < 150)
		ir90();
	else
		ir45();
}

void ir90() {

	setMotor(strafeR(50));
	wait1Msec(800);
	gyroTurnLeft(90, 60);
	setMotor(strafeR(100));
	wait1Msec(2800);
	setMotor(backward(100));
	wait1Msec(1500);
	setMotor(stopMotors());
/*
	gyroTurnLeft(83, 60);

	//Go to red line and reset heading
	headingX = 0;

	setMotor(stopMotors());

	while (!(HTCS2readColor(color) == (short)BLUE1 || HTCS2readColor(color) == (short)BLUE2 || HTCS2readColor(color) == (short)RED1 || HTCS2readColor(color) == (short)RED2)) {
		setMotor(forward(30));
	}

	setMotor(backward(30));

	wait1Msec(300);

	setMotor(stopMotors());

	//Orient to center tube
	while(USreadDist(sonar) > 60) {
		setMotor(gyroFixHeading(strafeR(100)));
	}

	wait1Msec(450);

	setMotor(stopMotors());

	approachCenter();
	depositBall(elevator120);
	knockDownKickstand();*/
}

void ir45() {
	//Move forward off wall a bit
	setMotor(backward(30));
	wait1Msec(1400);
	setMotor(stopMotors());

	//Rotate 45 degrees
	gyroTurnLeft(45, 30);

	//Reset the heading after turning
	headingX = 0;

	//Orient to center tube
	time1[T1] = 0;
	while(USreadDist(sonar) > 40) {
		if (time1[T1] > 7000) {
			while (true) {
				print("SAFETY ENDED");
			}
		}
		setMotor(gyroFixHeading(strafeR(50)));
	}

	setMotor(forward(30));

	wait1Msec(500);

	setMotor(stopMotors());

	approachCenter();
	depositBall(elevator120);
	knockDownKickstand();
}

//When beacon is 0Deg from robot
void ir0() {
	approachCenter();
	depositBall(elevator120);
	knockDownKickstand();
}

void approachCenter() {
	elevatorPosition = elevator120;

	StartTask(moveElevatorAsync);

	servo[irServo] = IRUP;

	wait1Msec(500);

	while (USreadDist(sonar) > DISTCENTER) {
		setMotor(gyroFixHeading(backward(30)));
	}

	setMotor(stopMotors());

	wait1Msec(500);

	while(HTIRS2readDCDir(ir) > 4) {
		setMotor(strafeL(40));
	}

	while(HTIRS2readDCDir(ir) < 5) {
		setMotor(strafeR(40));
	}

	wait1Msec(200);

	setMotor(forward(50));

	wait1Msec(100);

	setMotor(stopMotors());

	servo[irServo] = IRDOWN;
}


/* FUNCTIONS FOR OFF RAMP AUTO */
void depositBall(elevatorPositions position) {

	//Ensure elevator's at 120
	while (elevatorMovingAsync == true)
		wait1Msec(1);

	//Get a lil closer
	setMotor(backward(30));
	wait1Msec(500);
	setMotor(stopMotors());

	//Deposit ball
	servo[armHatch] = ARMHATCHDOWN;
	wait1Msec(2000);
	servo[armHatch] = ARMHATCHUP;

	//Go back
	setMotor(forward(30));
	wait1Msec(500);
	setMotor(stopMotors());

	elevatorMove(elevatorDown);
}

void knockDownKickstand() {
	setMotor(strafeL(50));

	wait10Msec(100);

	while (USreadDist(sonar) < 50) {
		setMotor(strafeL(50));
	}

	wait1Msec(700);

	setMotor(strafeR(100));

	wait1Msec(200);

	setMotor(backward(100));

	wait1Msec(2000);

	setMotor(stopMotors());
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

void elevatorMove(elevatorPositions position) {
	servo[armHatch] = ARMHATCHUP;

	switch (position) {
		case elevatorDown:
			moveElevatorDown();
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
		elevatorMotors(-100);
	}

	elevatorMotors(0);
	resetElevatorEncoders();
}

void moveElevatorDist(elevatorPositions position) {
	while(nMotorEncoder[elevatorR] < (float)position) {
		elevatorMotors(100);
		servo[armHatch] = ARMHATCHUP;
	}

	elevatorMotors(0);
}

mVals *gyroFixHeading(mVals *m) {
	//Update heading
	updateHeadingX();

	//Motor powers are adjusted by this value (based on heading and correction factor)
	float adjusted_val = headingX * CF;

	float fl, fr, bl, br;

	fl = m->fl;
	fr = m->fr;
	bl = m->bl;
	br = m->br;

	//Store corrected motor vals
	mVals newM;

  newM.fl = fl - adjusted_val;
	newM.fr = fr + adjusted_val;
	newM.bl = bl - adjusted_val;
	newM.br = br + adjusted_val;

	return newM;
}

void elevatorMotors(int power) {
	motor[elevatorL] = power;
	motor[elevatorR] = power;
}

void resetElevatorEncoders() {
	nMotorEncoder[elevatorL] = 0;
	nMotorEncoder[elevatorR] = 0;
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
	servo[rServo] = 280-val;
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

void gyroTurnLeft(int degrees, int power) {
	headingX = 0;
	while(abs(headingX) < degrees) {
		updateHeadingX();

		setMotor(turnLeft(power));
	}

	int timesToCorrect = 2;
	int timesCorrected = 0;

	while (timesCorrected < timesToCorrect) {
		if (headingX < 0) {
			while (headingX < (float)-1*degrees) {
				setMotor(turnRight(30));
				updateHeadingX();
			}

			setMotor(stopMotors());

			while (headingX > (float)-1*degrees) {
				setMotor(turnLeft(30));
				updateHeadingX();
			}

			setMotor(stopMotors());

		}
		else {
			while (headingX < (float)degrees) {
				setMotor(turnLeft(30));
				updateHeadingX();
			}

			setMotor(stopMotors());

			while (headingX > (float)degrees) {
				setMotor(turnRight(30));
				updateHeadingX();
			}

			setMotor(stopMotors());

		}

		timesCorrected += 1;

	}

	setMotor(stopMotors());
}

void gyroTurnRight(int degrees, int power) {
	headingX = 0;
	while(abs(headingX) < degrees) {
		updateHeadingX();

		setMotor(turnRight(power));
	}

	int timesToCorrect = 1;
	int timesCorrected = 0;

	while (timesCorrected < timesToCorrect) {
		if (headingX < 0) {
			while (headingX < (float)-1*degrees) {
				setMotor(turnLeft(30));
				updateHeadingX();
			}

			setMotor(stopMotors());

			while (headingX > (float)-1*degrees) {
				setMotor(turnRight(30));
				updateHeadingX();
			}

			setMotor(stopMotors());

		}
		else {
			while (headingX < (float)degrees) {
				setMotor(turnRight(30));
				updateHeadingX();
			}

			setMotor(stopMotors());

			while (headingX > (float)degrees) {
				setMotor(turnLeft(30));
				updateHeadingX();
			}

			setMotor(stopMotors());

		}

		timesCorrected += 1;

	}

	setMotor(stopMotors());
}
