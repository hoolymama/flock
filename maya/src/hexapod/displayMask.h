#ifndef DisplayMask_H
#define DisplayMask_H

/*
DisplayMask is a convenience object to package up some display flag booleans.
*/
class DisplayMask {
public:
	bool displayPlants;
	bool displayHome;
	bool displayFootPosition;
	bool displayId;
	bool displaySpeed;
	bool displayFootLocal;
	bool displayAgentMatrix;


	DisplayMask();

	DisplayMask(
		bool plants, 
		bool home, 
		bool footPos, 
		bool id, 
		bool speed, 
		bool footLocal,
		bool agentMatrix);

} ;


#endif


