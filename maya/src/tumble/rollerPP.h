 
 #ifndef _rollerPP
 #define _rollerPP
 
 
 #include <maya/MVectorArray.h> 
 #include <maya/MDoubleArray.h> 
 #include <maya/MTime.h> 
 
 #include <maya/MTypeId.h> 
 
 
 
 class rollerPP: public MPxNode
 {
 	
 public:
 	rollerPP() ;
 	
 	virtual ~rollerPP();
 	
 	static void		*creator();
 	static MStatus	initialize();
 	
 	// will compute output force.
 	virtual MStatus	compute( const MPlug& plug, MDataBlock& data );
 
 	static MTypeId	id;
 
 
 	static MObject	aCurrentViewPP;	
 	static MObject	aLastViewPP;	
 	static MObject	aSpeedPP;
 	static MObject	aUpPP;	
	

 	static MObject	aSpeedInputMax;	

 	static MObject	aDoBanking;	 
 	static MObject	aBankingSpeedRamp;	
 	static MObject	aBankingStability;	 	
	static MObject	aBankingWorldUpStability;

	static MObject	aDoRolling;	 
	static MObject	aRollingSpeedRamp;	
	static MObject	aRollingStability;	
	static MObject	aRollingWorldUpStability;
	
	
	static MObject	aCurrentTime;
	
 	static MObject	aOutput;	


 private:
 
 
 };
 #endif
