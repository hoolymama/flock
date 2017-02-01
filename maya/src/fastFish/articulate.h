#ifndef _articulate
#define _articulate



#include <maya/MVectorArray.h> 
#include <maya/MDoubleArray.h> 

#include <maya/MTypeId.h> 
#include <maya/MObject.h> 



class articulate: public MPxNode
{
	
public:
	articulate() ;
	
	virtual ~articulate();
	
	static void		*creator();
	static MStatus	initialize();

	virtual MStatus	compute( const MPlug& plug, MDataBlock& data );

	static MTypeId	id;
	
// 	enum DrawStyle {  kDrawOff, kDrawBars, kDrawVectors };


 	enum ThrustFacingStrategy {  kOff ,kForward, kAllAround };	
private:

	
	// use these attributes of the objects motion to calculate how 
	// much rotation there has been about the up vector
	// from this we calculate a bend value
	
	// the bend value can be treated as a dynamic impulse
	// If conserve is not zero, then some of the bend value from the last frame
	// is conserved and the new bend is added, in effect creating an impulse.
	// In this case it is helful to set the spring value to help bring the bend 
	// back to 0
	//////////////////////////////////////////////////////////////////////////
	static MObject	aViewPP;
 	static MObject	aDeltaViewPP;
 	static MObject	aUpVectorPP;
	//////////////////////////////////////////////////////////////////////////	


	//////////////////////////////////////////////////////////////////////////	
	static MObject	aDoBend;
	static MObject	aBendPP;
	static MObject	aDeltaBendPP;
	static MObject	aBendImpulsePP;	
	static MObject	aBendLookup;
	static MObject	aBendMagnitude;
	static MObject	aBendSpring;
	static MObject	aConserveBend;
	static MObject	aOutBendPP;		
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	static MObject	aDoArch;
	static MObject	aArchPP;
	static MObject	aDeltaArchPP;
	static MObject	aArchImpulsePP;	
	static MObject	aArchLookup;
	static MObject	aArchMagnitude;
	static MObject	aArchSpring;
	static MObject	aConserveArch;
	static MObject	aOutArchPP;
	
	//	static MObject	aDoArch;
	//	static MObject	aArchMagnitude ;
	//	static MObject	aArchRangeMax ;
	//	static MObject	aArchLookup;
	// 	static MObject	aOutArchPP;	
	//////////////////////////////////////////////////////////////////////////

	// we receive a thrust value from sensors or fields.
	// this represents propulsion in some direction
	// we can conserve some effort from the previous frame
	// so as to smooth it over time
	//////////////////////////////////////////////////////////////////////////
	static MObject	aDoThrust;
	static MObject	aThrustPP;
	static MObject	aThrustMax;
	static MObject	aThrustFacingLookup;	
	static MObject	aThrustFacingStrategy;
	static MObject	aVelocityPP;
	static MObject	aVelocityMax;
	static MObject	aDragLookup;	
	static MObject	aDoDrag;
	
	static MObject	aEffortSpan;
	
	
	static MObject	aEffortAttack;	
	static MObject	aEffortDecay;	
	static MObject	aLastEffortPP;	
	static MObject	aOutEffortPP;	
	

//	static MObject	aFlapOffsetPP;	
	static MObject	aDoFlap;
	static MObject	aFlapOffsetMult;
	static MObject	aFlapOffsetLookup;
	static MObject	aOutFlapOffsetPP;
	
	static MObject	aFlapAmplitudeMult;	
 	static MObject	aFlapAmplitudeLookup;
	static MObject	aOutFlapAmplitudePP;
	
	static MObject	aCurrentTime;
	
	MStatus calculateArch(
		const MVectorArray & viewPP, 
		const MVectorArray & deltaViewPP, 
		const MVectorArray & upVectorPP, 
		const MDoubleArray  & archPP , 
		const MDoubleArray  & deltaArchPP , 
		const MDoubleArray  & archImpulsePP,
		MDoubleArray  & outArchPP , 
		double archMagnitude, 
		double conserveArch,
		double archSpring
		) const;
		
		

	   
	MStatus calculateBend(
		const MVectorArray & viewPP, 
		const MVectorArray & deltaViewPP, 
		const MVectorArray & upVectorPP, 
		const MDoubleArray  & bendPP , 
		const MDoubleArray  & deltaBendPP , 
		const MDoubleArray  & bendImpulsePP,
		MDoubleArray  & outBendPP , 
		double bendMagnitude, 
		double conserveBend, 
		double bendSpring
		) const;
		
	
	MStatus calculateEffort(
		const MVectorArray &thrustPP, 
		const MVectorArray &viewPP,
		const MVectorArray &velocityPP,
		double thrustMax, 
		double velocityMax, 
		double effortSpan, 
		double effortAttack, 
		double effortDecay, 
		MDoubleArray &lastEffortPP, 
		MDoubleArray &outEffortPP, 
		bool doThrust, 
		bool doDrag,
		ThrustFacingStrategy thrustStrategy = articulate::kOff
	) const;
		
  // MStatus doRampLookup(
  //  	const MObject& node, 
  // 	const MObject& attribute, 
  // 	const MDoubleArray& in, 
  // 	MDoubleArray& results, 
  // 	float oldMin,		
  // 	float oldMax,
  // 	float newMin,	
  // 	float newMax
  // ) const;
		
};
#endif
