/**
 @filename GenotypeScanner.h
 
 2013-12-05

 Genotype Script Scanner
*/
#pragma once


namespace evc { namespace genotype_parser {

	enum Tokentype
	{
		_ERROR, NONE, ENDFILE, ID, NUM, FNUM, STRING, ASSIGN, LPAREN, RPAREN, 
		LBRACE/*{*/, RBRACE/*}*/, 
		LBRACKET/* [ */, RBRACKET/* ] */, 
		DOT, COMMA, COLON, SEMICOLON,
		PLUS, MINUS, TIMES, DIV, REMAINDER, REF, ARROW, INC, DEC,
		LT/* < */, RT/* > */, LTEQ/* <= */, RTEQ/* >= */, NEQ/* != */, EQ/* == */, LOGIC_OR/* | */, OR/* || */, AND/* && */, NEG/* ! */, SCOPE/*::*/,

		// special
		IF, ELIF, ELSE, ARRAY, LIST,  TREE, CHILDREN, PREVIEW, STRINGVIEW,
		SHARP_BRACKET, 
	};


	struct SVec3 
	{ 
		float x, y, z; 
	};

	struct SQuat
	{ 
		float angle; // radian
		SVec3 dir;
	};

	struct SExprList;
	struct SExpr
	{
		bool isIdOnly;
		string id;
		string shape;
		SVec3 dimension;
		string joint;
		SQuat orient;
		SVec3 pos;
		SExprList *connection;
		int refCount;
	};

	struct SExprList
	{
		SExpr *expr;
		SExprList *next;
	};

	void RemoveExpression(SExpr *expr);
	void RemoveExpressoin_OnlyExpr(SExpr *expr);


	class CGenotypeScanner
	{
	public:
		CGenotypeScanner();
		virtual ~CGenotypeScanner();

		bool LoadFile( const string &fileName, bool bTrace=false );
		bool LoadPackageFile( BYTE *pFileMem, int nFileSize );
		Tokentype GetToken();
		Tokentype GetTokenQ( int nIdx );
		const string& GetTokenStringQ( int nIdx );
		//char* CopyTokenStringQ( int nIdx );
		int GetLineNo() { return m_lineNo; }
		bool IsEnd();
		void Clear();


	protected:
		void Init();
		char GetNextChar();
		void UngetNextChar();
		Tokentype _GetToken( string &pToken );
		bool GetString(char *receiveBuffer, int maxBufferLength);
		void SetEndOfFile();


	private:
		enum { MAX_QSIZE=8, MAX_BUFF=256, MAX_TOKENLEN=256 };
		enum StateType { START, INASSIGN, INCOMMENT, INMULTI_COMMENT, OUTMULTI_COMMENT, 
			INNUM, INID, INSTR, INDIV, INEQ, INNEQ, INOR, INAND, INLTEQ, INRTEQ, INARROW, INSCOPE, INSHARP, 
			INPLUS, INMINUS,
			DONE };

		typedef struct _tagSTokDat
		{
			string str;
			Tokentype tok;
		} STokDat;

		std::string m_Source;
		int m_pCurrentMemPoint;
		int m_memSize;

		int m_lineNo;
		int m_linePos;
		int m_bufSize;
		char m_buf[ MAX_BUFF];
		std::deque<STokDat> m_TokQ;
		bool m_IsTrace;
	};

}}

