
#include "displayMask.h"

DisplayMask::DisplayMask() {}

DisplayMask::DisplayMask(bool plants, 
	bool home, 
bool footPos, 
bool id, 
bool speed, 
bool footLocal,
bool agentMatrix)
: displayPlants(plants), 
displayHome(home), 
displayFootPosition(footPos), 
displayId(id), 
displaySpeed(speed),
displayFootLocal(footLocal),
displayAgentMatrix(agentMatrix)
{}

