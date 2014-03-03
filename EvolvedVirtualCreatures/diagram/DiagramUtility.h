/**
 @filename DiagramUtility.h
 
*/
#pragma once


class RenderBezierActor;

namespace evc
{
	class CGenotypeNode;
	namespace genotype_parser { struct SExpr; struct SConnection; }


	// genotype node
	CGenotypeNode* CreateGenotypeNode(CEvc &sample, const genotype_parser::SExpr *expr);

	PxVec3 GetDiagramPosition(CGenotypeNode *parent, CGenotypeNode *dispNode, OUT u_int &order);
	PxVec3 GetDiagramPositionByIndex(const genotype_parser::SExpr *parent_expr, const PxVec3 &parentPos, const u_int index, OUT u_int &order);


	// transition
	RenderBezierActor* CreateTransition(CEvc &sample, CGenotypeNode *from, CGenotypeNode *to, const u_int order=0);
	void MoveTransition(RenderBezierActor *transition, CGenotypeNode *from, CGenotypeNode *to, const u_int order=0);
	void CalcuateTransitionPositions(CGenotypeNode *from, CGenotypeNode *to, const u_int order, OUT vector<PxVec3> &out);

	// etc
	void RemoveGenotypeTree(CEvc &sample, CGenotypeNode *node);
}
