/***************************************************************************
 * Authors:     J.M de la Rosa Trevin (jmdelarosa@cnb.csic.es)
 *
 *
 * Unidad de  Bioinformatica of Centro Nacional de Biotecnologia , CSIC
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your param) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307  USA
 *
 *  All comments concerning this program package may be sent to the
 *  e-mail address 'xmipp@cnb.csic.es'
 ***************************************************************************/

#include "argsparser.h"

//-------------------   LEXER IMPLEMENTATIONS --------------------------------

const char * ArgToken::typeString(TokenType type)
{
    switch (type)
    {
    case TOK_ID: //identifier
        return "ID";
    case TOK_OPT: // -ID or --ID
        return "OPTION";
    case TOK_INT: //integer number
        return "INT";
    case TOK_FLOAT: //float number
        return "FLOAT";
    case TOK_STR: //string value
        return "STRING";
    case TOK_MINUS: // - or --
        return "MINUS";
    case TOK_EQ: // =
        return "EQUAL";
    case TOK_COMM: // Comment: from : until end of line
        return "COMMENT";
    case TOK_LAN: // <
        return "<";
    case TOK_RAN: // >
        return ">";
    case TOK_LBRA: // [
        return "[";
    case TOK_RBRA: // ]
        return "]";
    case TOK_END: // end of input
        return "EOF";
        ///Reserved words
    case TOK_WHERE: // 'WHERE' keyword
        return "WHERE";
    case TOK_ALIAS: // 'ALIAS' keyword
        return "ALIAS";
    case TOK_SECTION:
        return "SECTION";
    case TOK_SEMI:
        return ";";
    case TOK_COMMA:
        return ",";
    case TOK_PLUS:
        return "+";
    default:
        return "UKNOWN";
    };
}

ArgLexer::ArgLexer()
{
    line = 0;
    pos = 0;
    pToken = new ArgToken();
    //Initialize reserved words dictionary
    reservedWords["WHERE"] = TOK_WHERE;
    reservedWords["ALIAS"] = TOK_ALIAS;
    reservedWords["OR"] = TOK_OR;
    reservedWords["IMPLIES"] = TOK_IMPLIES;
}

ArgLexer::~ArgLexer()
{
    delete pToken;
}

void ArgLexer::addLine(std::string line)
{
    input.push_back(line + " ");
}

inline void ArgLexer::nextLine()
{
    ++line;
    pos = 0;
}

void ArgLexer::setupToken(TokenType type)
{
    pToken->type = type;
    if (type != TOK_END)
    {
        pToken->line = line;
        pToken->start = pos;
        pToken->end = pos + offset - 1;
        pToken->lexeme = input[line].substr(pos, offset);

        if (type == TOK_ID)
        {
            std::string s = pToken->lexeme;
            std::transform(s.begin(), s.end(), s.begin(),
                           (int(*)(int)) std::toupper);
            std::map<std::string, TokenType>::iterator it;
            it = reservedWords.find(s);
            if (it != reservedWords.end())
            {
                pToken->lexeme = it->first;
                pToken->type = it->second;
            }

        }
        else if (type == TOK_STR)
            ++offset;
        pos += offset;
        offset = 1;
    }
    else
        pToken->lexeme = "EOF";
}

void ArgLexer::checkVisibility()
{
    while (input[line][pos] == '+')
    {
        ++pToken->visibility;
        ++pos;
    }
}

bool ArgLexer::nextToken()
{
    //make it visible by default
    pToken->visibility = 0;

    if (line == input.size())
    {
        setupToken(TOK_END);
        return false;
    }

    char c = input[line][pos];

    //Skip all white spaces
    while (line < input.size() && pos < input[line].length() && isspace(c
            = input[line][pos]))
    {
        ++pos;
        if (c == '\0' || c == '\n' || pos == input[line].length()) //Next line
        {
            nextLine();
        }
    }

    if (line == input.size())
        setupToken(TOK_END);
    else if (isalpha(c) || c == '_')
    {
        offset = 0;
        while (isalnum(c) || c == '_')
        {
            ++offset;
            c = input[line][pos + offset];
        }
        setupToken(TOK_ID);
    }
    else if (isalnum(c) || (c == '-' && isdigit(input[line][pos + 1])))
    {
        offset = 1;
        TokenType t = TOK_INT;
        while (isdigit(input[line][pos + offset]))
            ++offset;
        if (input[line][pos + offset] == '.')
        {
            ++offset;
            while (isdigit(input[line][pos + offset]))
                ++offset;
            t = TOK_FLOAT;
        }
        if (input[line][pos + offset] == 'e')
        {
            ++offset;
            if (input[line][pos + offset] != '+' && input[line][pos + offset]
                != '-')
            {
                std::cerr << "ERROR: expected '+' or '-' " << std::endl
                << "at line: " << line + 1 << " pos: " << offset + 1
                << std::endl;
                exit(1);
            }
            ++offset;
            while (isdigit(input[line][pos + offset]))
                ++offset;
            t = TOK_FLOAT;

        }
        setupToken(t);
    }
    else
    {
        offset = 1;
        bool empty;
        switch (c)
        {
        case '<':
            setupToken(TOK_LAN);
            break;
        case '>':
            setupToken(TOK_RAN);
            break;
        case '=':
            if (input[line][pos + offset] == '=')
            {
                pos = input[line].find_first_not_of('=', pos + 1);
                checkVisibility();
                offset = input[line].find_first_of('=', pos + 1);
                offset -= pos;
                setupToken(TOK_SECTION);
                nextLine();
            }
            else
                //simple equal sign '='
                setupToken(TOK_EQ);
            break;
        case '[':
            setupToken(TOK_LBRA);
            break;
        case ']':
            setupToken(TOK_RBRA);
            break;
        case ';':
            setupToken(TOK_SEMI);
            break;
        case ',':
            setupToken(TOK_COMMA);
            break;
        case '+':
            setupToken(TOK_PLUS);
            break;
        case '.':
            if (input[line][pos + offset] == '.' &&
                input[line][pos + offset + 1] == '.')
            {
                offset += 2;
                setupToken(TOK_ETC);
            }
            break;
        case ':':
            ++pos;

            checkVisibility();

            offset = input[line].find_first_of("\n\r", pos);
            offset -= pos;
            setupToken(TOK_COMM);
            nextLine();
            break;
        case '-':
            if (input[line][pos + offset] == '-')
                ++offset;
            empty = true;
            c = input[line][pos + offset];
            if (isalpha(c) || c == '_') //should start with letter or _
            {
                while (isalnum(c) || c == '_')
                {
                    empty = false;
                    ++offset;
                    c = input[line][pos + offset];
                }
            }
            if (empty)
            {
                std::cerr << "ERROR: Params should be of the form -ID or --ID"
                << std::endl;
                exit(1);
            }
            setupToken(TOK_OPT);
            checkVisibility();
            break;
        case '"':
            offset = input[line].find_first_of('"', pos + 1);
            ++pos;
            offset -= pos;
            setupToken(TOK_STR);
            break;
        default:
            std::cerr << "ERROR: Unexpected character '" << c << "'"
            << std::endl << "at line: " << line + 1 << " pos: " << pos
            + 1 << std::endl;
            exit(1);
        }
    }

    //ConsolePrinter * cp = new ConsolePrinter();
    //cp->printToken(pToken);

    return true;
}

ArgToken * ArgLexer::currentToken() const
{
    return pToken;
}
TokenType ArgLexer::lookahead() const
{
    return pToken->type;
}


//-------------------   PARSER IMPLEMENTATIONS   --------------------------------
void CommentList::addComment(std::string comment, int visible)
{
    comments.push_back(comment);
    visibility.push_back(visible);
}

void CommentList::clear()
{
    comments.clear();
    visibility.clear();
}
size_t CommentList::size() const
{
    return comments.size();
}

ASTNode::ASTNode(ArgLexer *lexer, ASTNode * parent)
{
    pLexer = lexer;
    visible = 0;
    this->parent = parent;
}

TokenType ASTNode::lookahead() const
{
    return pLexer->lookahead();
}

bool ASTNode::lookahead(TokenType type) const
{
    return pLexer->lookahead() == type;
}

ArgToken * ASTNode::currentToken() const
{
    return pLexer->currentToken();
}

void ASTNode::nextToken()
{
    pLexer->nextToken();
}

bool ASTNode::consume(TokenType type)
{
    TokenType t = lookahead();
    if (t != type)
        unexpectedToken();
    //Store consumed token
    token = *currentToken();

    //Ask for new token
    nextToken();
    return true;
}

bool ASTNode::parseCommentList(CommentList &comments)
{
    comments.clear();
    //Comment List (CL)
    //CL -> comment  CL | e
    while (lookahead(TOK_COMM))
    {
        consume(TOK_COMM);
        comments.addComment(token.lexeme, token.visibility);
    }

    return true;
}

void ASTNode::error(std::string msg)
{
    std::cerr << ">>> ERROR: " << msg << std::endl << "    at line "
    << token.line + 1 << " column " << token.start + 1 << std::endl;
    exit(1);
}

void ASTNode::unexpectedToken(std::string msg)
{
    token = *currentToken();
    error((std::string) "Unexpected token '" + ArgToken::typeString(token.type)
          + "' " + msg);
}

ArgumentDef::ArgumentDef(ArgLexer *lexer, ASTNode * parent) :
        ASTNode(lexer, parent)
{
    isList = false;
    hasDefault = false;
}

bool ArgumentDef::parse()
{
    //     A -> < ID DEF > | <...>
    //   DEF -> = VALUE | e
    // VALUE -> INT | FLOAT | STRING
    consume(TOK_LAN);
    if (lookahead(TOK_ID))
    {
        consume(TOK_ID);
        name = token.lexeme;
        if (lookahead(TOK_EQ))
        {
            consume(TOK_EQ);
            //Consume a value, that can be int, float or string
            TokenType t = lookahead();

            if (t == TOK_INT || t == TOK_FLOAT || t == TOK_STR || t == TOK_ID)
                consume(t);
            else
                unexpectedToken(" expecting INT, FLOAT, STRING or ID.");

            hasDefault = true;
            argDefault = token.lexeme;
        }
    }
    else
    {
        consume(TOK_ETC);
        name = token.lexeme;
        isList = true;
    }
    consume(TOK_RAN);
    return true;
}

bool ArgumentDef::acceptArguments(std::stringstream &errors, size_t & index, std::vector<const char *> &cmdArguments)
{
    if (index == cmdArguments.size())
    {
        if (hasDefault)
        {
            cmdArguments.push_back(argDefault.c_str());
        }
        else
        {
            errors << "Not enough arguments, <" << name << "> has not default. ";
            return false;
        }
    }

    if (isList)
        return true;

    if (!subParams.empty())
    {
        bool found = false;
        std::string optionValue = (std::string)cmdArguments[index];
        for (size_t i = 0; i < subParams.size(); ++i)
        {
            if (subParams[i]->name == optionValue)
            {
                found = true;
                ++index;
                for (size_t j = 0; j < subParams[i]->arguments.size(); ++j)
                    if (!subParams[i]->arguments[j]->acceptArguments(errors, index, cmdArguments))
                        return false;
                break;
            }
        }
        if (!found)
        {
            errors << optionValue << " is not a valid option for <" << name <<"> ";
            return false;
        }
    }

    //if not list increment index
    ++index;

    return true;
}

ParamDef::ParamDef(ArgLexer *lexer, ASTNode * parent) :
        ASTNode(lexer, parent)
{}

bool ParamDef::containsArgument(const std::string & argName)
{
    return findArgument(argName) == NULL;
}

ArgumentDef * ParamDef::findArgument(const std::string & argName)
{
    for (size_t i = 0; i < arguments.size(); ++i)
        if (argName == arguments[i]->name)
            return arguments[i];
    return NULL;
}

bool ParamDef::containsAlias(const std::string & alias)
{
    for (size_t i = 0; i < aliases.size(); ++i)
        if (alias == aliases[i])
            return true;
    return false;
}

bool ParamDef::parse()
{
    notOptional = true;
    orBefore = false;
    counter = 0;

    //Param Definition(OD)
    //OD -> OH CL
    //Param Header(OH)
    //OH -> O | [O] | OR O
    //Param(O)
    //O -> param AL
    if (lookahead(TOK_LBRA))
    {
        consume(TOK_LBRA);
        notOptional = false;
    }
    else if (lookahead(TOK_OR))
    {
        consume(TOK_OR);
        orBefore = true;
    }
    consume(TOK_OPT);
    name = token.lexeme;
    visible = token.visibility;

    ((ProgramDef*) parent->parent)->addParamName(name, this);

    //Parse argument list
    parseArgumentList();

    if (notOptional == false)
        consume(TOK_RBRA);

    //Parse comment list
    parseCommentList(comments);

    //WHERE section
    if (lookahead(TOK_WHERE))
    {
        consume(TOK_WHERE);
        while (lookahead(TOK_LAN))
        {
            consume(TOK_LAN);
            consume(TOK_ID);
            ArgumentDef * pArg = findArgument(token.lexeme);
            if (pArg == NULL)
            {
                std::cerr << "ERROR; on WHERE definition.\n Param '" << name
                << "' not contains argument '" << token.lexeme << "'"
                << std::endl;
                exit(1);
            }
            consume(TOK_RAN);

            while (lookahead(TOK_ID))
            {
                ParamDef * pOpt = new ParamDef(pLexer, this);
                pOpt->consume(TOK_ID);

                pOpt->name = pOpt->token.lexeme;
                pOpt->parseArgumentList();
                pOpt->parseCommentList(pOpt->comments);
                pArg->subParams.push_back(pOpt);
            }

        }
    }

    //ALIAS section
    parseParamList(TOK_ALIAS, aliases, true);

    //IMPLIES section
    parseParamList(TOK_IMPLIES, implies, false);

    return true;
}

bool ParamDef::parseArgumentList()
{
    bool previousList = false; // to check only one list and at end of arguments
    bool previousDefault = false; // to check that default values only can be at end
    //Argument List (AL)
    //AL -> argument  AL | e
    while (lookahead(TOK_LAN))
    {
        ArgumentDef * arg = new ArgumentDef(pLexer, this);
        arg->parse();
        token = arg->token;

        if (previousList)
            error("A list <...> has found not at the end of argument list");

        if (previousDefault && !arg->hasDefault)
            error("A non default argument was found before a default one");

        previousList = arg->isList;
        previousDefault = arg->hasDefault;

        arguments.push_back(arg);
    }
    return true;
}

bool ParamDef::parseParamList(TokenType startToken, StringVector &paramList,
                              bool isAlias)
{
    paramList.clear();
    if (lookahead(startToken))
    {
        consume(startToken);
        consume(TOK_OPT);
        paramList.push_back(token.lexeme);

        if (isAlias)
            ((ProgramDef*) parent->parent)->addParamName(token.lexeme, this);
        else
            ((ProgramDef*) parent->parent)->addParamImplies(token.lexeme);

        while (lookahead(TOK_COMMA))
        {
            consume(TOK_COMMA);
            consume(TOK_OPT);
            paramList.push_back(token.lexeme);
            if (isAlias)
                ((ProgramDef*) parent->parent)->addParamName(token.lexeme, this);
            else
                ((ProgramDef*) parent->parent)->addParamImplies(token.lexeme);
        }
        consume(TOK_SEMI);
    }

    return true;
}

void ParamDef::check(std::stringstream & errors)
{
    if (counter > 1 )
    {
        errors << "Duplicated parameter: " << name << " (check alias)" << std::endl;
        return;
    }

    if (counter == 1)
    {
        //Check implies restrictions
        ParamDef * param;
        for (size_t i = 0; i < implies.size(); ++i)
        {
            param = ((ProgramDef*) parent->parent)->findParam(implies[i]);
            if (param->counter < 1)
                errors << "Parameter " << name << " requires " << implies[i] << std::endl;
        }

        //Check the number of arguments
        if (arguments.empty()) //if not arguments
        {
            if (!cmdArguments.empty())
                errors << "Parameter " << name << " doesn't take any argument, "
                << cmdArguments.size() << " provided." << std::endl;
        }
        else
        {
            size_t argIndex = 0;

            for (size_t i = 0; i < arguments.size(); ++i)
                if (!arguments[i]->acceptArguments(errors, argIndex, cmdArguments))
                {
                    errors << " parameter: " << name << std::endl;
                    return;
                }
        }
    }
    else
    {
        //Fill default arguments
        for (size_t i = 0; i < arguments.size(); ++i)
            if (arguments[i]->hasDefault)
                cmdArguments.push_back(arguments[i]->argDefault.c_str());
    }


}

SectionDef::SectionDef(ArgLexer * lexer, ASTNode * parent) :
ASTNode(lexer, parent)
{}

bool SectionDef::parse()
{
    if (lookahead(TOK_SECTION))
    {
        consume(TOK_SECTION);
        name = token.lexeme;
        visible = token.visibility;
        parseCommentList(comments);
    }

    //OL -> params OD ODL | SD ODL | e
    TokenType t = lookahead();

    if (!(t == TOK_OPT || t == TOK_OR || t == TOK_LBRA))
        unexpectedToken("parsing section, expecting param definition");

    while (t == TOK_OPT || t == TOK_OR || t == TOK_LBRA)
    {
        ParamDef * param = new ParamDef(pLexer, this);
        param->parse();
        params.push_back(param);
        t = lookahead();
    }

    return true;
}

ProgramDef::ProgramDef(ArgLexer * lexer) :
        ASTNode(lexer)
{}

/** Parse the program definition. */
bool ProgramDef::parse()
{
    // P -> ID CL OL
    //consume(TOK_ID);
    //name = token.lexeme;
    //Usage comments
    //parseCommentList(usageComments);

    while (!lookahead(TOK_END))
    {
        SectionDef * s = new SectionDef(pLexer, this);
        s->parse();
        sections.push_back(s);
    }
    consume(TOK_END);

    return true;
}

void addOcurrence(std::map<std::string, int> &map, const std::string &name)
{
    if (map.find(name) != map.end())
        map[name]++;
    else
        map[name] = 1;
}

void reportExclusiveErrors(std::stringstream & errors, std::vector<ParamDef*> &exclusive)
{
    if (exclusive.empty())
        return;

    std::vector<ParamDef*> exclusive2;
    for (size_t i = 0; i < exclusive.size(); ++i)
        if (exclusive[i]->counter == 1)
            exclusive2.push_back(exclusive[i]);
    if (exclusive2.size() > 1)
    {
        errors << "Parameters ";
        for (size_t i = 0; i < exclusive.size() - 1; ++i)
            errors << exclusive[i]->name << " ";
        errors << "and " << exclusive[exclusive.size()-1]->name << " are mutually exclusive (check alias)" << std::endl;
    }
    else if (exclusive2.empty() && exclusive[0]->notOptional)
    {
        errors << "You should provide parameter " << exclusive[0]->name;
        for (size_t i = 1; i < exclusive.size(); ++i)
            errors << " or " << exclusive[i]->name;
        errors << std::endl;
    }
    exclusive.clear();
}

void ProgramDef::check(std::stringstream & errors)
{
    std::vector<ParamDef*> exclusive;
    bool isAlias = false;
    SectionDef * section;
    ParamDef * param;

    for (size_t i = 0; i < sections.size(); ++i)
    {
        section = sections[i];
        for (size_t j = 0; j < section->params.size(); ++j)
        {
            param = section->params[j];
            std::string name = param->name;
            bool orBefore = param->orBefore;
            size_t size = exclusive.size();
            //Doesn't check for alias, for doesn't repeat error messages
            param->check(errors);

            std::cerr << "name: " << name << std::endl;
            if (!param->orBefore)
            {
                reportExclusiveErrors(errors, exclusive);
                std::cerr << "-----------------------------" <<std::endl;
            }
            exclusive.push_back(param);
        }
    }
    reportExclusiveErrors(errors, exclusive);
}

ParamDef * ProgramDef::findParam(const std::string &name)
{
    if (paramsMap.find(name) != paramsMap.end())
        return paramsMap[name];
    return NULL;
}

void ProgramDef::addParamName(const std::string &name, ParamDef * param)
{
    if (paramsMap.find(name) != paramsMap.end())
        error((std::string) "The param '" + name + "' is repeated.");
    else
        paramsMap[name] = param;
}
void ProgramDef::addParamImplies(const std::string &name)
{
    pendingImplies.push_back(name);
}

void ProgramDef::read(int argc, char ** argv)
{
    std::stringstream errors;
    //Set the name with the first argument
    name = argv[0];
    //We assume that all options start with -
    if (argc > 1 && argv[1][0] != '-')
        REPORT_ERROR(ERR_ARG_INCORRECT, "Parameters should start with a -");

    ParamDef * param = NULL;

    //Read command line params and arguments
    for (int i = 1; i < argc; ++i)
    {
        if (argv[i][0] == '-')
        {
            param = findParam(argv[i]);
            if (param == NULL)
                errors << "Unrecognized parameter: " << argv[i] << std::endl;
            else
                param->counter++;
        }
        else if (param != NULL)
            param->cmdArguments.push_back(argv[i]);
    }

    check(errors);

    //Report errors found
    if (errors.str().length() > 0)
    {
        //Unrecognized parameters
        //for (size_t i = 0; i < unrecognized.size(); ++i)
        //    std::cerr << "Unrecognized parameter: " << unrecognized[i] << std::endl;
        REPORT_ERROR(ERR_ARG_BADCMDLINE, errors.str().c_str());
    }
}

const char * ProgramDef::getParam(const char * paramName, int argNumber)
{
    ParamDef * param = findParam(paramName);
    if (param == NULL)
        REPORT_ERROR(ERR_ARG_INCORRECT, ((std::string)"Doesn't exist param " + paramName).c_str());
    return paramsMap[paramName]->cmdArguments.at(argNumber);
}

//-------------------   PRINTER IMPLEMENTATIONS   --------------------------------

void ConsolePrinter::printProgram(const ProgramDef &program, int v)
{
    std::cout << "PROGRAM" << std::endl << "   " << program.name << std::endl;
    if (program.usageComments.size() > 0)
    {
        std::cout << "USAGE" << std::endl;
        printCommentList(program.usageComments, v);
    }
    if (program.sections.size() > 0)
    {
        std::cout << "OPTIONS" << std::endl;
        for (size_t i = 0; i < program.sections.size(); ++i)
            printSection(*program.sections[i], v);
    }
}

void ConsolePrinter::printSection(const SectionDef &section, int v)
{
    if (section.visible <= v)
    {
        std::cout << std::endl;
        if (section.name.length() > 0)
            std::cout << section.name << std::endl;
        for (size_t i = 0; i < section.params.size(); ++i)
            printParam(*section.params[i], v);
    }
}

void ConsolePrinter::printParam(const ParamDef &param, int v)
{
    if (param.visible <= v)
    {
        std::cout << "   " << param.name;
        //print alias
        for (size_t i = 0; i < param.aliases.size(); ++i)
            std::cout << ", " << param.aliases[i];
        //print arguments
        for (size_t i = 0; i < param.arguments.size(); ++i)
        {
            std::cout << " ";
            printArgument(*param.arguments[i], v);
        }
        std::cout << std::endl;
        printCommentList(param.comments, v);
        for (size_t i = 0; i < param.arguments.size(); ++i)
        {
            ArgumentDef &arg = *param.arguments[i];
            if (!arg.subParams.empty())
            {
                std::cout << "      where <" << arg.name << "> can be:" << std::endl;
                for (size_t j = 0; j < arg.subParams.size(); ++j)
                    std::cout << "        " << arg.subParams[j]->name << std::endl;
            }
        }

    }
}

void ConsolePrinter::printArgument(const ArgumentDef & argument, int v)
{
    std::cout << "<" << argument.name;
    if (argument.hasDefault)
        std::cout << "=" << argument.argDefault;
    std::cout << ">";
}

void ConsolePrinter::printCommentList(const CommentList &comments, int v)
{
    for (size_t i = 0; i < comments.size(); ++i)
        if (comments.visibility[i] <= v)
            std::cout << "          " << comments.comments[i] << std::endl;
}

void ConsolePrinter::printToken(ArgToken * token)
{

    std::cerr << "token: '" << token->lexeme
    << "' type: " << ArgToken::typeString(token->type)
    << " line: " << token->line + 1
    << " pos: " << token->start + 1 << std::endl;
}
