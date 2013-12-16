
#include "stdafx.h"
#include "VisualizerDefine.h"

using namespace visualizer;
using namespace visualizer::parser;


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
std::string parser::PrintToken( Tokentype token, char *szTokenString )
{
	std::stringstream ss;
	switch( token )
	{
	case ID:
	case STRING:	ss << "string = " << szTokenString; break;
	case ASSIGN:	ss << "="; break;
	case TIMES:		ss <<"*"; break;
	case LPAREN:	ss << "("; break;
	case RPAREN:	ss << ")"; break;
	case LBRACE:	ss << "{"; break;
	case RBRACE:	ss << "}"; break;
	case COMMA:	ss << ","; break;
	case NUM:		ss << "number"; break;
	case OR:		ss << "||"; break;
	case AND:		ss << "&&"; break;

	default: 		ss << "UnKnown token " << token << szTokenString; break;
	}
	return ss.str();
}

SExpression* parser::NewExpression( Kind kind )
{
	SExpression *p = new SExpression;
	p->kind = kind;
	p->rhs = NULL;
	return p;
}

SStatements* parser::NewStatement( StatementKind kind )
{
	SStatements *p = new SStatements;
	p->kind = kind;
	p->next = NULL;
	return p;
}

void RemoveStatements( SStatements *p);
void RemoveExpression( SExpression*p );

void RemoveElif_Stmt( SElif_Stmt *p)
{
	if (!p) return;
	RemoveExpression(p->cond);
	RemoveStatements(p->stmts);
	RemoveElif_Stmt(p->next);
	delete p;
}

void RemoveGVis_Stmt( SGVis_Stmt *p)
{
	if (!p) return;
	RemoveExpression(p->expr);
	delete p;
}

void RemoveExpression( SExpression*p )
{
	if (!p) return;

	switch (p->kind)
	{
	case CondExprK:
	case AddTermK:
	case MulTermK:
	case IndirectK:
		RemoveExpression( p->lhs );
		break;
	
	case DispFormatK:
	case VariableK:
	case NumberK:
	case StringK:
		break;
	}

	RemoveExpression( p->rhs );
	delete p;
}

void parser::RemoveType_Stmt( SType_Stmt *p)
{
	if (!p) return;
	if (p->templateArgs)
	{
		SType_TemplateArgs *tp = p->templateArgs;
		while (tp)
		{
			RemoveType_Stmt(tp->type);
			SType_TemplateArgs *next = tp->next;
			delete tp;
			tp = next;
		}
	}
	delete p;
}

void parser::RemoveType_Stmts( SType_Stmts *p )
{
	if (!p) return;
	RemoveType_Stmt(p->type);
	RemoveType_Stmts(p->next);
	delete p;
}

void RemoveStatements( SStatements *p)
{
	if (!p) return;

	switch(p->kind)
	{
	case Stmt_StringLiteral: break;
	case Stmt_Expression: 
		RemoveExpression(p->exp); 
		break;

	case Stmt_SimpleExpression: 
		RemoveExpression(p->simple_exp->expr);
		RemoveExpression(p->simple_exp->text);
		delete p->simple_exp;		
		break;

	case Stmt_If: 
		RemoveExpression( p->if_stmt->cond );
		RemoveElif_Stmt(p->if_stmt->elif_stmt);
		RemoveStatements( p->if_stmt->else_stmts );
		RemoveStatements( p->if_stmt->stmts );
		delete p->if_stmt;
		break;

	case Stmt_Bracket_Iterator:
		RemoveExpression( p->itor_stmt->stmts->base );
		RemoveExpression( p->itor_stmt->stmts->expr );
		RemoveExpression( p->itor_stmt->stmts->size );
		RemoveExpression( p->itor_stmt->stmts->rank );
		RemoveExpression( p->itor_stmt->stmts->base );
		RemoveExpression( p->itor_stmt->stmts->next );
		RemoveExpression( p->itor_stmt->stmts->left );
		RemoveExpression( p->itor_stmt->stmts->right );
		RemoveExpression( p->itor_stmt->stmts->skip );
		RemoveExpression( p->itor_stmt->stmts->head );
		delete p->itor_stmt->stmts;
		RemoveStatements(p->itor_stmt->disp_stmt );
		delete p->itor_stmt;
		break;

	case Stmt_GVis:
		RemoveGVis_Stmt(p->gvis_stmt);
		break;
	}	

	RemoveStatements(p->next);
	delete p;
}

void RemoveDisp_Format( SDisp_Format*p)
{
	if (!p) return;
	RemoveExpression(p->exp);
	RemoveExpression(p->text);
	delete p;
}

void RemoveVisualizer( SVisualizer *vis )
{
	if (!vis) return;
	RemoveType_Stmts(vis->matchType);
	RemoveStatements(vis->preview);
	RemoveStatements(vis->stringview);
	RemoveStatements(vis->children);
	RemoveStatements(vis->graph);
	delete vis;
}

void RemoveAutoExp( SAutoExp *autoexp )
{
	if (!autoexp) return;
	RemoveDisp_Format(autoexp->disp_format);
	RemoveType_Stmts(autoexp->match_type);
	delete autoexp;
}

void parser::RemoveVisualizerScript(SVisualizerScript*p)
{
	if (!p) return;
	if (p->kind == VisualizerScript_Visualizer)
		RemoveVisualizer(p->vis);
	else
		RemoveAutoExp(p->autoexp);
	RemoveVisualizerScript(p->next);	
	delete p;
}
