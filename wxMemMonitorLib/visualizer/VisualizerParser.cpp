
#include "stdafx.h"
#include "VisualizerParser.h"
#include "../Control/Global.h"
#include "../ui/LogWindow.h"

using namespace visualizer;
using namespace parser;
using namespace std;


parser::CParser::CParser() 
{
	m_pScan = new CScanner();
	m_IsTrace = FALSE;
	m_IsErrorOccur = FALSE;

}


parser::CParser::~CParser()
{
	SAFE_DELETE(m_pScan);

}


//---------------------------------------------------------------------
// 튜토리얼 스크립트를 파싱한다.
//---------------------------------------------------------------------
SVisualizerScript* parser::CParser::Parse( const std::string &fileName, BOOL bTrace )
{
	if( !m_pScan->LoadFile(fileName.c_str(), bTrace) )
		return NULL;

	m_fileName = fileName;
	//	cout << fileName;
	//((CMainFrame*)::AfxGetMainWnd())->GetOutputWnd().AddString(L"Compile...");
	//((CMainFrame*)::AfxGetMainWnd())->GetOutputWnd().AddString( 
	//	common::str2wstr(fileName).c_str() );	

	m_Token = m_pScan->GetToken();
	if( ENDFILE == m_Token )
	{
		m_pScan->Clear();
		return NULL;
	}

	SVisualizerScript *script = visualizerscript();

	if( ENDFILE != m_Token )
	{
		SyntaxError( " code ends before file " );
		PrintToken( m_Token, m_pScan->GetTokenStringQ(0) );
		m_pScan->Clear();
		RemoveVisualizerScript(script);
		return NULL;
	}

	return script;
}


// 		script -> (autoexpand | visualizer) [script]
SVisualizerScript* parser::CParser::visualizerscript()
{
	SVisualizerScript *pVisScr = NULL;

	SType_Stmts *pMatchType = types();
	if (!pMatchType)
		return NULL;

	SAutoExp *pAutoExp = autoexpand();
	if (pAutoExp)
	{
		pVisScr = new SVisualizerScript;
		pVisScr->next = NULL;

		pAutoExp->match_type = pMatchType;
		pVisScr->autoexp = pAutoExp;
		pVisScr->kind = VisualizerScript_AutoExp;
	}
	else
	{
		SVisualizer *pVis = visualizer();
		if (pVis)
		{
			pVisScr = new SVisualizerScript;
			pVisScr->next = NULL;

			pVis->matchType = pMatchType;
			pVisScr->vis = pVis;
			pVisScr->kind = VisualizerScript_Visualizer;
		}
		else
		{
			parser::RemoveType_Stmts(pMatchType);
		}
	}
	if (pVisScr)
		pVisScr->next = visualizerscript();

	return pVisScr;
}

// visualizer rule
// 		visualizer -> 	match_typename '{' preview '(' preview_string_expression ')'
// 			stringview '(' text-visualizer-expression ')'
// 			children '(' expanded-contents-expression ')'
// 			'}'
SVisualizer* parser::CParser::visualizer()
{
	SVisualizer *p=NULL;
	if (LBRACE == m_Token)
	{
		Match(LBRACE);
		p = new SVisualizer;
		p->preview = NULL;
		p->stringview = NULL;
		p->children = NULL;
		p->graph = NULL;

		bool loop = true;
		while (RBRACE != m_Token && loop)
		{
			switch (m_Token)
			{
			case PREVIEW: p->preview = preview(); break;
			case STRINGVIEW: p->stringview = stringview(); break;
			case CHILDREN: p->children = children(); break;
			case GRAPH: p->graph = graph(); break;
			default: loop = false; break;
			}
		}
		Match(RBRACE);
	}
	return p;
}

 // preview '(' preview_string_expression ')'
SStatements* parser::CParser::preview()
{
	SStatements*p = NULL;
	if (PREVIEW == m_Token)
	{
		Match(PREVIEW);
		Match(LPAREN);
		p = statements();
		Match(RPAREN);
	}
	return p;
}

//stringview '(' text-visualizer-expression ')'
SStatements* parser::CParser::stringview()
{
	SStatements*p = NULL;
	if (STRINGVIEW == m_Token)
	{
		Match(STRINGVIEW);
		Match(LPAREN);
		p = statements();
		Match(RPAREN);
	}
	return p;
}

//children '(' expanded-contents-expression ')'
SStatements* parser::CParser::children()
{
	SStatements*p = NULL;
	if ( CHILDREN == m_Token)
	{
		Match(CHILDREN);
		Match(LPAREN);
		p = statements();
		Match(RPAREN);
	}
	return p;
}

// graph '(' text-visualizer-expression  ')'
SStatements* parser::CParser::graph()
{
	SStatements*p = NULL;
	if (GRAPH == m_Token)
	{
		Match(GRAPH);
		Match(LPAREN);
		p = statements();
		Match(RPAREN);
	}
	return p;
}


// 			match_typename ->	type { | type }
// 		;

// 		preview_string_expression ->		bracket_stmt 
// 			;
// 		text-visualizer-expression ->	bracket_stmt 
// 			;
// 		expanded-contents-expression ->	bracket_stmt 
// 			;

// 		bracket_stmt ->	'#(' statement_list ')'
// 			;

// 		statement_list->	(expression | simple_exp | vis_command)  [, statement_list]
// 		| e
// 			;
SStatements* parser::CParser::statements()
{
	SStatements *stmt;
	if (IsVisCommand(m_Token))
	{
		stmt = viscommand();
	}
	else if (SHARP_BRACKET == m_Token)
	{
		Match(SHARP_BRACKET);
		Match(LPAREN);
		stmt = statements();
		Match(RPAREN);
	}
	else
	{
		bool isSimplExp = false;
		if (LBRACKET == m_Token)
		{
			int i=1;
			for (; i < 7; ++i)
			{
				const Tokentype tok = m_pScan->GetTokenQ(i);
				char *pp = m_pScan->GetTokenStringQ(i);
				if (RBRACKET == tok)
				{
					const Tokentype nextTok = m_pScan->GetTokenQ(i+1);
					if (COLON == nextTok)
					{
						isSimplExp = true;
						break;
					}					
				}
			}			
		}

		SExpression *text = NULL;
		if (isSimplExp)
		{
			text = NewExpression(StringK);
			Match(LBRACKET);
			int cnt = 0; // 오류처리를 위한 코드
			while (RBRACKET != m_Token && cnt < 10)
			{
				text->str += m_pScan->GetTokenStringQ(0);
				text->str += " ";
				Match(m_Token);
				++cnt;
			}
			Match(RBRACKET);
		}
		else
		{
			text = expression();
		}		

		if (COLON == m_Token)
		{
			SSimpleExp *sim_exp = simple_exp();
			sim_exp->text = text;

			stmt = NewStatement(Stmt_SimpleExpression);
			stmt->simple_exp = sim_exp;
		}
		else
		{
			stmt = NewStatement(Stmt_Expression);
			stmt->exp = text;
		}
	}

	if (COMMA == m_Token)
	{
		Match(COMMA);
		stmt->next = statements();
	}
	return stmt;
}

// , format
Display_Format parser::CParser::display_format()
{
	Match(COMMA);
	Display_Format reval = Disp_D;

	if (std::string("d") == m_pScan->GetTokenStringQ(0)
		|| std::string("i") == m_pScan->GetTokenStringQ(0))
		reval = Disp_D;
	else if (std::string("u") == m_pScan->GetTokenStringQ(0))
		reval = Disp_U;
	else if (std::string("o") == m_pScan->GetTokenStringQ(0))
		reval = Disp_O;
	else if (std::string("x") == m_pScan->GetTokenStringQ(0)
		|| std::string("X") == m_pScan->GetTokenStringQ(0))
		reval = Disp_X;
	else if (std::string("l") == m_pScan->GetTokenStringQ(0)
		|| std::string("h") == m_pScan->GetTokenStringQ(0))
		reval = Disp_L;
	else if (std::string("f") == m_pScan->GetTokenStringQ(0))
		reval = Disp_F;
	else if (std::string("e") == m_pScan->GetTokenStringQ(0))
		reval = Disp_E;
	else if (std::string("g") == m_pScan->GetTokenStringQ(0))
		reval = Disp_G;
	else if (std::string("c") == m_pScan->GetTokenStringQ(0))
		reval = Disp_C;
	else if (std::string("s") == m_pScan->GetTokenStringQ(0))
		reval = Disp_S;
	else if (std::string("su") == m_pScan->GetTokenStringQ(0))
		reval = Disp_SU;

	if (NEG == m_Token)
	{
		reval = Disp_Auto;
		Match(NEG);
	}
	else
	{
		Match(ID);
	}
	return reval;
}


bool parser::CParser::IsVisCommand(Tokentype tok)
{
	switch(tok)
	{
	case IF:
	case ELIF:
	case ELSE:
	case ARRAY:
	case LIST:
	case TREE:
	case VERTICAL:
	case HORIZONTAL:
		return true;
	}
	return false;
}


// 		simple_exp ->	text ':' '[' expression [,format] ']'
//							| '[' expression [,format] ']'
//							| text ':' expression
// 			;
SSimpleExp* parser::CParser::simple_exp()
{ 
	SSimpleExp *p = NULL;

	p = new SSimpleExp;

	Match(COLON);

	if (LBRACKET == m_Token)
	{
		Match(LBRACKET);
		p->expr = expression();

		// format
		if (COMMA == m_Token)
			p->format = display_format();
		Match(RBRACKET);
	}
	else
	{
		p->expr = expression();
	}

	return p;
}

// 		vis_command ->	if_stmt | array_stmt | list_stmt | tree_stmt | vert_stmt | horz_stmt
// 			;
SStatements* parser::CParser::viscommand() 
{
	SStatements *p=NULL;
	switch(m_Token)
	{
	case IF:
		p = NewStatement(Stmt_If);
		p->if_stmt = if_stmt();
		break;

	case ARRAY:
	case LIST:
	case TREE:
		{
			p = NewStatement(Stmt_Bracket_Iterator);
			p->itor_stmt = visbracketiterator_stmt();
		}
		break;

	case VERTICAL:
	case HORIZONTAL:
		p = NewStatement(Stmt_GVis);
		p->gvis_stmt = gvis_stmts();
		break;

	default:
		return NULL;
	}
	return p;
}


// 		if_stmt ->	'#if' '(' expression ')' '(' statement_list')'  
// 			[ '#elif' '(' expression ')' '(' statement_list')' ]
// 		[ '#else' '(' statement_list ')' ]
// 		;
SIf_Stmt* parser::CParser::if_stmt() 
{
	SIf_Stmt *p = new SIf_Stmt;
	p->elif_stmt = NULL;
	p->else_stmts = NULL;

	Match(IF);
	Match(LPAREN);
	p->cond = expression();
	Match(RPAREN);

	Match(LPAREN);
	p->stmts = statements();
	Match(RPAREN);

	if (ELIF == m_Token)
		p->elif_stmt = elif_stmt();

	if (ELSE == m_Token)
	{
		Match(ELSE);
		Match(LPAREN);
		p->else_stmts = statements();
		Match(RPAREN);
	}

	return p;
}


// #elif' '(' expression ')' '(' statement_list')' 
SElif_Stmt* parser::CParser::elif_stmt()
{
	SElif_Stmt *p = NULL;
	Match(ELIF);
	Match(LPAREN);
	p->cond = expression();
	Match(RPAREN);

	Match(LPAREN);
	p->stmts = statements();
	Match(RPAREN);

	if (ELIF == m_Token)
		p->next = elif_stmt();
	return p;
}


// 		array_stmt ->	'#array' '(' 	bracket_inner_stmt ')' [: expression]
// 		;
// 
// 		list_stmt ->	'#list' '(' bracket_inner_stmt ')' [: expression]
// 		;
// 
// 		tree_stmt ->	'#tree' '(' bracket_inner_stmt ')'  [: expression]
// 		;
SVisBracketIterator_Stmt* parser::CParser::visbracketiterator_stmt() 
{
	SVisBracketIterator_Stmt *p = NULL;

	VisBracketIteratorKind kind;
	switch(m_Token)
	{
	case ARRAY:  kind = Iter_Array; break;
	case LIST: kind = Iter_List; break;	
	case TREE: kind = Iter_Tree; break;	
	default: return NULL;
	}
	Match(m_Token);

	p = new SVisBracketIterator_Stmt;
	p->disp_stmt = NULL;
	p->kind = kind;
	p->stmts = bracket_inner_stmts();

	if (COLON == m_Token)
	{
		Match(COLON);
		p->disp_stmt = statements();
	}
	return p;
}

//		switch_stmt // 생략

// 			bracket_inner_stmt -> (expr_expression | size_expression |
// 											rank_expression | base_expression | left_expression | 
// 											right_expression | skip_expression | head_expression)
// 											[, bracket_inner_stmt]
// 											| e
// 			;
SBracket_Inner_Stmts* parser::CParser::bracket_inner_stmts() 
{
	SBracket_Inner_Stmts *p = new SBracket_Inner_Stmts;
	ZeroMemory(p, sizeof(SBracket_Inner_Stmts));

	Match(LPAREN);

	while (RPAREN != m_Token)
	{
		if (COMMA == m_Token)
			Match(COMMA);

		if (std::string("expr") == m_pScan->GetTokenStringQ(0))
		{
			Match(ID);
			Match(COLON);
			p->expr = expression();
		}
		else if (std::string("head") == m_pScan->GetTokenStringQ(0))
		{
			Match(ID);
			Match(COLON);
			p->head = expression();
		}
		else if (std::string("skip") == m_pScan->GetTokenStringQ(0))
		{
			Match(ID);
			Match(COLON);
			p->skip = expression();
		}
		else if (std::string("size") == m_pScan->GetTokenStringQ(0))
		{
			Match(ID);
			Match(COLON);
			p->size = expression();
		}
		else if (std::string("rank") == m_pScan->GetTokenStringQ(0))
		{
			Match(ID);
			Match(COLON);
			p->rank = expression();
		}
		else if (std::string("base") == m_pScan->GetTokenStringQ(0))
		{
			Match(ID);
			Match(COLON);
			p->base = expression();
		}
		else if (std::string("next") == m_pScan->GetTokenStringQ(0))
		{
			Match(ID);
			Match(COLON);
			p->next = expression();
		}
		else if (std::string("left") == m_pScan->GetTokenStringQ(0))
		{
			Match(ID);
			Match(COLON);
			p->left = expression();
		}
		else if (std::string("right") == m_pScan->GetTokenStringQ(0))
		{
			Match(ID);
			Match(COLON);
			p->right = expression();
		}
		else
		{
			break; // error
		}
	}
	Match(RPAREN);
	return p;
}


/**
 @brief 
	gvis_stmt -> 'expr' : expression
 @date 2013-12-16
*/
SGVis_Stmt *parser::CParser::gvis_stmts()
{
	SGVis_Stmt *p = NULL;

	if ((VERTICAL == m_Token) || (HORIZONTAL == m_Token))
	{
		p = new SGVis_Stmt;
		p->kind = (VERTICAL==m_Token)? GVis_Vertivcal : GVis_Horizontal;

		Match(m_Token);
		Match(LPAREN);
		if (std::string("expr") == m_pScan->GetTokenStringQ(0))
		{
			Match(ID);
			Match(COLON);
			p->expr = expression();
		}
		Match(RPAREN);
	}

	return p;
}


// 		expr_expression ->		'expr' ':' expression
// 			size_expression ->		'size' ':' expression
// 			rank_expression ->		'rank' ':' expression
// 			base_expression ->		'base' ':' expression
// 			left_expression ->		'left' ':' expression
// 			right_expression ->	'right' ':' expression
// 			skip_expression ->		'skip' ':' expression
// 			head_expression ->	'head' ':' expression

// autoexpand rule
// 			autoexpand -> 	type '=' disp_format+
SAutoExp* parser::CParser::autoexpand() 
{
	SAutoExp *p = NULL;
	if (ASSIGN == m_Token)
	{
		Match(ASSIGN);
		p = new SAutoExp;
		p->disp_format = disp_format();
	}
	return p;
}

//			disp_format ->	text '<' expression [,format] '>'
SDisp_Format* parser::CParser::disp_format() 
{
	SDisp_Format *p = NULL;
	SExpression *text = expression();
	if (!text)
		return NULL;

	if (m_Token == LT)// <
	{
		p = new SDisp_Format;
		p->text = text;
		p->exp = expression();
		if (COMMA == m_Token)
			p->format = display_format();			
	}
	else
	{
		SAFE_DELETE(text);
	}

	return p;
}


// types -> type { | type }
SType_Stmts* parser::CParser::types()
{
	SType_Stmt *pt = type();
	if (!pt) return NULL;

	SType_Stmts *p = new SType_Stmts;
	p->type = pt;
	p->next = NULL;
	if (LOGIC_OR == m_Token)
	{
		Match(LOGIC_OR);
		p->next = types();	
	}
	return p;
}


// type -> type_id
//				| type_id '<' template_args  '>'

// template_args -> type {, type}
//							| *

// type_id -> id
//					| id::id
SType_Stmt* parser::CParser::type() 
{
	SType_Stmt *p = NULL;
	if (ID == m_Token)
	{
		p = new SType_Stmt;
		p->templateArgs = NULL;
		p->id = m_pScan->GetTokenStringQ(0);
		Match(ID);

		while (SCOPE == m_Token || ID == m_Token) // ex unsigned short
		{
			if (ID == m_Token)
			{
				p->id += " ";
				p->id += m_pScan->GetTokenStringQ(0);

				Match(ID);
			}
			else
			{
				Match(SCOPE);
				if (ID != m_Token)
					break;
				p->id += "::";
				p->id += m_pScan->GetTokenStringQ(0);
				Match(ID);
			}
		}

		// template
		if (LT == m_Token)
		{
			Match(LT);
			p->templateArgs = template_args();
			Match(RT);
		}
	}
	else if (TIMES == m_Token)
	{
		p = new SType_Stmt;
		p->templateArgs = NULL;
		p->id = "*";
		Match(TIMES);
	}	
	return p;
}

// template_args -> type {, type}
//							| *
SType_TemplateArgs* parser::CParser::template_args()
{
	SType_TemplateArgs *p = new SType_TemplateArgs;
	p->next = NULL;

	p->type = type();
	if (COMMA == m_Token)
	{
		Match(COMMA);
		p->next = template_args();
	}
	return p;
}


// type_sub -> id '<' type_sub [,type_sub] '>'
//			| id::id
//			| id
//			| *
std::string parser::CParser::type_sub()
{
	std::string str = "";

	if (ID == m_Token)
	{
		Tokentype nextTok = m_pScan->GetTokenQ(1);
		if (LT == nextTok)
		{
			str += id();
			str += "<";
			Match(LT);
			str += type_sub();
			str += ">";
			Match(RT);
		}
		else if (SCOPE == nextTok)
		{
			str += id();
			str += "::";
			Match(SCOPE);
			str += type_sub();
		}
		else
		{
			str += id();
		}
	}
	return str;
}


// 		format ->	d | i | u | o | x | X | l | h | f | e |g |c | s | su
// 			;

// 		expression ->	term [condition_op expression]
//								|  term [ '[' expression ']' [ ->|. expression ]
// 		;
SExpression* parser::CParser::expression() 
{
	SExpression *t = term();
	if (!t) return NULL;

	if (IsConditionOp(m_Token))
	{
		SExpression *p = NewExpression(CondExprK);
		p->op = m_Token;
		Match(p->op);
		p->lhs = t;
		p->rhs = expression();
		t = p;
	}
	else if (LBRACKET == m_Token)
	{
		SExpression *p = NewExpression(IndirectK);
		p->op = m_Token;
		Match(p->op);
		p->lhs = expression();
		Match(RBRACKET);

		if ((m_Token == ARROW) ||
			(m_Token == DOT))
		{
			p->rhs = variable();
		}

		// right handle 에 붙인다.
		SExpression *c = t;
		while (c->rhs)
			c = c->rhs;
		c->rhs = p;
	}
	return t;
}

bool parser::CParser::IsConditionOp(Tokentype tok)
{
	switch (tok)
	{
	case LT:/* < */
	case RT:/* > */
	case LTEQ:/* <= */
	case RTEQ:/* >= */
	case NEQ:/* != */
	case EQ: /* == */
	case OR:/* || */
	case AND:/* && */
		return true;
	}
	return false;
}


// 		term ->		mul_term [add_op term]
// 		;
SExpression* parser::CParser::term() 
{
	SExpression *mul = mul_term();
	if (IsAddOp(m_Token))
	{
		SExpression *p = NewExpression(AddTermK);
		p->op = m_Token;
		Match(p->op);
		p->lhs = mul;
		p->rhs = term();
		mul = p;
	}
	return mul;
}


bool parser::CParser::IsAddOp(Tokentype tok)
{
	switch (tok)
	{
	case PLUS:
	case MINUS:
		return true;
	}
	return false;
}


// 		mul_term ->	factor [mul_op mul_term]
// 		| factor
// 			;
SExpression* parser::CParser::mul_term() 
{
	SExpression *fac = factor();
	if (IsMultiOp(m_Token))
	{
		SExpression *p = NewExpression(MulTermK);
		p->op = m_Token;
		Match(p->op);
		p->lhs = fac;
		p->rhs = mul_term();
		fac = p;
	}
	return fac;
}

bool parser::CParser::IsMultiOp(Tokentype tok)
{
	switch (tok)
	{
	case TIMES:
	case DIV:
		return true;
	}
	return false;
}


// 		factor ->		prefix_exp
// 			;

// 		prefix_exp ->	[prefix_op] primary_expression
// 			;

SExpression* parser::CParser::factor() 
{
	SExpression *p = NULL;
	Tokentype prefix = NONE;
	if (IsPrefixOp(m_Token))
	{
		prefix = m_Token;
		Match(prefix);
	}

	p = primary_expression();
	if (!p) return NULL;

	p->prefix_op = prefix;
	return p;
}


// prefix_op ->	+ | - | * | & | ! | ++ | --
bool parser::CParser::IsPrefixOp(Tokentype tok)
{
	switch (tok)
	{
	case PLUS:
	case MINUS:
	case TIMES:
	case REF:
	case NEG:
	case INC:
	case DEC:
		return true;
	}
	return false;
}


// 		primary_expression -> variable [ '[' expression ']' ]
// 			| number constant
//				| string constant
// 			| '(' expression ')'
// 			;

SExpression* parser::CParser::primary_expression() 
{
	SExpression*p=NULL;
	switch (m_Token)
	{
	case LPAREN:
		Match(LPAREN);
		p = expression();
		Match(RPAREN);	
		break;

	case NUM:
	case FNUM:
		p = NewExpression(NumberK);
		p->num = (float)atof(m_pScan->GetTokenStringQ(0));
		Match(m_Token);
		break;

	case STRING:
		p = NewExpression(StringK);
		p->str = m_pScan->GetTokenStringQ(0);
		Match(STRING);
		break;

	case LBRACKET:
		{
			Match(LBRACKET);
			p = expression();
			if (COMMA == m_Token) // display format
			{
				SExpression *pnew = NewExpression(DispFormatK);
				pnew->dispFormat = display_format();
				pnew->rhs = p;
				p = pnew;
			}
			Match(RBRACKET);
		}
		break;

	case ID:
		p = variable();
		break;
	}
	return p;
}

// 		variable -> 	typeName [ '.' typeName ]
// 		| typeName ['->' typeName ]
// 		;
SExpression* parser::CParser::variable() 
{
	SExpression*p = NewExpression(VariableK);
	p->str = id();

	if (DOT  == m_Token 
		|| ARROW == m_Token
		//|| LBRACKET == m_Token
		)
	{
		const Tokentype oldTok = m_Token;
		p->varOpkind = m_Token;
		Match(p->varOpkind);
		p->rhs = variable();
		//if (LBRACKET == oldTok)
		//	Match(RBRACKET);
	}
	return p;
}

std::string parser::CParser::number()
{
	std::string str = "";
	str = m_pScan->GetTokenStringQ(0);
	Match(NUM);
	return str;
}

int parser::CParser::num()
{
	int n = atoi(m_pScan->GetTokenStringQ(0));
	Match(NUM);
	return n;
}

std::string parser::CParser::id()
{
	std::string str = m_pScan->GetTokenStringQ(0);
	Match( ID );
	return str;
}

BOOL parser::CParser::Match( Tokentype t )
{
	if( m_Token == t )
	{
		m_Token = m_pScan->GetToken();
	}
	else
	{
		SyntaxError( "unexpected token -> " );

		memmonitor::GetLogWindow()->PrintText( 
			PrintToken( m_Token, m_pScan->GetTokenStringQ(0) ) );

		//((CMainFrame*)::AfxGetMainWnd())->GetOutputWnd().AddString( 
		//	common::str2wstr(
		//		PrintToken( m_Token, m_pScan->GetTokenStringQ(0) )).c_str() );

		printf( "\n" );
	}
	return TRUE;
}

void parser::CParser::SyntaxError( char *szMsg, ... )
{
	m_IsErrorOccur = TRUE;
	char buf[ 256];
	va_list marker;
	va_start(marker, szMsg);
	vsprintf_s(buf, sizeof(buf), szMsg, marker);
	va_end(marker);

	std::stringstream ss;
	ss << "Syntax error at line " << m_fileName << " " << m_pScan->GetLineNo() <<  ": " << buf ;

	memmonitor::GetLogWindow()->PrintText( ">>>" );
	memmonitor::GetLogWindow()->PrintText( ss.str() );
	//((CMainFrame*)::AfxGetMainWnd())->GetOutputWnd().AddString( L">>>" );
 //	((CMainFrame*)::AfxGetMainWnd())->GetOutputWnd().AddString( 
 //		common::str2wstr(ss.str()).c_str() );
}

