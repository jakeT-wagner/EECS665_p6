#include "ast.hpp"

namespace cminusminus{

IRProgram * ProgramNode::to3AC(TypeAnalysis * ta){
	IRProgram * prog = new IRProgram(ta);
	for (auto global : *myGlobals){
		global->to3AC(prog);
	}
	return prog;
}

void FnDeclNode::to3AC(IRProgram * prog){
	//make a procedure and add it to prog then call its interior statements
	//with the proc
	Procedure * proc = prog->makeProc(ID()->getName());
	for (auto formal : *myFormals) {
		formal->to3AC(proc);
	}
	for (auto body : *myBody) {
		body->to3AC(proc);
	}
	
	//TODO(Implement me)
}

void FnDeclNode::to3AC(Procedure * proc){ //proc = local scope?
	//This never needs to be implemented,
	// the function only exists because of 
	// inheritance needs (A function declaration
	// never occurs within another function)
	throw new InternalError("FnDecl at a local scope");
}

void FormalDeclNode::to3AC(IRProgram * prog){ //prog = global scope?
	//This never needs to be implemented,
	// the function only exists because of 
	// inheritance needs (A formal never 
	// occurs at global scope)
	throw new InternalError("Formal at a global scope");
}

void FormalDeclNode::to3AC(Procedure * proc){ //this is for a formal in a function

	SemSymbol * sym = ID()->getSymbol();
	assert(sym != nullptr);
	proc->gatherFormal(sym);
	//create quad to getarg
	size_t index = proc->getFormals().size();
	SymOpd * opd = proc->getFormal(index-1);
	GetArgQuad * quad = new GetArgQuad(index, opd);
	proc->addQuad(quad);
}

Opd * ShortLitNode::flatten(Procedure * proc){
	const DataType * type = proc->getProg()->nodeType(this);
	return new LitOpd(std::to_string(myNum), 1);
}

Opd * IntLitNode::flatten(Procedure * proc){
	const DataType * type = proc->getProg()->nodeType(this);
	return new LitOpd(std::to_string(myNum), 8);
}

Opd * StrLitNode::flatten(Procedure * proc){
	Opd * res = proc->getProg()->makeString(myStr);
	return res;
}

Opd * TrueNode::flatten(Procedure * proc){
	const DataType * type = proc->getProg()->nodeType(this);
	return new LitOpd("1", 8);
}

Opd * FalseNode::flatten(Procedure * proc){
	const DataType * type = proc->getProg()->nodeType(this);
	return new LitOpd("0", 8);
}

Opd * AssignExpNode::flatten(Procedure * proc){
	//get operands
	Opd * dst = myDst->flatten(proc);
	Opd * src = mySrc->flatten(proc);
	assert(dst != nullptr);
	assert(src != nullptr);

	//create quad
	AssignQuad * quad = new AssignQuad(dst, src);
	proc->addQuad(quad);
	return nullptr;//?? what to return??
}

Opd * LValNode::flatten(Procedure * proc){
	TODO(Implement me)
}

Opd * CallExpNode::flatten(Procedure * proc){
	TODO(Implement me)
}

Opd * NegNode::flatten(Procedure * proc){
	//get operands
	AuxOpd * tmp = proc->makeTmp(8); //8?
	Opd * src = myExp->flatten(proc);
	assert(tmp != nullptr);

	//create UnaryOpQuad and add to function body
	UnaryOp neg = NEG64;
	UnaryOpQuad * quad = new UnaryOpQuad(tmp, neg, src);
	proc->addQuad(quad);
	return tmp;
}

Opd * NotNode::flatten(Procedure * proc){
	//get operands
	AuxOpd * tmp = proc->makeTmp(8); //8?
	Opd * src = myExp->flatten(proc);
	assert(tmp != nullptr);

	//create UnaryOpQuad and add to function body
	UnaryOp _not = NOT8; // should it be NOT64 as on the oracle
	UnaryOpQuad * quad = new UnaryOpQuad(tmp, _not, src);
	proc->addQuad(quad);
	return tmp;
}

Opd * PlusNode::flatten(Procedure * proc){
	//get operands
	AuxOpd * tmp = proc->makeTmp(8); //8?
	Opd * src1 = myExp1->flatten(proc);
	Opd * src2 = myExp2->flatten(proc);
	assert(tmp != nullptr);
	assert(src1 != nullptr);
	assert(src2 != nullptr);

	//create BinOpQuad and add to function body
	BinOp plus = ADD64;
	BinOpQuad * quad = new BinOpQuad(tmp, plus, src1, src2);
	proc->addQuad(quad);
	return tmp;
}
//Hello everyone... I am a computer
Opd * MinusNode::flatten(Procedure * proc){
	//get operands
	AuxOpd * tmp = proc->makeTmp(8); //8?
	Opd * src1 = myExp1->flatten(proc);
	Opd * src2 = myExp2->flatten(proc);
	assert(tmp != nullptr);
	assert(src1 != nullptr);
	assert(src2 != nullptr);

	//create BinOpQuad and add to function body
	BinOp minus = SUB64;
	BinOpQuad * quad = new BinOpQuad(tmp, minus, src1, src2);
	proc->addQuad(quad);
	return tmp;
}

Opd * TimesNode::flatten(Procedure * proc){
	//get operands
	AuxOpd * tmp = proc->makeTmp(8); //8?
	Opd * src1 = myExp1->flatten(proc);
	Opd * src2 = myExp2->flatten(proc);
	assert(tmp != nullptr);
	assert(src1 != nullptr);
	assert(src2 != nullptr);

	//create BinOpQuad and add to function body
	BinOp times = MULT64;
	BinOpQuad * quad = new BinOpQuad(tmp, times, src1, src2);
	proc->addQuad(quad);
	return tmp;
}

Opd * DivideNode::flatten(Procedure * proc){
	//get operands
	AuxOpd * tmp = proc->makeTmp(8); //8?
	Opd * src1 = myExp1->flatten(proc);
	Opd * src2 = myExp2->flatten(proc);
	assert(tmp != nullptr);
	assert(src1 != nullptr);
	assert(src2 != nullptr);

	//create BinOpQuad and add to function body
	BinOp div = DIV64;
	BinOpQuad * quad = new BinOpQuad(tmp, div, src1, src2);
	proc->addQuad(quad);
	return tmp;
}

Opd * AndNode::flatten(Procedure * proc){
	//get operands
	AuxOpd * tmp = proc->makeTmp(8); //8?
	Opd * src1 = myExp1->flatten(proc);
	Opd * src2 = myExp2->flatten(proc);
	assert(tmp != nullptr);
	assert(src1 != nullptr);
	assert(src2 != nullptr);

	//create BinOpQuad and add to function body
	BinOp _and = AND64;
	BinOpQuad * quad = new BinOpQuad(tmp, _and, src1, src2);
	proc->addQuad(quad);
	return tmp;
}

Opd * OrNode::flatten(Procedure * proc){
	//get operands
	AuxOpd * tmp = proc->makeTmp(8); //8?
	Opd * src1 = myExp1->flatten(proc);
	Opd * src2 = myExp2->flatten(proc);
	assert(tmp != nullptr);
	assert(src1 != nullptr);
	assert(src2 != nullptr);

	//create BinOpQuad and add to function body
	BinOp _or = OR64;
	BinOpQuad * quad = new BinOpQuad(tmp, _or, src1, src2);
	proc->addQuad(quad);
	return tmp;
}

Opd * EqualsNode::flatten(Procedure * proc){
	//get operands
	AuxOpd * tmp = proc->makeTmp(8); //8?
	Opd * src1 = myExp1->flatten(proc);
	Opd * src2 = myExp2->flatten(proc);
	assert(tmp != nullptr);
	assert(src1 != nullptr);
	assert(src2 != nullptr);

	//create BinOpQuad and add to function body
	BinOp eq = EQ64;
	BinOpQuad * quad = new BinOpQuad(tmp, eq, src1, src2);
	proc->addQuad(quad);
	return tmp;
}

Opd * NotEqualsNode::flatten(Procedure * proc){
	//get operands
	AuxOpd * tmp = proc->makeTmp(8); //8?
	Opd * src1 = myExp1->flatten(proc);
	Opd * src2 = myExp2->flatten(proc);
	assert(tmp != nullptr);
	assert(src1 != nullptr);
	assert(src2 != nullptr);

	//create BinOpQuad and add to function body
	BinOp neq = NEQ64;
	BinOpQuad * quad = new BinOpQuad(tmp, neq, src1, src2);
	proc->addQuad(quad);
	return tmp;
}

Opd * LessNode::flatten(Procedure * proc){
	//get operands
	AuxOpd * tmp = proc->makeTmp(8); //8?
	Opd * src1 = myExp1->flatten(proc);
	Opd * src2 = myExp2->flatten(proc);
	assert(tmp != nullptr);
	assert(src1 != nullptr);
	assert(src2 != nullptr);

	//create BinOpQuad and add to function body
	BinOp lt = LT64;
	BinOpQuad * quad = new BinOpQuad(tmp, lt, src1, src2);
	proc->addQuad(quad);
	return tmp;
}

Opd * GreaterNode::flatten(Procedure * proc){
	//get operands
	AuxOpd * tmp = proc->makeTmp(8); //8?
	Opd * src1 = myExp1->flatten(proc);
	Opd * src2 = myExp2->flatten(proc);
	assert(tmp != nullptr);
	assert(src1 != nullptr);
	assert(src2 != nullptr);

	//create BinOpQuad and add to function body
	BinOp gt = GT64;
	BinOpQuad * quad = new BinOpQuad(tmp, gt, src1, src2);
	proc->addQuad(quad);
	return tmp;
}

Opd * LessEqNode::flatten(Procedure * proc){
	//get operands
	AuxOpd * tmp = proc->makeTmp(8); //8?
	Opd * src1 = myExp1->flatten(proc);
	Opd * src2 = myExp2->flatten(proc);
	assert(tmp != nullptr);
	assert(src1 != nullptr);
	assert(src2 != nullptr);

	//create BinOpQuad and add to function body
	BinOp lte = LTE64;
	BinOpQuad * quad = new BinOpQuad(tmp, lte, src1, src2);
	proc->addQuad(quad);
	return tmp;
}

Opd * GreaterEqNode::flatten(Procedure * proc){
	//get operands
	AuxOpd * tmp = proc->makeTmp(8); //8?
	Opd * src1 = myExp1->flatten(proc);
	Opd * src2 = myExp2->flatten(proc);
	assert(tmp != nullptr);
	assert(src1 != nullptr);
	assert(src2 != nullptr);

	//create BinOpQuad and add to function body
	BinOp gte = GTE64;
	BinOpQuad * quad = new BinOpQuad(tmp, gte, src1, src2);
	proc->addQuad(quad);
	return tmp;
}

Opd * ShortToIntNode::flatten(Procedure * proc){
	TODO(Implement me)
}

Opd * RefNode::flatten(Procedure * proc){
	TODO(Implement me)
}

Opd * DerefNode::flatten(Procedure * proc){
	TODO(Implement me)
}

void AssignStmtNode::to3AC(Procedure * proc){
	myExp->flatten(proc);
}

void PostIncStmtNode::to3AC(Procedure * proc){
	TODO(Implement me)
}

void PostDecStmtNode::to3AC(Procedure * proc){
	TODO(Implement me)
}

void ReadStmtNode::to3AC(Procedure * proc){
	TODO(Implement me)
}

void WriteStmtNode::to3AC(Procedure * proc){
	TODO(Implement me)
}

void IfStmtNode::to3AC(Procedure * proc){
	TODO(Implement me)
}

void IfElseStmtNode::to3AC(Procedure * proc){
	TODO(Implement me)
}

void WhileStmtNode::to3AC(Procedure * proc){
	TODO(Implement me)
}

void CallStmtNode::to3AC(Procedure * proc){
	TODO(Implement me)
}

void ReturnStmtNode::to3AC(Procedure * proc){
	TODO(Implement me)
}

void VarDeclNode::to3AC(Procedure * proc){
	SemSymbol * sym = ID()->getSymbol();
	assert(sym != nullptr);
	proc->gatherLocal(sym);
}

void VarDeclNode::to3AC(IRProgram * prog){
	SemSymbol * sym = ID()->getSymbol();
	assert(sym != nullptr);
	prog->gatherGlobal(sym);
}

//We only get to this node if we are in a stmt
// context (DeclNodes protect descent) 
Opd * IDNode::flatten(Procedure * proc){
	SemSymbol * sym = mySymbol;
	return proc->getSymOpd(sym);
}

}
