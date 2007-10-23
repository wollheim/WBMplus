/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDSMoistChg.c

balazs.fekete@unh.edu

*******************************************************************************/

#include <stdio.h>
#include <math.h>
#include <cm.h>
#include <MF.h>
#include <MD.h>

// Input
static int _MDInRainSoilMoistID     = MFUnset;
static int _MDInRainSoilMoistChgID  = MFUnset;
static int _MDInIrrSoilMoistID      = MFUnset;
static int _MDInIrrSoilMoistChgID   = MFUnset;
static int _MDInSoilAvailWaterCapID = MFUnset;
static int _MDInIrrAreaFracID       = MFUnset;
// Output
static int _MDOutSoilMoistID        = MFUnset;
static int _MDOutSMoistChgID        = MFUnset;
static int _MDOutRelSoilMoistID     = MFUnset;

static void _MDSoilMoistChg (int itemID) {	
// Input
	float rainSMoist;        // Non-irrigated soil moisture [mm/dt]
	float rainSMoistChg;     // Non-irrigated soil moisture change [mm/dt]
	float irrSMoist;         // Irrigated soil moisture [mm/dt]
	float irrSMoistChg;      // irrigated soil moisture change [mm/dt]
	float soilAvailWaterCap; // Available water capacity [mm]
	float irrAreaFraction;   // Irrigated area fraction 
// Output
	float sMoist;            // Soil moisture [mm/dt]
	float sMoistChg;         // Soil moisture change [mm/dt]
	
	rainSMoist        = MFVarGetFloat (_MDInRainSoilMoistID,     itemID, 0.0);
	rainSMoistChg     = MFVarGetFloat (_MDInRainSoilMoistChgID,  itemID, 0.0);
	soilAvailWaterCap = MFVarGetFloat (_MDInSoilAvailWaterCapID, itemID, 0.0);
	if (_MDInIrrAreaFracID != MFUnset) {
		irrAreaFraction = MFVarGetFloat (_MDInIrrAreaFracID,     itemID, 0.0);
		irrSMoist       = MFVarGetFloat (_MDInIrrSoilMoistID,    itemID, 0.0);
		irrSMoistChg    = MFVarGetFloat (_MDInIrrSoilMoistChgID, itemID, 0.0);
		sMoist    = rainSMoist    * (1.0 - irrAreaFraction) + irrSMoist    * irrAreaFraction;
		sMoistChg = rainSMoistChg * (1.0 - irrAreaFraction) + irrSMoistChg * irrAreaFraction;
	}
	else {
		sMoist    = rainSMoist;
		sMoistChg = rainSMoistChg;
	}
	MFVarSetFloat (_MDOutSoilMoistID,    itemID, sMoist);
	MFVarSetFloat (_MDOutSMoistChgID,    itemID, sMoistChg);
	MFVarSetFloat (_MDOutRelSoilMoistID, itemID, sMoist / soilAvailWaterCap);
}

int MDSoilMoistChgDef () {
	int ret;
	if (_MDOutSMoistChgID != MFUnset) return (_MDOutSMoistChgID);

	MFDefEntering ("Soil Moisture");

	if (((ret = MDIrrGrossDemandDef ()) != MFUnset) &&
	    ((ret == CMfailed) ||
	     ((_MDInIrrSoilMoistID     = MFVarGetID (MDVarIrrSoilMoisture,     "mm",   MFInput, MFState, MFBoundary)) == CMfailed) ||
	     ((_MDInIrrSoilMoistChgID  = MFVarGetID (MDVarIrrSoilMoistChange,  "mm",   MFInput, MFFlux,  MFBoundary)) == CMfailed) ||
	     ((_MDInIrrAreaFracID      = MFVarGetID (MDVarIrrAreaFraction,     "%",    MFInput, MFState, MFBoundary)) == CMfailed)))
	     return (CMfailed);
	if (((_MDInSoilAvailWaterCapID = MDSoilAvailWaterCapDef ()) == CMfailed) ||
	    ((_MDInRainSoilMoistID     = MFVarGetID (MDVarRainSoilMoisture,    "mm",   MFInput, MFState, MFInitial))  == CMfailed) ||
        ((_MDInRainSoilMoistChgID  = MFVarGetID (MDVarRainSoilMoistChange, "mm",   MFInput, MFState, MFBoundary)) == CMfailed) ||
	    (MFModelAddFunction (_MDSoilMoistChg) == CMfailed)) return (CMfailed);
	MFDefLeaving ("Soil Moisture");
	return (_MDOutSMoistChgID);
}
