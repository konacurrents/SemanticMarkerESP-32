#include "TokenParser.h"

#ifdef WILDCARD_DEVICE_NAME_SUPPORT

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
*/

//!retrieves the parsed values

//!the tokens in the token array
typedef enum
{
    notModifier_enum,
    wildcardAny_enum,
    booleanOr_enum,
    booleanAnd_enum,
    identifier_enum,
    null_enum
} TokensEnum;

//!max length of an identifier..
const int _identifierMaxLen= 30;
//! storage for the current identifier
char _identifier[_identifierMaxLen];
// !where in the current identifier
int _identifierIndex;

typedef struct identifierStruct
{
    char identifier[_identifierMaxLen];
    boolean wildcardBefore;
    boolean wildcardAfter;
    boolean notModifier;
} IdentifierStruct;

//!so now a query is { IdentifierStruct <booleanAnd/Or> }

//!the max number of identifiers
const int _identifierStructsMax = 3;
//!index into _identifiers array
int _identifierStructsIndex = 0;
//! array of identifiers ..
IdentifierStruct _identifierStructs[_identifierStructsMax];

const int _tokensMax = 10;
TokensEnum _tokens[_tokensMax];
//!index into _tokens array
int _tokensStackPointer;

//!whether in an identifier parsing
boolean _parsingIdentifier;

//!pushes token onto token stack
void pushToken(TokensEnum token)
{
    SerialLots.printf("\nPushToken[%d] %d\n", _tokensStackPointer, token);

    if (_tokensStackPointer >= _tokensMax)
    {
        SerialMin.println(" *** Over the token max ***");
        return;
    }
    _tokens[_tokensStackPointer++] = token;
}

//!get the current top of stack token
TokensEnum topOfTokenStack()
{
    //if stack empty .. return null_enum
    if (_tokensStackPointer == 0)
        return null_enum;
    //! otherwise grab the top
    return _tokens[_tokensStackPointer-1];
}

//!get the current top of stack token, and move the token stack pointer
TokensEnum popTokenStack()
{
    TokensEnum result;
    //if stack empty .. return null_enum
    if (_tokensStackPointer == 0)
        result = null_enum;
    else
    {
        //!else pop
        _tokensStackPointer--;
        result = _tokens[_tokensStackPointer];
    }
    SerialLots.printf("\npop[%d] = %d\n", _tokensStackPointer, result);
    return result;
}

//!pushes identifier onto identifier stack, while also inserting a token for identifier
void pushIdentifier(char *identifier)
{
    SerialLots.printf("\nPushId %s\n", identifier);

    pushToken(identifier_enum);
    //! now save Identifier
    strcpy(_identifierStructs[_identifierStructsIndex++].identifier, identifier);
}

//! how many tokens
int numTokens()
{
    return _tokensStackPointer;
}
//!how many identifiers
int numIdentifiers()
{
    return _identifierStructsIndex;
}
//!get the nth token
TokensEnum getToken(int num)
{
    return _tokens[num];
}
//!the nth identifier
char *getIdentifier(int num)
{
    return _identifierStructs[num].identifier;
}

//!grab full identifier struct (Pointer?)
IdentifierStruct getIdentifierStruct(int num)
{
    return _identifierStructs[num];
}

//!adds a valie character to an identifier
void addIdentifierChar(char c)
{
    _identifier[_identifierIndex] = c;
    _identifierIndex++;
    _identifier[_identifierIndex] = (char)NULL;
}
//!initi for each new identifier
void initIdentifier(char c)
{
    _identifierIndex = 0;
    addIdentifierChar(c);
}

//!finished parsing an identifier
void doneParsingIdentifier()
{
    _parsingIdentifier = false;
    pushIdentifier(_identifier);
}

//! whether the character is a valid identifier character
//! 1.22.24 adding ":" so fits a BLE address
//! DevQuery: !M5Laybrinth & !84:cc:a8:7a:f6:aa
boolean validIdentifierChar(char c)
{
    boolean valid = false;
    if (c == '_')
        valid = true;
    else if (c >= 'A' && c <= 'Z')
        valid = true;
    else if (c >= 'a' && c <= 'z')
        valid = true;
    else if (c >= '0' && c <= '9')
        valid = true;
    else if (c == ':')
        valid = true;
    else valid = false;
    return valid;
}

//!push identifier .. looks at the stack and fills in the identifier object
//!called after seeing a & or | (the follow set of the identifier, after a *)*
void processIdentifier()
{
    //!the index is already 1 past..
    int identifierStructsIndex = _identifierStructsIndex-1;
    SerialLots.printf("\nprocessIdentifier[%d]\n", identifierStructsIndex);
   //! look at stack
    // the _identifierStructsIndex is the identifier we will modify..
    TokensEnum top = topOfTokenStack();
    if (top == wildcardAny_enum)
    {
        SerialLots.printf("wildcardAny_enum\n");

        _identifierStructs[identifierStructsIndex].wildcardAfter = true;
        top = popTokenStack();
    }
    top = topOfTokenStack();
    if (top != identifier_enum)
    {
        SerialMin.printf("*** Syntax error: expected identifier, found: %d\n", top);
    }
    else
    {
        SerialLots.printf("identifier_enum\n");

        // should be identifier we are looking for..
        top = popTokenStack();
    }
    top = topOfTokenStack();
    if (top == wildcardAny_enum)
    {
        SerialLots.printf("wildcardAny_enum\n");

        _identifierStructs[identifierStructsIndex].wildcardBefore = true;
        top = popTokenStack();
    }
    top = topOfTokenStack();
    if (top == notModifier_enum)
    {
        SerialLots.printf("notModifier_enum\n");

        _identifierStructs[identifierStructsIndex].notModifier = true;
        top = popTokenStack();
        SerialLots.printf("token = %d\n", top);
    }
      
    //!replace the stack with our filled out identifier
    pushToken(identifier_enum);
    
}

//!initialize the parser
void initParser()
{
    _identifierStructsIndex = 0;
    _identifierIndex = 0;
    _tokensStackPointer = 0;
    _parsingIdentifier = false;
    for (int i=0; i< _identifierStructsMax; i++)
    {
        _identifierStructs[i].wildcardBefore = false;
        _identifierStructs[i].wildcardAfter = false;
        _identifierStructs[i].notModifier = false;
    }
}


//!parses a line of text, returning the number of tokens..
void parseQueryLine_mainModule(char *line)
{
//! NOTE: doesn't support the BLE addresses like:
//! DevQuery: !M5Laybrinth & !84:cc:a8:7a:f6:aa
    SerialTemp.printf("\nDevQuery: %s\n", line);
    
    //! initialize the variables for parsing this line
    initParser();
    //! if no query params, then make it an identifier and exit
    if (!stringIsQuery_mainModule(line))
    {
        SerialLots.println("*** Not a query, so optimze to just identifier");
        pushIdentifier(line);
        return;
    }
    while (*line)
    {
        char c = *line++;
        SerialLots.print(c);
        if (validIdentifierChar(c))
        {
            //! if parsing still then add to identifier
            if (_parsingIdentifier)
            {
                addIdentifierChar(c);
            }
            else
            {
                //! start a new identifier..
                _parsingIdentifier = true;
                //! init the identifier
                initIdentifier(c);
            }
        }
        //! else not valid identifier (so a follow..)
        else
        {
            boolean finishIdNeeded = false;
            //! done parsing if not a valid identifier character
            if (_parsingIdentifier)
            {
                doneParsingIdentifier();
                finishIdNeeded = true;
                //! finishIdNeeded is before pushing another token, unless a not or wildcard
            }
            //! now classify this char
            //! if an identifier modifier
            if (c == '!' || c == '*')
            {
                switch (c)
                {
                        //! ! and * belong to the identifier object
                        //! if a modifier of ident (~ or *) then push the token,
                        //! and process the identifier
                    case '!':
                        pushToken(notModifier_enum);
                        break;
                    case '*':
                        pushToken(wildcardAny_enum);
                        break;
                }
                if (finishIdNeeded)
                    processIdentifier();
            }
            else
            {
                //! an operator or space
                //! if inside an identifier, finish it first
                if (finishIdNeeded)
                    processIdentifier();
                //! process the token
                switch (c)
                {
                        //!& and | are expression modifiers
                    case '&':
                        pushToken(booleanAnd_enum);
                        break;
                    case '|':
                        pushToken(booleanOr_enum);
                        break;
                    case ' ':
                        break;
                    default:
                        //** Invalid character **.
                        SerialTemp.println(" *** INVALID CHAR ***");
                        break;
                }
            } // switch
            
        }
    } //while
    
    //! if end of line, and was parsing an identifier .. then finish
    if (_parsingIdentifier)
    {
        doneParsingIdentifier();
        processIdentifier();
    }
    
}


//! These are CASE SENSITIVE matches ...
//!matches  name with wildcards and the idToMatch
//!name is what is being queried, the idQueryStruct is the QUERY
boolean nameMatchesWildcardQuery(char *name, IdentifierStruct idQueryStruct)
{
    char *queryIdToMatch = idQueryStruct.identifier;
    boolean wildcardBefore = idQueryStruct.wildcardBefore;
    boolean wildcardAfter = idQueryStruct.wildcardAfter;
    boolean wildcardNot = idQueryStruct.notModifier;
    //default
    boolean match = false;

    SerialLots.printf("%s - idQuery(%s), before=%d,after=%d,not=%d\n", name, queryIdToMatch, wildcardBefore, wildcardAfter, wildcardNot);
    //!@see https://legacy.cplusplus.com/reference/cstring/strncmp/
    //! eg.  idToMatch == M5 and wildcard after and before, then just search for idToMatch in name
    //! If not before, then starts with ...
    //! if non, then strmatch
    if (wildcardBefore && wildcardAfter)
        // contains .. ptr to first occurance of idToMatch in name
        match = (strstr(name, queryIdToMatch)!=NULL);
    else if (wildcardAfter)
        // starts with ..
        match = (strncmp(name, queryIdToMatch, strlen(queryIdToMatch))==0);
    else if (wildcardBefore)
    {
        int queryIdLen = strlen(queryIdToMatch);
        int nameLen = strlen(name);
        // ends with..
        //eg:  *Rainier   (and name == M5Rainier)
        // and *Rainier matched Rainier
        //  *Room1  and Scooby_Room1  should match
        // diff =   *R  and SR  ->  queryLen = 1, our len = 2,  2 -1 = 1 CORRECT
        // just look for the last idLen characters of name
        if (nameLen >= queryIdLen)
        {
            //start search at N into name
            char *startInName = name + nameLen - queryIdLen;
            match = strcmp(startInName, queryIdToMatch) == 0;
        }
    }
    else
        // is identical
        match = strcmp(name, queryIdToMatch)==0;
    
    //!see if there was a NOT (!)
    if (wildcardNot)
        match = !match;
    return match;
}


//! whether the stirng is a potential query
boolean stringIsQuery_mainModule(char *line)
{
    //strpbrk = first occurance of any characters in str2 in str1
    boolean isQuery = strpbrk(line, (char*)"|*&!") != NULL;
    return isQuery;
}

//!now need to process the token tree to see if name1 and name2 match the query
//!eg.  ! *name* & name2 ...
//!wildcards after name make it slightly complicated
//!    SO:  look for wildcards before and after, and the id. then decide if that matches
//!    before the boolean and notmodifier applied
boolean queryMatchesName_mainModule(char*name)
{
    //!basically for each identifier in the query, the names (1&2) have to be compared each time..
    boolean match = true;
    int tokensIndex = 0;
    int idIndex = 0;

    //!what the last boolean_enum was..
    TokensEnum boolean_enum = null_enum;
    IdentifierStruct lastIdentifier;
    // each identifier has a match..
    boolean thisMatch = false;
    
    while (tokensIndex < numTokens())
    {
        // A or B and D
        TokensEnum token = getToken(tokensIndex++);
        switch (token)
        {
            case booleanOr_enum:
                boolean_enum = booleanOr_enum;
                break;
            case booleanAnd_enum:
                boolean_enum = booleanAnd_enum;
                break;
            case identifier_enum:
                lastIdentifier = getIdentifierStruct(idIndex++);
                thisMatch = nameMatchesWildcardQuery(name, lastIdentifier);
                if (boolean_enum == null_enum)
                {
                    // first time, reset of times the enum will be set..
                    // good if only Identifier too..  if false .. true & false = false
                    match = match && thisMatch;
                }
                else
                {
                    //! the result of A <> B
                    // at the B,  eg  A or B  | A and B
                    if (boolean_enum == booleanOr_enum)
                        match = match || thisMatch;
                    else
                        match = match && thisMatch;
                    // reset, should never read again
                    boolean_enum = null_enum;
                }
                SerialLots.printf("thisMatch = %d, match = %d\n", thisMatch, match);

                break;
                //  ID  or  ID and ID
                // null ID or IDx
        };
    }
    return match;
}

//#define TEST_PARSER
#ifdef TEST_PARSER

//!walks the tree printing it out..
void printTokenTree()
{
    //SerialLots.printf("tokens=%d, id=%d\n", numTokens(), numIdentifiers());
    SerialTemp.println("TOKEN TREE..");
    
    int tokensIndex = 0;
    int idIndex = 0;
    while (tokensIndex < numTokens())
    {
        TokensEnum token = getToken(tokensIndex);
        SerialTemp.printf("Token [%d]= ", tokensIndex);
        tokensIndex++;
        switch (token)
        {
            case notModifier_enum:
                SerialTemp.println("notModifier_enum");
                break;
            case wildcardAny_enum:
                SerialTemp.println("wildcardAny_enum");
                break;
            case booleanOr_enum:
                SerialTemp.println("booleanOr_enum");
                break;
            case booleanAnd_enum:
                SerialTemp.println("booleanAnd_enum");
                break;
            case identifier_enum:
            {
                IdentifierStruct idQueryStruct = getIdentifierStruct(idIndex++);
                char *id = idQueryStruct.identifier;
                SerialTemp.printf("identifier_enum: %s%s%s%s\n",
                                  idQueryStruct.notModifier?"!":"",
                                  idQueryStruct.wildcardBefore?"*":"",
                                  id,
                                  idQueryStruct.wildcardAfter?"*":""
                                  );
                
            }
                break;
        };
    }
}
//!printout
void testMatchResult(char*query, boolean expectedBool)
{
    boolean match = queryMatchesName_mainModule(query);
    SerialTemp.printf("query '%s' ", query);
    if (match != expectedBool)
        SerialTemp.printf("!= expected: ");
    else
        SerialTemp.printf("== matched : ");
    SerialTemp.printf("%s\n", expectedBool?(char*)"MATCH":(char*)"NO-MATCH");
    
}

boolean _firstTimeParserTest = true;

//!test
void testParser_mainModule()
{
    int tokens;
    boolean match;
    parseQueryLine_mainModule((char*)"!M5WRR");
    //printTokenTree();
    testMatchResult((char*)"M5WRR", false);
    testMatchResult((char*)"M5Rainier", true);
    
    parseQueryLine_mainModule((char*)"!M5WRR & !DukeGEN3");
    //printTokenTree();
    testMatchResult((char*)"M5WRR", false);
    testMatchResult((char*)"DukeGEN3", false);
    testMatchResult((char*)"ScoobyDoo", true);
    testMatchResult((char*)"ScottyGEN3", true);

    parseQueryLine_mainModule((char*)"M5* | DukeGEN3");
    testMatchResult((char*)"M5Scooby", true);
    testMatchResult((char*)"DukeGEN3", true);
    
    
    parseQueryLine_mainModule((char*)"!*GEN3*");
    testMatchResult((char*)"M5Scooby", true);
    testMatchResult((char*)"DukeGEN3", false);

    parseQueryLine_mainModule((char*)"!M5* ");
    testMatchResult((char*)"M5Scooby", false);
    testMatchResult((char*)"DukeGEN3", true);

    parseQueryLine_mainModule((char*)"!M5* & !*GEN3*");
    testMatchResult((char*)"M5Scooby", false);
    testMatchResult((char*)"DukeGEN3", false);
    testMatchResult((char*)"M5DukeGEN3", false);
    testMatchResult((char*)"M5DukeGEN3", true);

    parseQueryLine_mainModule((char*)"!M5* | *GEN3*");
    testMatchResult((char*)"M5Scooby", false);
    testMatchResult((char*)"DukeGEN3", true);
    testMatchResult((char*)"M5DukeGEN3", false); //interesting

    parseQueryLine_mainModule((char*)"!M5*");
    testMatchResult((char*)"M5WRR", false);
    testMatchResult((char*)"DukeGEN3", true);

    parseQueryLine_mainModule((char*)"!M5WRR");
    testMatchResult((char*)"M5WRR", false);
    testMatchResult((char*)"M5Scooby", true);

    parseQueryLine_mainModule((char*)"*M5* |*GEN3* | ScoobyDoo");
    testMatchResult((char*)"M5Scooby", true);
    testMatchResult((char*)"GEN3Scott", true);
    testMatchResult((char*)"ScoobyDoo", true);
    testMatchResult((char*)"Duke", false);
    testMatchResult((char*)"M5Duke", true);

    parseQueryLine_mainModule((char*)"ScoobyDoo");
    testMatchResult((char*)"ScoobyDoo", true);
    testMatchResult((char*)"Duke", false);

    parseQueryLine_mainModule((char*)"!*M5* | *GEN3*  &ScoobyDoo");
    testMatchResult((char*)"ScoobyDoo", true);
    testMatchResult((char*)"M5WRR", false);
    testMatchResult((char*)"GEN3Scott", true);

    parseQueryLine_mainModule((char*)"!*M5* | *GEN3* | ScoobyDoo");
    testMatchResult((char*)"ScoobyDoo", true);
    testMatchResult((char*)"M5WRR", false);
    testMatchResult((char*)"GEN3Scott", true);
    
    //! all Room1 which are M5. eg.  Room1_M5Scooby
    parseQueryLine_mainModule((char*)"Room1_* & *M5*");
    testMatchResult((char*)"Room1_M5Scooby", true);
    testMatchResult((char*)"Room1_GEN3", false);
    testMatchResult((char*)"Room2_GEN3", false);
    testMatchResult((char*)"M5WRR", false);

    //! all Room1 which are M5. eg.  Room1_M5Scooby
    parseQueryLine_mainModule((char*)"Room1_* | *M5*");
    testMatchResult((char*)"Room1_M5Scooby", true);
    testMatchResult((char*)"Room1_GEN3", true);
    testMatchResult((char*)"Room2_GEN3", false);
    testMatchResult((char*)"Room2_M5", true);
    testMatchResult((char*)"M5WRR", true);

    //! all Room1 which are M5. eg.  Room1_M5Scooby
    parseQueryLine_mainModule((char*)"Room1_*");
    testMatchResult((char*)"Room1_M5Scooby", true);
    testMatchResult((char*)"Room1_GEN3", true);
    testMatchResult((char*)"Room2_GEN3", false);

    //! all Room1 which are M5. eg.  Room1_M5Scooby
    parseQueryLine_mainModule((char*)"*Room1");
    testMatchResult((char*)"Scooby_Room1", true);
    testMatchResult((char*)"Scooby_Room1_GEN3", false);
    testMatchResult((char*)"Room1", true);
    testMatchResult((char*)"Room2", false);

    //! all Room1 which are M5. eg.  Room1_M5Scooby
    parseQueryLine_mainModule((char*)"*GEN3");
    testMatchResult((char*)"Scooby_GEN3", true);
    testMatchResult((char*)"Scooby_GEN3_Room1", false);
    testMatchResult((char*)"GEN3", true);
    testMatchResult((char*)"GEN2", false);
    
    //! all Room1 which are M5. eg.  Room1_M5Scooby
    parseQueryLine_mainModule((char*)"!*GEN3");
    testMatchResult((char*)"Scooby_GEN3", false);
    testMatchResult((char*)"Scooby_GEN3_Room1", true);
    testMatchResult((char*)"GEN3", false);
    testMatchResult((char*)"GEN2", true);
    testMatchResult((char*)"M5Scooby", true);
    testMatchResult((char*)"DukeGEN3", false);
    testMatchResult((char*)"M5DukeGEN3", false);
}

#endif //test parser

//! setup a test ..
void setup_tokenParser_mainModule()
{
#ifdef TEST_PARSER
    if (_firstTimeParserTest)
    {
        _firstTimeParserTest = false;
        testParser_mainModule();
    }
#endif
}

#else


//!parses a line of text, The caller then uses queryMatchesName() to see if their name matches
void parseQueryLine_mainModule(char *line)
{
    //noop
}

//!now need to process the token tree to see if name1 and name2 match the query
//!eg.  ! *name* & name2 ...
//!wildcards after name make it slightly complicated
//!    SO:  look for wildcards before and after, and the id. then decide if that matches
//!    before the boolean and notmodifier applied
boolean queryMatchesName_mainModule(char*name)
{
    return true;
}

//! whether the stirng is a potential query. This can be used, but it is also used by the parseQueryLine
//! to optimize out the original dev:name query.
boolean stringIsQuery_mainModule(char *line)
{
    //noop
}

//! setup for the token parser (really just for testing)
void setup_tokenParser_mainModule()
{
    //noop
}

#endif
