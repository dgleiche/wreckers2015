#ifndef COMMON_577_REF
#define COMMON_577_REF

//Constants
const int GYRODOWN = 300;
const int GYROUP = 130;

const int GRABBERDOWN = 0;
const int GRABBERUP = 110;

//Whether or not we're in an FTC comp
//Comment this out if we're not to save time
#define wait_for_start

//Easier vars for holding motor vals
//Translational Motor Vals
typedef struct {
	float fl, fr, bl, br;
} mVals;

//Rotational Motor Vals
typedef struct {
	float fl, fr, bl, br;
} rVals;

//Function prototypes
//Quickly create an mvals var
mVals *setMVals(float fl, float fr, float bl, float br);
rVals *setRVals(float fl, float fr, float bl, float br);

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

rVals *setRVals(float fl, float fr, float bl, float br) {
	rVals r;
	r.fl = fl;
	r.fr = fr;
	r.bl = bl;
	r.br = br;
	return r;
}


//Definite Movement Functions
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
mVals *rCW(int power = 50)
{
	return setMVals(power, -power, power, -power);
}

//rotate counterclockwise
mVals *rCCW(int power = 50)
{
	return setMVals(-power, power, -power, power);
}

//Easy print overloads
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

#endif
