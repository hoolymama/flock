#ifndef _hexUtil_h
#define _hexUtil_h

class hexUtil {

public:

	enum Rank { kAnterior, kMedial, kPosterior };
	enum ActuatorAxis { kStepParam, kX, kY, kZ};
	enum ActuatorChannel { kTX, kTY, kTZ, kRX, kRY, kRZ };
	enum Space { kLocal, kWorld };
	enum Side { kLeft, kRight };

};
#endif