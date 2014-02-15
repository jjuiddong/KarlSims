/**
 @filename GenotypeParser.h
 
  GenoType parser
*/
#pragma once


#include "GenotypeScanner.h"

namespace evc { namespace genotype_parser {
	using std::string;
	using std::stringstream;


	class CGenotypeScanner;
	class CGenotypeParser
	{
	public:
		CGenotypeParser();
		virtual ~CGenotypeParser();
		SExpr* Parse( const string &fileName, bool isTrace=false);
		bool Parse( BYTE *pFileMem, int fileSize , bool isTrace=false);
		bool IsError() { return m_IsErrorOccur; }
		void Clear();


	protected:
		//////////////////////////////////////////////////////////////////////////////////////
		// Rule~~
		//start -> expression_list;
		SExprList* start();

		//expression -> id ( id, vec3, material, [randshape,] connection-list )
		//	| id;
		SExpr* expression();

		//expression-list -> [ expression {, expression } ];
		SExprList* expression_list();

		// connection -> connection( id, quat, quat, vec3, limit, velocity, [randpos,] [randorient,] expression )
		SConnection* connection();

		// connection-list -> [ connection{, connection} ];
		SConnectionList* connection_list();

		//vec3 -> vec3( num, num, num ) ;
		SVec3 vec3();

		//quat -> quat(num, vec3);
		SQuat quat();

		// limit -> limit(num, num, num)
		SVec3 limit();

		// mass -> mass(num)
		float mass();

		// material -> material( material_arg )
		SVec3 material();

		// material_arg -> id | rgb
		SVec3 rgbValue();

		// velocity -> velocity(num)
		SVec3 velocity();

		// period -> period(num)
		float period();

		// randshape -> randshape(num, num, num)
		SVec3 randshape();

		// randfield -> randshape | randpos | randorient
		SVec3 randField();

		// randpos -> randpos(num, num, num)
		SVec3 randpos();

		// randorient -> randorient(num, num, num)
		SVec3 randorient();

		// terminalOnly -> terminalOnly
		bool terminalonly();


		string number();
		int num();
		string id();
		//////////////////////////////////////////////////////////////////////////////////////

		bool Match( Tokentype t );
		void SyntaxError( char *msg, ... );
		void Build( SExpr *pmainExpr );
		void RemoveNotRefExpression();


	private:
		CGenotypeScanner *m_pScan;
		string m_fileName;
		Tokentype m_Token;
		bool m_IsTrace;
		bool m_IsErrorOccur;
		map<string,SExpr*> m_SymTable;
		set<string> m_RefCount;
	};

}}
