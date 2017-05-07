#ifndef containmentPP_H
#define containmentPP_H

#include <maya/MPxNode.h>

class containmentPP: public MPxNode {
	public:
virtual	void		postConstructor();
		containmentPP (); virtual ~containmentPP ();
		virtual MStatus compute (const MPlug& plug, MDataBlock& data);
		static void *creator (); static MStatus initialize (); static MTypeId id;
	private:
		enum Side {kInside, kOutside};

		static MObject aPosition;
		static MObject aMesh;	
		static MObject aSide ;
		static MObject aOutput;	   


};
#endif

/*
nstring $conns[] = `listConnections -p 1 -c 1 "flockShape1.flatTargets[*].flatTranslateTarget"`;

for ($i=0;$i<`size $conns`;$i+=2) {
    connectAttr -f ("arrayMulti1.outputsV["+($i/2)+"]") ($conns[($i+1)]);
}

select `ls -type "multVectorDoublePP"`    ;
 ----


for ($i = 0;$i<24;$i++) {
    connectAttr -f ("flockShape1.flatTargets["+$i+"].flatUserDoubleTarget[0]") ("agent_"+$i+"_0:beetle.preferredLegAngleId");
}

select ("agent_*_0:High");


string $sel[] = `ls -sl`;
for ($o in $sel) {nww
    float $rand = rand(0.97,1.03);
    setAttr  ($o+".s") $rand $rand $rand;
}

\\

getAttr "aliveSampler.outRed";
getAttr "prioritizeShape1.outSignal";

getAttr "navPaused.outputV";


select "agent_*_0:Low";
select -add "agent_*_0:Shoes";
sting $sel[] = ``
 
-----

source "/Users/julian/dev/w/flock/maya/script/flockShape/flock.extractHeroes.mel";
select -r flockShape1 ;
flockConnectFlatTargets(0);


float $f1[] = `getAttr feederLeftOffset.outputV`;n
float $f2[] = `getAttr feederRightOffset.outputV`;

print `size $f1`;
print "\n";
print `size $f2`;
print "\n";


float $phi[] = `getAttr hexapodShape.outPhi`;



*/