//------------------------------------------------------------------------
// Name:    ProtocolParser.h
// Author:  jjuiddong
// Date:    12/26/2012
// 
// 프로토콜 파서
//------------------------------------------------------------------------
#pragma once

#include "VisualizerScanner.h"

namespace visualizer { namespace parser 
{
	class CScanner;
	class CParser
	{
	public:
		CParser();
		virtual ~CParser();

	protected:
		CScanner *m_pScan;
		std::string m_fileName;
		Tokentype m_Token;
		BOOL m_IsTrace;
		BOOL m_IsErrorOccur;

	public:
		SVisualizerScript* Parse( const std::string &fileName, BOOL bTrace=FALSE );
		BOOL Parse( BYTE *pFileMem, int nFileSize , BOOL bTrace=FALSE );
		BOOL IsError() { return m_IsErrorOccur; }
		void Clear();

	private:
		BOOL Match( Tokentype t );
		void SyntaxError( char *szMsg, ... );

		SVisualizerScript* visualizerscript();
		SVisualizer* visualizer();
		SStatements* preview();
		SStatements* stringview();
		SStatements* children();
		SStatements* graph();
		SStatements* statements();
		SSimpleExp* simple_exp();
		SStatements* viscommand();
		SIf_Stmt* if_stmt();
		SElif_Stmt* elif_stmt();
		SVisBracketIterator_Stmt* visbracketiterator_stmt();
		SBracket_Inner_Stmts* bracket_inner_stmts();
		SGVis_Stmt *gvis_stmts();
		SAutoExp* autoexpand();
		SDisp_Format* disp_format();
		Display_Format display_format();
		SType_Stmts* types();
		SType_Stmt* type();
		SType_TemplateArgs* template_args();

		SExpression* text();
		SExpression* expression();
		SExpression* term();
		SExpression* mul_term();
		SExpression* factor();
		SExpression* primary_expression();
		SExpression* variable();

		std::string number();
		int num();
		std::string id();
		std::string type_sub();

		bool IsVisCommand(Tokentype tok);
		bool IsConditionOp(Tokentype tok);
		bool IsAddOp(Tokentype tok);
		bool IsMultiOp(Tokentype tok);
		bool IsPrefixOp(Tokentype tok);

	};
}}