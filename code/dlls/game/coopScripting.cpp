//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl, 
// Based upon code from the HaZardModding Coop Mod Level Scripts created at 2006
// E-Mail:		chrissstrahl@yahoo.de
//
// CONTAINING SCRIPTING OR LEVEL SCRIPT RELATED FUNCTIONS FOR THE HZM CO-OP MOD

//HAZARDMODDING CO-OP SCRIPT MODIFICATION ©2006-2018 SOME RIGHTS RESERVED AND
//PRIMARY (IP)INTELLECTUAL PROPERTY ON THE HZM COOP MOD HELD BY CHRISTIAN SEBASTIAN STRAHL, ALIAS CHRISSSTRAHL.

//YOU ARE EXPLICITE FORBIDDEN TO PUBLISH A MODIFIED VARIANT OF THIS CODE,
//ANY MATERIALS OR INTELLECTUAL PROPERTY OF THIS FILE WITHOUT THE EXPLICIT
//WRITTEN PERMISSION OF THE RESPECTIVE OWNERS!

//YOU MAY USE CODE PARTS AS LONG AS THEY DO NOT COMPROMISE THE GAME SAFTY
//LOCAL AND INTERNATIONAL LAWS, AS WELL AS VIOLATE UPON THE ENDCLIENT ITS PRIVACY

//CONTACT: chrissstrahl@yahoo.de [Christian Sebastian Strahl, Germany]

#include "_pch_cpp.h"

#include "coopScripting.hpp"
#include "coopObjectives.hpp"


//================================================================
// Name:        coop_scriptingFunctionStarted
// Class:       -
//              
// Description: gets called when a function is called eigther as thread, or in line
//              
// Parameters:  str sFunctioname
//              
// Returns:     VOID
//              
//================================================================
//void coop_scriptingFunctionStarted( str sFunctioname )
//{
//	gi.Printf( "coop func call - %s\n" , sFunctioname.c_str() );
	//if ( !Q_stricmp( sFunctioname , "" ) ){
	//	gi.Printf( va("FUNCTION: %s\n" , sFunctioname.c_str() ) );
	//}
//}

void coop_setVectorScriptVariableValue( const char* varname , Vector vSet )
{
	program.setVectorVariableValue( varname , vSet );
}
void coop_setFloatScriptVariableValue( const char* varname , float fSet )
{
	program.setFloatVariableValue( varname , fSet );
}
void coop_setStringScriptVariableValue( const char* varname , const char* sSet )
{
	program.setStringVariableValue( varname , sSet );
}



