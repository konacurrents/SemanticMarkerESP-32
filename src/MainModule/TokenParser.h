
#ifndef TokenParser_h
#define TokenParser_h

#include "../../Defines.h"

/*
 <identifier>   ::= {characters A-Z,a-z,0-9,_}
 <notModifier>  ::= '!'
 <modifier>     ::=  <notModifier>
 <wildcardAny>  ::= '*'
 <wildcard>     ::= <wildcardAny>
 <booleanAnd>   ::= '&'
 <booleanOr>    ::= '|'
 <booleanOps>   ::= <booleanAnd> | <booleanOr> | <null>
 <queryElement> ::= { [<modifier>] [<wildcard>] <identifier> [<wildcard>] }
 <queryPart>    ::=  { <queryElement> <booleanOps> }
 
 <identifierFollow> ::= {anything not <identifier> }
 eg.   ! M5*   ->  <notModifier> <identifier> <wildcard>
 !*M5* & ! *GEN3*   -> <wildcard=*> <identifier=M5> <wildcard=*> <booleanOps=&> <wildcard=*> <identifier=GEN3>
 means: any non M5* and not *GEN3*
 
 
 Currently presidence will be AND over OR (or just left to right)
 
 Usage:
     parseQueryLine_mainModule((char*)"M5* | DukeGEN3");
      boolean queryMatchesName_mainModule("M5Duke");
 */


//!parses a line of text, The caller then uses queryMatchesName() to see if their name matches
void parseQueryLine_mainModule(char *line);

//!now need to process the token tree to see if name1 and name2 match the query
//!eg.  ! *name* & name2 ...
//!wildcards after name make it slightly complicated
//!    SO:  look for wildcards before and after, and the id. then decide if that matches
//!    before the boolean and notmodifier applied
boolean queryMatchesName_mainModule(char*name);

//! whether the stirng is a potential query. This can be used, but it is also used by the parseQueryLine
//! to optimize out the original dev:name query. 
boolean stringIsQuery_mainModule(char *line);

//! setup for the token parser (really just for testing)
void setup_tokenParser_mainModule();

//! 3.23.25 rainy weekend
//! create a JSON string from the SemanticMarker
//! https://semanticmarker.org/bot/setdevice/scott@konacurrents.com/PASS/M5AtomSocket/socket/on}
//! Create a JSON knowing the "bot" syntax, eg.  setdevice/USER/PASS/device/<set>/<flag>
//! and others like set etc.
//! Maybe not 'dev' here.   Others  cmddevice/USER/PASS
//! return:  {'set':<set>,"dev':
//! Supports these for now:
//!GET /set/{username}/{password}/{command}/{value}
//!GET /send/{username}/{password}/{request}
//! only support 'device' ones
//!GET /setdevice/{username}/{password}/{devicename}/{command}/{value}
//!GET /senddevice/{username}/{password}/{device}/{request}
//!return "" if not valid
char *semanticMarkerToJSON_TokenParser(char* semanticMarker);

//! 3.29.25 Raiiiinier Beeer movie last night
//! add returning a UUID.FLOWNUM if valid, or nil if nota
char *parseSM_For_UUID_Flownum(char* semanticMarker);

#endif// TokenParser_h

