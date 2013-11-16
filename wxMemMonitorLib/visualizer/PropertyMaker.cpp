
#include "stdafx.h"
#include "PropertyMaker.h"
#include "VisualizerParser.h"
#include "../Dia/DiaWrapper.h"
#include "DefaultPropertyMaker.h"
#include <atlcomcli.h>
#include "../Control/Global.h"
#include "../ui/LogWindow.h"
#include "../ui/PropertyWindow.h"

namespace visualizer
{
	using namespace parser;

	struct SMakerData
	{
		// $e
		std::string origSymbolName;
		SMemInfo origMem;
		int index;	/// $i
		int rank; /// $r
		int count; ///
		CPropertyWindow *propertyWindow;
		wxPGProperty *parentProperty;
		SSymbolInfo symbol; /// current symbol
		int depth;

		SMakerData() {}
		SMakerData(const SMakerData &rhs) {
			if (this != &rhs) {
				origSymbolName = rhs.origSymbolName;
				origMem = rhs.origMem;
				index = rhs.index;
				rank = rhs.rank;
				count = rhs.count;
				depth = rhs.depth;

				propertyWindow = rhs.propertyWindow;
				parentProperty = rhs.parentProperty;
				symbol = rhs.symbol;
				symbol.isNotRelease = true; /// �ѹ��� release ��Ű������ �ӽù��� �ڵ��.
			}
		}
	};


	// search visualizer
	SVisualizerScript* SearchVisualizerScript(SType_Stmt *psymT);
	
	SVisualizerScript* FindVisualizer( const std::string &typeName );


	// parse visualizer
	void MakeProperty_Visualizer( SVisualizer *pvis, const SMakerData &makerData);

	void MakeProperty_AutoExpand( SAutoExp *pautoexp, const SMakerData &makerData );

	void MakePropertyStatements( SStatements *pstmt, const SMakerData &makerData, const std::string &title="" );

	void MakePropertySimpleExpression( SSimpleExp *pexp, const SMakerData &makerData );

	void MakePropertyExpression( SExpression *pexp, const SMakerData &makerData, const std::string &title="" );

	void MakePropertyIfStmt( SIf_Stmt *pif_stmt, const SMakerData &makerData );

	void MakePropertyIteratorStmt( SVisBracketIterator_Stmt *pItor_stmt, 
		const SMakerData &makerData );

	void MakePropertyIteratorStmt_List( SVisBracketIterator_Stmt *pItor_stmt, 
		const SMakerData &makerData );

	void MakePropertyIteratorStmt_Array( SVisBracketIterator_Stmt *pItor_stmt, 
		const SMakerData &makerData );

	void MakePropertyIteratorStmt_Tree( SVisBracketIterator_Stmt *pItor_stmt, 
		const SMakerData &makerData );

	bool MakePropertyElifStmt( SElif_Stmt *pelif_stmt, const SMakerData &makerData );


	// evaluate
	_variant_t Eval_Expression( SExpression *pexp, const SMakerData &makerData,
		OUT SSymbolInfo *pOut=NULL);

	_variant_t Eval_Variable( SExpression *pexp, const SMakerData &makerData, 
		OUT SSymbolInfo *pOut=NULL );


	// Find
	bool Find_Expression( SExpression *pexp, 
		IN const SMakerData &makerData, OUT SSymbolInfo *pOut);

	bool Find_Variable( SExpression *pexp, 
		IN const SMakerData &makerData, OUT SSymbolInfo *pOut );

	bool Find_Indirect( SExpression *pexp, 
		IN const SMakerData &makerData, OUT SSymbolInfo *pOut );

	bool Find_Variable_FromId( const std::string &varId, 
		IN const SMakerData &makerData, OUT SSymbolInfo *pOut );

	void Disp_Expression( SExpression *pexp );
	char GetAsciiFromTokentype(Tokentype tok);
	void CheckError( bool condition, const SMakerData &makerData, const std::string &msg="error" );


	// Visualizer Search
	bool CompareTypes( SType_Stmt *psymT, SType_Stmts *pvisT);
	bool CompareType( SType_Stmt *psymT, SType_Stmt *pvisT,OUT bool &isAstrisk );
	bool CompareTemplateTypes( SType_TemplateArgs *psymT, 
		SType_TemplateArgs *pvisT );
	SType_Stmt* ParseType( INOUT std::string &typeName );
	SType_TemplateArgs* ParseTemplateType( INOUT std::string &typeName );


	// Visualizer Script Parsing
	visualizer::parser::SVisualizerScript *g_pVisScr = NULL;

	/// dia basetype �� �ӽ� ����
	int g_BaseTypeInt = 0;
}


using namespace dia;
using namespace std;
using namespace visualizer;


// exception class
class VisualizerScriptError
{
public:
	VisualizerScriptError(const string &msg) : m_Msg(msg) {}
	string m_Msg;
};


/**
 @brief ���� ������ ���� ptr�� ���� ����Ű�� �ּ� ���� �����Ѵ�. return  *(DWORD*)(void*)ptr
 */
DWORD visualizer::Point2PointValue(DWORD ptr)
{
	DWORD value = 0;
	if (ptr)
		value = *(DWORD*)(void*)ptr;
	return value;
}


//------------------------------------------------------------------------
// Visualizer ��ũ��Ʈ�� �д´�.
//------------------------------------------------------------------------
bool visualizer::OpenVisualizerScript( const std::string &fileName )
{
	RemoveVisualizerScript(g_pVisScr);
	CParser parser;
	g_pVisScr = parser.Parse(fileName);
	if (!g_pVisScr)
		return false;
	return true;
}

void visualizer::Release()
{
	RemoveVisualizerScript(g_pVisScr);

}


//------------------------------------------------------------------------
// Property ����
// symbolName : �����޸𸮿� ����� �ɺ��̸�
//------------------------------------------------------------------------
bool	visualizer::MakeVisualizerProperty( CPropertyWindow *pPropertiesWnd, 
											   wxPGProperty *pParentProp, 
											   const SMemInfo &memInfo, const string &symbolName, const int depth )
{
	const std::string str = ParseObjectName(symbolName);
	SVisualizerScript *pVisScript = FindVisualizer(str);
	if (pVisScript)
	{
		IDiaSymbol *pSymbol = dia::FindType(str);
		RETV(!pSymbol, false);

		SMakerData makerData;
		makerData.origSymbolName = str;
		makerData.origMem = memInfo;
		makerData.propertyWindow = pPropertiesWnd;
		makerData.parentProperty = pParentProp;
		makerData.symbol.pSym = pSymbol;
		makerData.symbol.mem = SMemInfo(symbolName.c_str(), memInfo.ptr,0);
		makerData.depth = depth;

		try
		{
			if (VisualizerScript_Visualizer == pVisScript->kind)
				MakeProperty_Visualizer(pVisScript->vis, makerData );
			else 
				MakeProperty_AutoExpand(pVisScript->autoexp, makerData);
		}
		catch (VisualizerScriptError &e)
		{
			GetLogWindow()->PrintText( e.m_Msg );
			GetLogWindow()->PrintText( "\n" );
		}
		return true;
	}
	else
	{
		return false;
	}
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
bool	visualizer::MakeVisualizerProperty( CPropertyWindow *pPropertiesWnd, 
	wxPGProperty *pParentProp, const SSymbolInfo &symbol, const int depth )
{
	// Ÿ�Խɺ��� ��´�.
	dia::SymbolState symState;
	IDiaSymbol *pBaseType = dia::GetBaseTypeSymbol(symbol.pSym, 0, symState);
	RETV(!pBaseType, false);

	string typeName = dia::GetSymbolName(pBaseType);
	const bool result = MakeVisualizerProperty(pPropertiesWnd, pParentProp, 
		symbol.mem, typeName, depth);

	if (dia::NEW_SYMBOL == symState)
		pBaseType->Release();
	return result;
}


//------------------------------------------------------------------------
// visualizer ������Ƽ ����
//------------------------------------------------------------------------
void visualizer::MakeProperty_Visualizer( SVisualizer *pvis, const SMakerData &makerData )
{
	RET(!pvis);

	//todo : iterator ������ �ִ� ��� ��� �� ��Ĵ�� �ڽĵ��� �����ؾ� �Ѵ�.
	// list, map, vector �� ��츸 ���������� �ڽĵ��� ��������ؼ� ���������Ѵ�.
	if ((pvis->matchType->type->id == "std::list") || 
		(pvis->matchType->type->id == "std::map") ||
		(pvis->matchType->type->id == "std::vector"))
	{
		if (makerData.propertyWindow && makerData.parentProperty)
			makerData.propertyWindow->RemoveChildProperty(makerData.parentProperty);		
	}

	// �ϴ� preview�� ��� 	
	MakePropertyStatements( pvis->preview, makerData );
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void visualizer::MakeProperty_AutoExpand( SAutoExp *pautoexp, const SMakerData &makerData )
{
	RET(!pautoexp);

}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void visualizer::MakePropertyStatements( SStatements *pstmt, const SMakerData &makerData,
	const std::string &title ) // title
{
	RET(!pstmt);
	SStatements *node = pstmt;
	while (node)
	{
		switch (node->kind)
		{
		case Stmt_Expression: MakePropertyExpression(node->exp, makerData, title); break;
		case Stmt_SimpleExpression: MakePropertySimpleExpression(node->simple_exp, makerData); break;
		case Stmt_If: MakePropertyIfStmt(node->if_stmt, makerData); break;
		case Stmt_Bracket_Iterator: MakePropertyIteratorStmt(node->itor_stmt, makerData); break;
		}	
		node = node->next;
	}
}


//------------------------------------------------------------------------
// Simple_Expression ó��
//------------------------------------------------------------------------
void visualizer::MakePropertySimpleExpression( SSimpleExp *pexp, const SMakerData &makerData )
{
	RET(!pexp);	
	RET(!pexp->text);
	RET(!pexp->expr);

	SSymbolInfo findVar;
	const bool result = Find_Variable(pexp->expr, makerData, &findVar);
	CheckError(result, makerData, "Simple Expression Error!!, not found variable" );
	
	const bool isApplyVisualizer = (pexp->format != Disp_Auto);
	findVar.mem.name = pexp->text->str;
	MakeProperty_DefaultForm( makerData.propertyWindow, makerData.parentProperty, findVar, isApplyVisualizer, makerData.depth );
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
void visualizer::MakePropertyIfStmt( SIf_Stmt *pif_stmt, const SMakerData &makerData )
{
	RET(!pif_stmt);
	if (Eval_Expression(pif_stmt->cond, makerData) == CComVariant(true))
	{
		MakePropertyStatements( pif_stmt->stmts, makerData );
	}
	else
	{
		if (MakePropertyElifStmt( pif_stmt->elif_stmt, makerData ))
		{
			// �ƹ��� ����
		}
		else
		{
			MakePropertyStatements( pif_stmt->else_stmts, makerData );
		}
	}
}


/**
 @brief �ݺ� ��ɹ� ó��
 */
void visualizer::MakePropertyIteratorStmt( SVisBracketIterator_Stmt *pitor_stmt,
							  const SMakerData &makerData )
{
	RET(!pitor_stmt);
	RET(!pitor_stmt->stmts);
	
	switch (pitor_stmt->kind)
	{
	case Iter_Array: 
		MakePropertyIteratorStmt_Array(pitor_stmt, makerData);
		break;

	case Iter_List:
		MakePropertyIteratorStmt_List(pitor_stmt, makerData);
		break;

	case Iter_Tree: 
		MakePropertyIteratorStmt_Tree(pitor_stmt, makerData);
		break;
	}
}


/**
 @brief execute List Command
 */
void visualizer::MakePropertyIteratorStmt_List( SVisBracketIterator_Stmt *pitor_stmt,
	const SMakerData &makerData )
{
	CheckError( pitor_stmt->stmts->head && pitor_stmt->stmts->next, makerData, "#list head, next not setting" );
	CheckError( pitor_stmt->stmts->expr || pitor_stmt->disp_stmt , makerData, "#list expr, disp_stmt not setting" );

	// size ����, IDiaSymbol
	_variant_t size_v;
	if (pitor_stmt->stmts->size)
	{
		size_v = Eval_Expression(pitor_stmt->stmts->size, makerData);
		CheckError( size_v.vt != VT_EMPTY, makerData, "#list size expression error" );
	}
	int size = (int)size_v;
	if (!pitor_stmt->stmts->size)
		size = 100; // default

	_variant_t skip;
	if (pitor_stmt->stmts->skip)
	{
		skip = Eval_Expression(pitor_stmt->stmts->skip, makerData);
		CheckError( skip.vt != VT_EMPTY, makerData, "#list skip expression error" );
	}
	const DWORD skipPtr = (DWORD)skip;//Point2PointValue((DWORD)skip);

	const _variant_t node = Eval_Expression(pitor_stmt->stmts->head, makerData);
	CheckError( node.vt != VT_EMPTY, makerData, "#list head expression error" );
	DWORD nodePtr = Point2PointValue((DWORD)node);

	SSymbolInfo head;
	SMakerData eachMakerData = makerData;
	eachMakerData.count = 0;
	const bool result = Find_Variable(pitor_stmt->stmts->head, makerData, &head);
	CheckError( result, makerData, "#list head expression error, $e not found" );

	eachMakerData.symbol = head;
	head.isNotRelease = true;

	int count = 0;

	while (nodePtr 
		&& (!skipPtr || (skipPtr && nodePtr != skipPtr))
		&& (count < size))
	{
		std::stringstream ss;
		ss << "[" << count << "]";
		const std::string title = ss.str();
		
		MakePropertyExpression( pitor_stmt->stmts->expr, eachMakerData, title ); /// display
		MakePropertyStatements( pitor_stmt->disp_stmt, eachMakerData, title ); /// display
		
		SSymbolInfo next;
		const bool result = Find_Variable(pitor_stmt->stmts->next, eachMakerData, &next);
		CheckError( result, makerData, "#list next expression error, $e not found" );

		nodePtr = Point2PointValue((DWORD)next.mem.ptr);
		SAFE_RELEASE(eachMakerData.symbol.pSym);

		eachMakerData.symbol = next;
		next.isNotRelease = true;
		++count;
	}
}


/**
 @brief Execute vector command
 */
void visualizer::MakePropertyIteratorStmt_Array( SVisBracketIterator_Stmt *pitor_stmt, const SMakerData &makerData )
{
	CheckError( pitor_stmt->stmts->expr || pitor_stmt->disp_stmt , makerData, "#array expr, disp_stmt not setting" );

	// size ����, IDiaSymbol
	_variant_t size_v;
	if (pitor_stmt->stmts->size)
	{
		size_v = Eval_Expression(pitor_stmt->stmts->size, makerData);
		CheckError( size_v.vt != VT_EMPTY, makerData, "#array size expression error" );
	}
	int size = (int)size_v;
	if (!pitor_stmt->stmts->size)
		size = 100; // default

	SMakerData arrayMakeData = makerData;
	arrayMakeData.index = 0;
	arrayMakeData.symbol.isNotRelease = true;

	for(arrayMakeData.index=0; arrayMakeData.index < size; ++arrayMakeData.index)
	{
		std::stringstream ss;
		ss << "[" << arrayMakeData.index << "]";
		const std::string title = ss.str();

		MakePropertyExpression( pitor_stmt->stmts->expr, arrayMakeData, title ); /// display
		MakePropertyStatements( pitor_stmt->disp_stmt, arrayMakeData, title ); /// display
	}
}


/**
 @brief MakePropertyTree_Traverse
 */
void MakePropertyTree_Traverse(SVisBracketIterator_Stmt *pitor_stmt, SMakerData &makerData, 
	DWORD skipPtr, const int displaySize)
{
	DWORD nodePtr = Point2PointValue((DWORD)makerData.symbol.mem.ptr);
	if (!nodePtr)
		return;
	if (makerData.count >= displaySize)
		return;	
	if (skipPtr && (skipPtr == nodePtr))
		return;

	std::stringstream ss;
	ss << "[" << makerData.count << "]";
	const std::string title = ss.str();
	MakePropertyStatements( pitor_stmt->disp_stmt, makerData, title ); /// display

	++makerData.count;

	SSymbolInfo left;
	const bool leftResult = Find_Variable(pitor_stmt->stmts->left, makerData, &left);
	CheckError( leftResult, makerData, "#tree left expression error, $e not found" );

	SMakerData leftMakerData = makerData;
	leftMakerData.symbol = left;
	leftMakerData.symbol.isNotRelease = true;
	MakePropertyTree_Traverse(pitor_stmt, leftMakerData, skipPtr, displaySize);


	SSymbolInfo right;
	const bool rightResult = Find_Variable(pitor_stmt->stmts->right, makerData, &right);
	CheckError( rightResult, makerData, "#tree right expression error, $e not found" );

	SMakerData rightMakerData = makerData;
	rightMakerData.count = leftMakerData.count;
	rightMakerData.symbol = right;
	rightMakerData.symbol.isNotRelease = true;
	MakePropertyTree_Traverse(pitor_stmt, rightMakerData, skipPtr, displaySize);

	makerData.count = rightMakerData.count;
}


/**
 @brief MakePropertyIteratorStmt_Tree
 */
void visualizer::MakePropertyIteratorStmt_Tree( SVisBracketIterator_Stmt *pitor_stmt, 
	const SMakerData &makerData )
{
	CheckError( pitor_stmt->stmts->head != NULL, makerData, "#tree head, next not setting" );
	CheckError( pitor_stmt->stmts->left && pitor_stmt->stmts->right, makerData, "#tree left, right not setting" );
	CheckError( pitor_stmt->stmts->expr || pitor_stmt->disp_stmt , makerData, "#tree expr, disp_stmt not setting" );

	// size ����, IDiaSymbol
	_variant_t size_v;
	if (pitor_stmt->stmts->size)
	{
		size_v = Eval_Expression(pitor_stmt->stmts->size, makerData);
		CheckError( size_v.vt != VT_EMPTY, makerData, "#tree size expression error" );
	}
	int size = (int)size_v;
	if (!pitor_stmt->stmts->size)
		size = 100; // default

	_variant_t skip;
	if (pitor_stmt->stmts->skip)
	{
		skip = Eval_Expression(pitor_stmt->stmts->skip, makerData);
		CheckError( skip.vt != VT_EMPTY, makerData, "#tree skip expression error" );
	}
	const DWORD skipPtr = (DWORD)skip;// Point2PointValue((DWORD)skip);

	SSymbolInfo head;
	SMakerData eachMakerData = makerData;
	const bool result = Find_Variable(pitor_stmt->stmts->head, makerData, &head);
	CheckError( result, makerData, "#tree head expression error, $e not found" );

	eachMakerData.count = 0;
	eachMakerData.symbol = head;
	eachMakerData.symbol.isNotRelease = true;

	MakePropertyTree_Traverse(pitor_stmt, eachMakerData, skipPtr, size);
}


/**
 @brief process '[' ']' indirect operator
 */
bool visualizer::Find_Indirect( SExpression *pexp, IN const SMakerData &makerData, 
	OUT SSymbolInfo *pOut )
{
	RETV(!pexp, false);

	// process [ 'index' ]
	_variant_t val = Eval_Expression(pexp->lhs, makerData);
	if( (int)val < 0)
		return false;

	HRESULT hr;
	DWORD ptr = Point2PointValue((DWORD)makerData.symbol.mem.ptr);
	//int size = dia::GetSymbolLength(makerData.symbol.pSym);

	ULONGLONG length = dia::GetSymbolLength(makerData.symbol.pSym);
	DWORD next = ptr + ((int)val * length);

	// get indirect type
	// ÷�ڿ����� �ش� Ÿ���� Ÿ���� �ȴ�. ��������Ÿ�� ����Ÿ�ϰ�� �ι� ����ؾ��Ѵ�.
	IDiaSymbol *pSymType;
	hr = makerData.symbol.pSym->get_type(&pSymType);
	enum SymTagEnum symTag;
	hr = pSymType->get_symTag((DWORD*)&symTag);
	if (SymTagData == symTag)
	{
		IDiaSymbol *pRealSymType;
		hr = pSymType->get_type(&pRealSymType);
		pSymType->Release();
		pSymType = pRealSymType;
	}
	IDiaSymbol *pIndirectType;
	hr = pSymType->get_type(&pIndirectType);
	//

	pOut->pSym = pIndirectType;
	pOut->mem = SMemInfo(makerData.symbol.mem.name.c_str(), (void*)next, 0);	
	return true;
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
bool visualizer::MakePropertyElifStmt( SElif_Stmt *pelif_stmt, const SMakerData &makerData  )
{
	RETV(!pelif_stmt, false);
	if (Eval_Expression(pelif_stmt->cond, makerData) == CComVariant(true))
	{
		MakePropertyStatements( pelif_stmt->stmts, makerData );
		return true;
	}
	else
	{
		return MakePropertyElifStmt( pelif_stmt->next, makerData );
	}
}


/**
 @brief 
 */
void visualizer::MakePropertyExpression( SExpression *pexp, const SMakerData &makerData, 
	const std::string &title ) // title = ""
{
	RET(!pexp);

	switch (pexp->kind)
	{
	case CondExprK:
	case AddTermK:
	case MulTermK:
		break;

	case DispFormatK:
		MakePropertyExpression(pexp->rhs, makerData);
		break;

	case VariableK:
		{
			SSymbolInfo findSymbol;
			const bool result = Find_Variable(pexp, makerData, &findSymbol);
			CheckError(result, makerData, " variable expression error!!, undetected" );
			if (!title.empty())
				findSymbol.mem.name = title;
			MakeProperty_DefaultForm( makerData.propertyWindow, makerData.parentProperty, findSymbol, true, makerData.depth );
		}
		break;

	case IndirectK:
	case NumberK:
	case StringK:
		break;
	}
}


//------------------------------------------------------------------------
// typeName�� �ش�Ǵ� Visualizer ��ũ��Ʈ ������
//------------------------------------------------------------------------
SVisualizerScript* visualizer::FindVisualizer(const std::string &typeName )
{
	RETV(!g_pVisScr, NULL);

	std::string parseName = typeName;
	SType_Stmt *pTypeStmt = ParseType(parseName);

	SVisualizerScript *pscr = SearchVisualizerScript(pTypeStmt);
	RemoveType_Stmt(pTypeStmt);
	return pscr;
}


//------------------------------------------------------------------------
// Dia ���̺귯������ �Ѱ��� Ÿ�� ��Ʈ���� SType_Stmt
// ���·� �м��ؼ� �����Ѵ�.
//------------------------------------------------------------------------
SType_Stmt* visualizer::ParseType( INOUT string &typeName  )
{
	SType_Stmt *p = new SType_Stmt;

	// ���� ��ū���� index�� ã�´�.
	list<int> indices;
	indices.push_back(typeName.find('<'));
	indices.push_back(typeName.find(','));
	indices.push_back(typeName.find('>'));
	indices.sort();
	int idx = indices.front();
	while (idx == string::npos && !indices.empty())
	{
		if (indices.size() == 1)
			break;
		indices.pop_front();
		idx = indices.front();
	}

	if (string::npos == idx)
	{
		p->id = typeName;
		typeName.clear();
		p->templateArgs = NULL;
	}
	else  if ('<' == typeName[ idx])
	{
		p->id = typeName.substr(0, idx);
		typeName = typeName.substr(idx+1, typeName.size()-1); // next string
		p->templateArgs = ParseTemplateType(typeName);

		ASSERT_RETV(!typeName.empty(), p);
		const int rtIdx = typeName.find('>');
		ASSERT_RETV(rtIdx < 2, p); // ��ĭ ����� '>' �� ������, >> �� �������� 
		typeName = typeName.substr(rtIdx, typeName.size()-1); // next string
	}
	else  
	{
		p->id = typeName.substr(0, idx);
		typeName = typeName.substr(idx, typeName.size()-1); // next string
		p->templateArgs = NULL;
	}
	return p;
}


//------------------------------------------------------------------------
// < typename, typename .. > parsing
//------------------------------------------------------------------------
SType_TemplateArgs* visualizer::ParseTemplateType( INOUT string &typeName )
{
	SType_TemplateArgs *p = new SType_TemplateArgs;
	p->type = ParseType(typeName);
	p->next = NULL;

	if (typeName.empty())
		return p;

	if (typeName[0] == ',')
	{
		typeName = typeName.substr(1, typeName.size()-1); // next string
		p->next = ParseTemplateType(typeName);
	}
	return p;
}


//------------------------------------------------------------------------
// psymT Ÿ�԰� ���� visualizer script�� �����Ѵ�.
//------------------------------------------------------------------------
SVisualizerScript* visualizer::SearchVisualizerScript(SType_Stmt *psymT)
{
	RETV(!g_pVisScr, NULL);

	SVisualizerScript *node = g_pVisScr;
	while (node)
	{
		switch (node->kind)
		{
		case VisualizerScript_AutoExp:
 			if (CompareTypes(psymT, node->autoexp->match_type))
 				return node;
			break;

		case VisualizerScript_Visualizer:
			if (CompareTypes(psymT, node->vis->matchType))
				return node;
			break;
		}
		node = node->next;
	}
	return NULL;
}


//------------------------------------------------------------------------
// visualizer script �� OR �������� �ϳ� �̻��� Ÿ����
// ������ �� �ִ�. ���߿� �� Ÿ���̶� ���ٸ�,
// �ش� visualizer �� ��Ī�ȴ�.
//------------------------------------------------------------------------
bool visualizer::CompareTypes( SType_Stmt *psymT, 
																	  SType_Stmts *pvisT)
{
	SType_Stmts *node = pvisT;
	while (node)
	{
		bool isAsterisk = false;
		if (CompareType(psymT, node->type, isAsterisk))
			return true;
		node = node->next;
	}
	return false;
}


//------------------------------------------------------------------------
// t1, t2 �ΰ��� Ÿ���� ���ؼ� ���ٸ� true�� �����Ѵ�.
// psymT : dia ���� �Ѿ���� Ÿ�Ը�
// pvisT : visualizer ��ũ��Ʈ Ÿ�� '*' �ƽ��׸�ũ�� �����Ѵ�.
// * �� �����ٸ� isAstrick ���� true�� ���ϵȴ�.
//------------------------------------------------------------------------
bool visualizer::CompareType( SType_Stmt *psymT, SType_Stmt *pvisT, OUT bool &isAstrisk )
{
	RETV(!psymT && !pvisT, true);
	RETV(!psymT, false);
	RETV(!pvisT, false);

	isAstrisk  = false;
	if (pvisT->id == "*")
	{
		isAstrisk = true;
		return true;
	}

	if (psymT->id == pvisT->id)
	{
		if (psymT->templateArgs && pvisT->templateArgs)
		{
			return CompareTemplateTypes(psymT->templateArgs, pvisT->templateArgs);
		}
		else if (!psymT->templateArgs && !pvisT->templateArgs)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}


//------------------------------------------------------------------------
// ���ø� Ÿ�Կ��� �ΰ��� 
//------------------------------------------------------------------------
bool visualizer::CompareTemplateTypes( SType_TemplateArgs *psymT, 
						  SType_TemplateArgs *pvisT )
{
	RETV(!psymT && !pvisT, true);
	RETV(!psymT, false);
	RETV(!pvisT, false);

	bool isAsterisk = false;
	if (CompareType(psymT->type, pvisT->type, isAsterisk))
	{
		if (isAsterisk)
			return true;
		return CompareTemplateTypes(psymT->next, pvisT->next);
	}
	return false;
}


//------------------------------------------------------------------------
//  SExpression ���
//------------------------------------------------------------------------
void visualizer::Disp_Expression( SExpression *pexp )
{

}


//------------------------------------------------------------------------
// SExpression ���� �����Ѵ�.
//------------------------------------------------------------------------
_variant_t visualizer::Eval_Expression( SExpression *pexp, const SMakerData &makerData,
	OUT SSymbolInfo *pOut) // pOut=NULL
{
	_variant_t reval;
	RETV(!pexp, reval);

	switch (pexp->kind)
	{
	case CondExprK:
		{
			switch (pexp->op)
			{
			case LT: 
				reval = ((float)Eval_Expression(pexp->lhs, makerData,pOut) < (float)Eval_Expression(pexp->rhs, makerData, pOut)); 
				break;
			case RT: 
				reval = ((float)Eval_Expression(pexp->lhs, makerData, pOut) > (float)Eval_Expression(pexp->rhs, makerData, pOut)); 
				break;
			case LTEQ: /* <= */
				reval = ((float)Eval_Expression(pexp->lhs, makerData, pOut) < (float)Eval_Expression(pexp->rhs, makerData, pOut)) 
					|| ((float)Eval_Expression(pexp->lhs, makerData, pOut) == (float)Eval_Expression(pexp->rhs, makerData, pOut));
				break;
			case RTEQ:	/* >= */
				reval = ((float)Eval_Expression(pexp->lhs, makerData, pOut) > (float)Eval_Expression(pexp->rhs, makerData, pOut)) 
					|| ((float)Eval_Expression(pexp->lhs, makerData, pOut) == (float)Eval_Expression(pexp->rhs, makerData, pOut));
				break;
			case NEQ: /* != */
				reval = ((int)Eval_Expression(pexp->lhs, makerData, pOut) != (int)Eval_Expression(pexp->rhs, makerData, pOut)); 
				break; 
			case EQ:	/* == */
				reval = ((int)Eval_Expression(pexp->lhs, makerData, pOut) == (int)Eval_Expression(pexp->rhs, makerData, pOut)); 
				break; 
			case OR: /* || */
				reval = ((int)Eval_Expression(pexp->lhs, makerData, pOut).llVal || (int)Eval_Expression(pexp->rhs, makerData, pOut).llVal); 
				break; 
			case AND: /* && */
				reval = ((int)Eval_Expression(pexp->lhs, makerData, pOut).llVal && (int)Eval_Expression(pexp->rhs, makerData, pOut).llVal); 
				break; 
			case NEG: 
				reval = (!(int)Eval_Expression(pexp->lhs, makerData, pOut).llVal); 
				break;
			}
		}	
		break;

	case AddTermK: 
		{
			SSymbolInfo lhsSymbol;
			SSymbolInfo rhsSymbol;
			if (pexp->op == PLUS)
				reval = ((float)Eval_Expression(pexp->lhs, makerData, &lhsSymbol) + (float)Eval_Expression(pexp->rhs, makerData, &rhsSymbol));
			else if(pexp->op == MINUS)
				reval = ((float)Eval_Expression(pexp->lhs, makerData, &lhsSymbol) - (float)Eval_Expression(pexp->rhs, makerData, &rhsSymbol));

			// pointer ���� ����Ÿũ�⸦ �����ؼ� ���Ǿ�� �Ѵ�.
			if (lhsSymbol.pSym && rhsSymbol.pSym)
			{
				SymbolState lhsState;
				IDiaSymbol* pLhsTypeSym = GetBaseTypeSymbol(lhsSymbol.pSym, 1, lhsState);
				if (!pLhsTypeSym)
					break;
				
				SymbolState rhsState;
				IDiaSymbol* pRhsTypeSym = GetBaseTypeSymbol(rhsSymbol.pSym, 1, rhsState);
				if (!pRhsTypeSym)
					break;

				enum SymTagEnum lhsSymTag;					 
				HRESULT hr = pLhsTypeSym->get_symTag((DWORD*)&lhsSymTag);
				enum SymTagEnum rhsSymTag;					 
				hr = pRhsTypeSym->get_symTag((DWORD*)&rhsSymTag);

				// ���� ��� ������ Ÿ���� �� ���
				if  (((lhsSymTag == SymTagPointerType) || (lhsSymTag == SymTagArrayType)) &&
					((rhsSymTag == SymTagPointerType) || (rhsSymTag == SymTagArrayType)))
				{
					// left �� �켱���� ����Ѵ�. �����Ͱ� ����Ű�� ����Ÿ�� ũ�⸦ �������� ����, �ٽ� 
					// Ÿ���� ����Ѵ�.
					SymbolState typeState;
					IDiaSymbol* pRealTypeSym = GetBaseTypeSymbol(lhsSymbol.pSym, 2, typeState);
					if (!pRealTypeSym)
						break;

					ULONGLONG length = 0;
					hr = pRealTypeSym->get_length(&length);

					if (length > 0)
						reval = (int)reval / (int)length;

					if (NEW_SYMBOL == typeState)
						SAFE_RELEASE(pRealTypeSym);
				}

				if (NEW_SYMBOL == lhsState)
					SAFE_RELEASE(pLhsTypeSym);
				if (NEW_SYMBOL == rhsState)
					SAFE_RELEASE(pRhsTypeSym);
			}
		}
		break;

	case MulTermK:
		//reval =  visualizer::VariantCalc(GetAsciiFromTokentype(pexp->op), 
		//	Eval_Expression(pexp->lhs, makerData), Eval_Expression(pexp->rhs, makerData)); 
		break;

	case VariableK:
		reval = Eval_Variable(pexp, makerData, pOut);
		break;

	case IndirectK:
		break;

	case NumberK: reval = pexp->num; break;
	case StringK: reval = pexp->str.c_str(); break;
	}

	return reval;
}


//------------------------------------------------------------------------
// Variable �� �����Ѵ�.
//------------------------------------------------------------------------
_variant_t visualizer::Eval_Variable( SExpression *pexp, const SMakerData &makerData, 
	OUT SSymbolInfo *pOut ) //pOut = NULL
{
	_variant_t reval;
	RETV(!pexp, reval);
	RETV(pexp->kind != VariableK, reval);

	SSymbolInfo findSymbol;
	if (!Find_Variable(pexp, makerData, &findSymbol))
		return reval;

	enum SymTagEnum symTag;
	HRESULT hr = findSymbol.pSym->get_symTag((DWORD*)&symTag);
	if (SymTagData == symTag) // constant data �ϰ�� ó��
	{
		DWORD dwLocType;
		hr = findSymbol.pSym->get_locationType(&dwLocType);
		if (LocIsConstant == dwLocType) // ��� 
			findSymbol.pSym->get_value(&reval);
	}

	if (reval.vt == VT_EMPTY)
		reval = dia::GetValueFromSymbol( findSymbol.mem.ptr, findSymbol.pSym);

	// ���߿� ó��
	switch (pexp->prefix_op)
	{
	case PLUS:
	case MINUS:
	case TIMES:
	case NEG:
	case INC: // ++
	case DEC: // --
		break;
	case REF: // &
		break;
	}

	if (pOut)
	{
		*pOut = findSymbol;
		findSymbol.isNotRelease = true; // pOut���� ��ü�ҰŸ� �ѱ��.
	}

	return reval;
}


/**
 @brief 
 */
bool visualizer::Find_Expression( SExpression *pexp, 
	IN const SMakerData &makerData, OUT SSymbolInfo *pOut)
{
	bool reval = false;
	RETV(!pexp, false);

	switch (pexp->kind)
	{
	case CondExprK:
	case AddTermK:
	case MulTermK:
		break;

	case VariableK: 
		reval = Find_Variable(pexp, makerData, pOut); 
		break;

	case IndirectK:
		{
			SSymbolInfo newSymbol;
			reval = Find_Indirect(pexp, makerData, &newSymbol);
			if (!reval)
				break;
			if (pexp->rhs)
			{
				SMakerData indirectMakerData = makerData;
				indirectMakerData.symbol = newSymbol;
				indirectMakerData.symbol.isNotRelease = true;
				reval = Find_Expression(pexp->rhs, indirectMakerData, pOut);
			}
			else
			{
				if (pOut)
				{
					*pOut = newSymbol;
					newSymbol.isNotRelease = true;
				}
			}
		}
		break;

	case NumberK:
	case StringK:
	case DispFormatK:
		break;
	}

	return true;
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
bool visualizer::Find_Variable( SExpression *pexp, IN const SMakerData &makerData, 
	OUT SSymbolInfo *pOut )
{
	bool reval = false;
	RETV(!pexp, false);
	RETV((pexp->kind != VariableK), false);

	if (pexp->str == "$e")
	{
		if (pexp->rhs)
		{
			reval = Find_Expression(pexp->rhs, makerData, pOut);
		}
		else
		{
			if (pOut)
			{
				pOut->isNotRelease = true; // symbol�� ���纻�̹Ƿ� release�ؼ� �ȵ�
				pOut->pSym = makerData.symbol.pSym;
				pOut->mem = SMemInfo( makerData.origSymbolName.c_str(), makerData.origMem.ptr, 0 );
				reval = true;
			}
		}
	}
	else if (pexp->str == "$r")
	{

	}
	else if (pexp->str == "$i")
	{
		if (pOut)
		{
			 pOut->pSym = dia::FindType("visualizer::g_BaseTypeInt");
			if (!pOut->pSym)
				return false;
			pOut->mem = SMemInfo("int", (void*)&makerData.index, 0);
			reval = true;
		}
	}
	else
	{
		if (pexp->rhs)
		{
			SSymbolInfo childSymbol;
			if (!Find_ChildSymbol(pexp->str, makerData.symbol, &childSymbol))
				return false;

			SMakerData childMakerData = makerData;
			childMakerData.symbol = childSymbol;
			const string childName = dia::GetSymbolName(childSymbol.pSym);
			reval = Find_Expression(pexp->rhs, childMakerData, pOut);
			childMakerData.symbol.isNotRelease = true;
		}
		else
		{
			reval = Find_ChildSymbol(pexp->str, makerData.symbol, pOut);
		}
	}

	return reval;
}


//------------------------------------------------------------------------
// symbol �� �ڽ����� findSymbolName �� �ش��ϴ� �ɺ���
// �ִٸ� pOut�� �����ؼ� �����Ѵ�. �� �� �ڽ��� �Ѵܰ踸
// ��������.
//------------------------------------------------------------------------
bool visualizer::Find_ChildSymbol(  const std::string findSymbolName,
								  IN const SSymbolInfo &symbol, OUT SSymbolInfo *pOut )
{
	RETV(!pOut, false);

	enum SymTagEnum symTag;
	HRESULT hr = symbol.pSym->get_symTag((DWORD*)&symTag);

	switch (symTag)
	{
	case SymTagData:
	case SymTagTypedef:
		{
			IDiaSymbol *pNewSymbol = NULL;
			HRESULT hr = symbol.pSym->get_type(&pNewSymbol);
			ASSERT_RETV(hr == S_OK, false);

			return Find_ChildSymbol( findSymbolName, SSymbolInfo(pNewSymbol, symbol.mem), pOut );
		}
		return true;

	case SymTagPointerType:
		{
			IDiaSymbol *pNewSymbol = NULL;
			HRESULT hr = symbol.pSym->get_type(&pNewSymbol);
			ASSERT_RETV(hr == S_OK, false);

			void *srcPtr = (void*)*(DWORD*)symbol.mem.ptr;
			void *newPtr = MemoryMapping(srcPtr);
			if (newPtr) //�����޸𸮿��� ����� newPtr�� NULL�̵ȴ�.
			{
					return Find_ChildSymbol( findSymbolName,
						SSymbolInfo(pNewSymbol, SMemInfo(symbol.mem.name.c_str(), newPtr, 0)),
						pOut);
			}
		}
		return false;

	default:
		{
			LONG offset = 0;
			IDiaSymbol *pFindSymbol = dia::FindChildSymbol(findSymbolName, symbol.pSym, &offset);
			RETV(!pFindSymbol, false);

			if (pOut)
			{
				pOut->pSym = pFindSymbol;

				// memInfo
				pOut->mem.name = dia::GetSymbolName(pFindSymbol);
				pOut->mem.ptr = (BYTE*)symbol.mem.ptr + offset;
				pOut->mem.size = 0;
			}
		}
		return true;
	}
}


//------------------------------------------------------------------------
// ���б�ȣ�� �����Ѵ�.
//------------------------------------------------------------------------
char visualizer::GetAsciiFromTokentype(Tokentype tok)
{
	char c = NULL;
	switch (tok)
	{
	case PLUS: c = '+'; break;
	case MINUS: c = '-'; break;
	case TIMES: c = '*'; break;
	case DIV: c = '/'; break;
	}
	return c;
}


//------------------------------------------------------------------------
// ���� üũ
// condition �� true �̸� ���� ����
//------------------------------------------------------------------------
void visualizer::CheckError( bool condition, const SMakerData &makerData,  const string &msg ) // msg="error"
{
	if (condition) // success
		return;

	// error occur
	throw VisualizerScriptError( makerData.origSymbolName + " " + msg);
	//throw VisualizerScriptError( msg);
}
