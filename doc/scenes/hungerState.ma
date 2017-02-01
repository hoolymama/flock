//Maya ASCII 2017 scene
//Name: hungerState.ma
//Last modified: Tue, Aug 02, 2016 07:26:06 pm
//Codeset: UTF-8
requires maya "2017";
requires "stereoCamera" "10.0";
requires -nodeType "hungerState" -dataType "poolData" -dataType "splinePoolData"
		 "Flock" "0.1.4";
requires "jPressureValve" "5.0";
currentUnit -l centimeter -a degree -t film;
fileInfo "application" "maya";
fileInfo "product" "Maya 2017";
fileInfo "version" "2017";
fileInfo "cutIdentifier" "201606150345-997974";
fileInfo "osv" "Mac OS X 10.11.6";
createNode hungerState -n "hungerState1";
	rename -uid "CA606766-8A40-F1F5-B9D6-1798B14CC2DB";
	addAttr -ci true -sn "docMeta_Author" -ln "docMeta_Author" -dt "string";
	addAttr -ci true -sn "docMeta_Email" -ln "docMeta_Email" -dt "string";
	addAttr -ci true -sn "docMeta_Keywords" -ln "docMeta_Keywords" -dt "string";
	addAttr -ci true -sn "docMeta_TestScene" -ln "docMeta_TestScene" -dt "string";
	addAttr -ci true -sn "docMeta_Description" -ln "docMeta_Description" -dt "string";
	addAttr -ci true -sn "docBlock_Synopsis" -ln "docBlock_Synopsis" -dt "string";
	addAttr -ci true -sn "docAttr_Feed" -ln "docAttr_Feed" -dt "string";
	addAttr -ci true -sn "docAttr_Spend" -ln "docAttr_Spend" -dt "string";
	addAttr -ci true -sn "docAttr_FoodLevel" -ln "docAttr_FoodLevel" -dt "string";
	addAttr -ci true -sn "docAttr_UpdatedFoodLevel" -ln "docAttr_UpdatedFoodLevel" -dt "string";
	addAttr -ci true -sn "docAttr_Hungry" -ln "docAttr_Hungry" -dt "string";
	addAttr -ci true -sn "docAttr_UpdatedHungry" -ln "docAttr_UpdatedHungry" -dt "string";
	addAttr -ci true -sn "docAttr_FullThreshold" -ln "docAttr_FullThreshold" -dt "string";
	addAttr -ci true -sn "docAttr_HungryFullCycle" -ln "docAttr_HungryFullCycle" -dt "string";
	addAttr -ci true -sn "docAttr_HungryFullRamp" -ln "docAttr_HungryFullRamp" -dt "string";
	setAttr ".hfr[0]"  0.13913 0.16 1;
	setAttr ".docMeta_Author" -type "string" "Julian Mann";
	setAttr ".docMeta_Email" -type "string" "julian.mann@gmail.com";
	setAttr ".docMeta_Keywords" -type "string" "";
	setAttr ".docMeta_TestScene" -type "string" "";
	setAttr ".docMeta_Description" -type "string" "A simple state machine to represent hunger for artificial life agents";
	setAttr ".docBlock_Synopsis" -type "string" (
		"Manage the state of being hungry or not hungry, based on the accumulation and depletion of food.\nthe attributes hungry and updatedHungry represent the hunger state. They are either 0 or 1. (satisfied or hungry)\n\n<%= movie_tag('An Alife rat colony', 'Notice how they stop and wait for a few frames. Their motion is controlled by a state machine','movies/ratSchool.mov','right') %>\n\n<hr>\n\nThe algorithm behaves differently depending on its state.\nWhen in the *hungry* state, the value of the feed attribute is added to the value at foodLevel, which is sent to the output as updatedFoodLevel. If that value exceeds the value of fullThreshold, then the state is flipped to 0 (satisfied). If it doesn't reach fullThreshold, it stays hungry and continues to eat food.\n\nWhen in the *satisfied* state, feed is ignored, and the value of the spend attribute is subtracted from the value at foodLevel, which again is sent to the output as updatedFoodLevel. If that value drops below zero, then the state is flipped to 1 (hungry). If it doesn't reach zero, it stays satisfied and continues to spend food.\n"
		+ "\nBy setting the the spend and feed proportions well, you can achieve seemingly very complex behaviours, such as rats that stop to sniff and scratch their noses, or flies that hammer away sporadically at a window. The perception is often that the creatures are stopping to think before launching into some other behaviour. \n\n\nThe reason for an \"updated\" version of two of the attibutes is that those signals need to be maintained by the particle system that represents the agents. The hungerState node can't maintain the state internally because the number of particles may change and it would have no way of tracking those changes. So the particle system itself maintains the state, the hungerState node simply does the calculations. Therfore it is neccessary to get information from the particle system, and then send back updated information.\n\nIt can be helpful to connect the updatedFoodLevel to a \"graph node\":../gl_graph/graphShape.html to see the rate that agents are filling up and emptying.\n\n\n");
	setAttr ".docAttr_Feed" -type "string" "The amount of food to be added to the food level when hungry";
	setAttr ".docAttr_Spend" -type "string" "The amount of food to be subtracted from the food level when satisfied";
	setAttr ".docAttr_FoodLevel" -type "string" "The amount of food held by the agent";
	setAttr ".docAttr_UpdatedFoodLevel" -type "string" "The updated food level sent back to the agent at the end of calculations.";
	setAttr ".docAttr_Hungry" -type "string" "The hungry state of the agent";
	setAttr ".docAttr_UpdatedHungry" -type "string" "The updated hungry state sent back to the agent at the end of calculations.";
	setAttr ".docAttr_FullThreshold" -type "string" "The amount of food held by the agent that signifies enough to be satisfied with. When this value is reached, the agent switches state.";
	setAttr ".docAttr_HungryFullCycle" -type "string" "hungryFullCycle is a value representing the stage from hungry to full. The hungryFullRamp can be used to remap it. \nIf the hungryFullRamp is not used, then the value is a linear 0 - 1.0. Within that range, 0 to 0.5 is when the state is hungry and feeding. 0.5 to 1 is where the state is satisfied and resting. \nIf the ramp is used, then the above value will be considered the input to the ramp.";
	setAttr ".docAttr_HungryFullRamp" -type "string" "hungryFullRamp allows control of the output of hungryFullCycle. The input is a linear 0 - 1.0. Within that range, 0 to 0.5 is when the state is hungry and feeding. 0.5 to 1 is where the state is satisfied and resting.\nThe ramp only takes effect when it has some stops that are not all 1.0";
select -ne :time1;
	setAttr ".o" 1;
	setAttr ".unw" 1;
select -ne :hardwareRenderingGlobals;
	setAttr ".vac" 2;
select -ne :renderPartition;
	setAttr -s 2 ".st";
select -ne :renderGlobalsList1;
select -ne :defaultShaderList1;
	setAttr -s 4 ".s";
select -ne :postProcessList1;
	setAttr -s 2 ".p";
select -ne :defaultRenderingList1;
select -ne :initialShadingGroup;
	setAttr ".ro" yes;
select -ne :initialParticleSE;
	setAttr ".ro" yes;
select -ne :defaultRenderGlobals;
	setAttr ".ep" 1;
select -ne :defaultResolution;
	setAttr ".w" 640;
	setAttr ".h" 480;
	setAttr ".dar" 1.3333332538604736;
select -ne :defaultColorMgtGlobals;
	setAttr ".cme" no;
select -ne :hardwareRenderGlobals;
	setAttr ".ctrs" 256;
	setAttr ".btrs" 512;
// End of hungerState.ma
