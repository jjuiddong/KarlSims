//------------------------------------------------------------------------
// Name:    ScriptDefine.h
// Author:  jjuiddong
// Date:    1/3/2013
// 
// 프로토콜 파싱에 관련된 구문트리나, 상수값들을 모아놓았다.
//
// 2013-01-28
//		Inc, Dec 추가
//
//------------------------------------------------------------------------
#pragma once

#include <string>

namespace visualizer { namespace parser {
	enum
	{
		MAX_STRING = 256,
		MAX_ARGUMENT = 10,
	};

	enum Tokentype
	{
		_ERROR, NONE, ENDFILE, ID, NUM, FNUM, STRING, ASSIGN, LPAREN, RPAREN, 
		LBRACE/*{*/, RBRACE/*}*/, 
		LBRACKET/* [ */, RBRACKET/* ] */, 
		DOT, COMMA, COLON, SEMICOLON,
		PLUS, MINUS, TIMES, DIV, REMAINDER, REF, ARROW, INC, DEC,
		LT/* < */, RT/* > */, LTEQ/* <= */, RTEQ/* >= */, NEQ/* != */, EQ/* == */, LOGIC_OR/* | */, OR/* || */, AND/* && */, NEG/* ! */, SCOPE/*::*/,

		// special
		IF, ELIF, ELSE, ARRAY, LIST,  TREE, VERTICAL, HORIZONTAL, CHILDREN, PREVIEW, STRINGVIEW, GRAPH,
		SHARP_BRACKET, 

	};

	enum NodeKind { Stmt, Exp };
	enum Kind
	{
		// expression
		CondExprK, AddTermK, MulTermK, VariableK, IndirectK, NumberK, StringK, DispFormatK,

		// statement
	};

	enum { MAXCHILD=8, };

	enum eCONDITION_OP { OP_NONE, OP_AND, OP_OR, OP_NEG, OP_LT, OP_RT, OP_LTEQ, OP_RTEQ, OP_EQ, OP_NEQ };

	struct SStatements;


	enum Display_Format
	{
		Disp_D,
		Disp_U,
		Disp_O,
		Disp_X,
		Disp_L,
		Disp_F,
		Disp_E,
		Disp_G,
		Disp_C,
		Disp_S,
		Disp_SU,
		Disp_Auto,
	};

	struct SExpression
	{
		Kind kind;

		Tokentype prefix_op;
		Tokentype varOpkind; //".", "->"
		Tokentype op;

//		union {
		float num;
		std::string str;
		SExpression *lhs;
		Display_Format dispFormat;
//		}

		SExpression *rhs;
	};

	struct SType_Stmt;
	struct SType_TemplateArgs
	{
		SType_Stmt *type;
		SType_TemplateArgs *next;
	};

	struct SType_Stmt
	{
		std::string id;
		SType_TemplateArgs *templateArgs;
	};

	struct SType_Stmts
	{
		SType_Stmt *type;
		SType_Stmts *next;
	};

	struct SDisp_Format
	{
		SExpression *text;
		SExpression *exp;
		Display_Format format;
	};

	struct SAutoExp
	{
		SType_Stmts *match_type;
		SDisp_Format *disp_format;
	};

	struct SBracket_Inner_Stmts
	{
		SExpression *expr;	/// display expression
		SExpression *size;
		SExpression *rank;
		SExpression *base;
		SExpression *next ;
		SExpression *left;
		SExpression *right;
		SExpression *skip;
		SExpression *head;
	};

	struct SElif_Stmt
	{
		SExpression *cond;
		SStatements *stmts;
		SElif_Stmt *next;
	};

	struct SIf_Stmt
	{
		SExpression *cond;
		SStatements *stmts;
		SElif_Stmt *elif_stmt;
		SStatements *else_stmts;
	};

	enum VisBracketIteratorKind
	{
		Iter_Array,
		Iter_List,
		Iter_Tree
	};

	struct SVisBracketIterator_Stmt
	{
		VisBracketIteratorKind kind;
		SBracket_Inner_Stmts *stmts;
		SStatements *disp_stmt;			/// bracket outer statement
	};

	enum GVisKind
	{
		GVis_Vertivcal,
		GVis_Horizontal,
	};
	
	struct SGVis_Stmt
	{
		GVisKind kind;
		SExpression *expr;
	};

	struct SSimpleExp
	{
		SExpression *text;
		SExpression *expr;
		Display_Format format;
	};

	enum StatementKind
	{
		Stmt_StringLiteral,
		Stmt_Expression,
		Stmt_SimpleExpression,
		Stmt_If,
		Stmt_Bracket_Iterator,
		Stmt_GVis,
	};

	struct SStatements
	{
		StatementKind kind;
		union {
			SExpression *exp;
			SSimpleExp *simple_exp;
			SIf_Stmt *if_stmt;
			SVisBracketIterator_Stmt *itor_stmt;
			SGVis_Stmt *gvis_stmt;
		};
		SStatements *next;
	};

	struct SVisualizer
	{
		SType_Stmts *matchType;
		SStatements *preview;
		SStatements *stringview;
		SStatements *children;
		SStatements *graph;
	};

	enum VisualizerScriptKind
	{
		VisualizerScript_Visualizer,
		VisualizerScript_AutoExp,
	};

	struct SVisualizerScript
	{
		VisualizerScriptKind kind;
		union {
			SVisualizer *vis;
			SAutoExp *autoexp;
		};
		SVisualizerScript *next;
	};

	SExpression* NewExpression( Kind kind );
	SStatements* NewStatement( StatementKind kind );

	// Functions
	std::string PrintToken( Tokentype token, char *szTokenString );
	void RemoveVisualizerScript(SVisualizerScript*p);
	void RemoveType_Stmt( SType_Stmt *p);
	void RemoveType_Stmts( SType_Stmts *p );

}}
