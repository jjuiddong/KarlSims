
#include "stdafx.h"
#include "EditCursor.h"

using namespace evc;

CEditCursor::CEditCursor() :
	m_selectNode(NULL)
{

}

CEditCursor::~CEditCursor()
{

}


/**
 @brief 
 @date 2014-03-07
*/
void CEditCursor::SelectNode(CGenotypeNode *node)
{
	m_selectNode = node;
	onSelectNode(node);
}

