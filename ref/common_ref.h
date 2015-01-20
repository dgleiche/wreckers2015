#ifndef COMMON_577_REF
#define COMMON_577_REF

//Constants

//Constants for grabber servo up/down
const int GRABBERDOWN = 10;
const int GRABBERUP = 90;

const int ARMHATCHDOWN = 100;
const int ARMHATCHUP = 240;

//Whether or not we're in an FTC comp
//Comment this out if we're not to save time
#define wait_for_start

//Easier vars for holding motor vals
//Translational Motor Vals
typedef struct {
	float fl, fr, bl, br;
} mVals;

typedef enum {
	elevatorDown,
	elevatorIR = 4000,
	elevator30 = 5700,
	elevator60,
	elevator90,
	elevator120
} elevatorPositions;

//Functions

//Quickly set vals of an mVal
mVals *setMVals(float fl, float fr, float bl, float br) {
	mVals m;
	m.fl = fl;
	m.fr = fr;
	m.bl = bl;
	m.br = br;
	return m;
}

/* Definite Movement Functions */
mVals *forward(int power = 50) {
	return setMVals(power, power, power, power);
}

mVals *backward(int power = 50) {
	return setMVals(-power, -power, -power, -power);
}
mVals *strafeL(int power = 50) {
	return setMVals(-power, power, power, -power);
}

mVals *strafeR(int power = 50) {
	return setMVals(power, -power, -power, power);
}

mVals *rotL(int power = 50) {
	return setMVals(-power, power, -power, power);
}

mVals *rotR(int power = 50) {
	return setMVals(power, -power, power, -power);
}

mVals *diagFR(int power = 50) {
	return setMVals(power, 0, 0, power);
}

mVals *diagFL(int power = 50) {
	return setMVals(0, power, power, 0);
}

mVals *diagBL(int power = 50) {
	return diagFR(-power);
}

mVals *diagBR(int power = 50) {
	return diagFL(-power);
}

//rotate clockwise
mVals *rCCW(int power = 50)
{
	return setMVals(power, -power, power, -power);
}

//rotate counterclockwise
mVals *rCW(int power = 50)
{
	return setMVals(-power, power, -power, power);
}

mVals *stopMotors() {
	return setMVals(0, 0, 0, 0);
}

/* Easy print overloads */
void print(int x)
{
	nxtDisplayCenteredTextLine(2, "%d", x);
}

void print(const string x)
{
	nxtDisplayCenteredTextLine(2, x);
}

void print(float x)
{
	nxtDisplayCenteredTextLine(2, "%f", x);
}

void print(float a, float b, float c, float d) {
	nxtDisplayCenteredTextLine(2, "%f", a);
	nxtDisplayCenteredTextLine(3, "%f", b);
	nxtDisplayCenteredTextLine(4, "%f", c);
	nxtDisplayCenteredTextLine(5, "%f", d);
}

#endif
