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
	Opd * src = mySrc->flatten(proc);
	Opd * dst = myDst->flatten(proc);
	assert(dst != nullptr);
	assert(src != nullptr);

	//create quad
	AssignQuad * quad = new AssignQuad(dst, src);
	proc->addQuad(quad);
	return dst;
	//return nullptr;//?? what to return??
}

Opd * LValNode::flatten(Procedure * proc){
	throw new InternalError("Inside LVal");
}

Opd * CallExpNode::flatten(Procedure * proc){
	for (auto arg : *myArgs) {
		arg -> flatten(proc);
	}

	CallQuad * quad = new CallQuad(myID -> getSymbol());
	proc -> addQuad(quad);
	if (getRetType() -> isVoid()) {
		return myID -> flatten(proc);
	}

	AuxOpd * tmp = proc -> makeTmp(8);
	GetRetQuad* grQuad = new GetRetQuad(tmp);
	proc -> addQuad(grQuad);
	return tmp;
}

Opd* CallExpNode::flattenAsStmt(Procedure * proc)
{
	for (auto arg : *myArgs) {
		arg -> flatten(proc);
	}

	if (!(getRetType() -> asFn() -> isVoid())) {
		AuxOpd * tmp = proc -> makeTmp(8);
	}

	CallQuad * quad = new CallQuad(myID -> getSymbol());
	proc -> addQuad(quad);
	return myID -> flatten(proc);
}

Opd * NegNode::flatten(Procedure * proc){
	//get operands
	Opd * src = myExp->flatten(proc);
	AuxOpd * tmp = proc->makeTmp(8); //8?
	assert(tmp != nullptr);

	//create UnaryOpQuad and add to function body
	UnaryOp neg = NEG64;
	UnaryOpQuad * quad = new UnaryOpQuad(tmp, neg, src);
	proc->addQuad(quad);
	return tmp;
}

Opd * NotNode::flatten(Procedure * proc){
	//get operands
	Opd * src = myExp->flatten(proc);
	AuxOpd * tmp = proc->makeTmp(8); //8?
	assert(tmp != nullptr);

	//create UnaryOpQuad and add to function body
	UnaryOp _not = NOT8; // should it be NOT64 as on the oracle
	UnaryOpQuad * quad = new UnaryOpQuad(tmp, _not, src);
	proc->addQuad(quad);
	return tmp;
}

Opd * PlusNode::flatten(Procedure * proc){
	//get operands
	Opd * src1 = myExp1->flatten(proc);
	Opd * src2 = myExp2->flatten(proc);
	AuxOpd * tmp = proc->makeTmp(8); //8?
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
	Opd * src1 = myExp1->flatten(proc);
	Opd * src2 = myExp2->flatten(proc);
	AuxOpd * tmp = proc->makeTmp(8); //8?
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
	Opd * src1 = myExp1->flatten(proc);
	Opd * src2 = myExp2->flatten(proc);
	AuxOpd * tmp = proc->makeTmp(8); //8?
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
	Opd * src1 = myExp1->flatten(proc);
	Opd * src2 = myExp2->flatten(proc);
	AuxOpd * tmp = proc->makeTmp(8); //8?
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
	Opd * src1 = myExp1->flatten(proc);
	Opd * src2 = myExp2->flatten(proc);
	AuxOpd * tmp = proc->makeTmp(8); //8?
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
	Opd * src1 = myExp1->flatten(proc);
	Opd * src2 = myExp2->flatten(proc);
	AuxOpd * tmp = proc->makeTmp(8); //8?
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
	Opd * src1 = myExp1->flatten(proc);
	Opd * src2 = myExp2->flatten(proc);
	AuxOpd * tmp = proc->makeTmp(8); //8?
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
	Opd * src1 = myExp1->flatten(proc);
	Opd * src2 = myExp2->flatten(proc);
	AuxOpd * tmp = proc->makeTmp(8); //8?
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
	Opd * src1 = myExp1->flatten(proc);
	Opd * src2 = myExp2->flatten(proc);
	AuxOpd * tmp = proc->makeTmp(8); //8?
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
	Opd * src1 = myExp1->flatten(proc);
	Opd * src2 = myExp2->flatten(proc);
	AuxOpd * tmp = proc->makeTmp(8); //8?
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
	Opd * src1 = myExp1->flatten(proc);
	Opd * src2 = myExp2->flatten(proc);
	AuxOpd * tmp = proc->makeTmp(8); //8?
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
	Opd * src1 = myExp1->flatten(proc);
	Opd * src2 = myExp2->flatten(proc);
	AuxOpd * tmp = proc->makeTmp(8); //8?
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
	Opd* src = myExp -> flatten(proc);
	AuxOpd* tmp = proc->makeTmp(8);
	AssignQuad * quad = new AssignQuad(tmp, src);
	proc -> addQuad(quad);
	return tmp;
}

Opd * RefNode::flatten(Procedure * proc){
	Opd* id = myID -> flatten(proc);
	AuxOpd * tmp = proc -> makeTmp(8);
	LocQuad * quad = new LocQuad(id, tmp, true, false);
	proc -> addQuad(quad);
	return tmp;
}

Opd * DerefNode::flatten(Procedure * proc){
	Opd * id = myID -> flatten(proc);
	AddrOpd * addrTemp = proc -> makeAddrOpd(8);
	LocQuad * quad = new LocQuad(id, addrTemp, false, true);
	proc -> addQuad(quad);
	return addrTemp;
}

void AssignStmtNode::to3AC(Procedure * proc){
	myExp->flatten(proc);
}

void PostIncStmtNode::to3AC(Procedure * proc){
	LitOpd* lit = new LitOpd("1", 8);
	Opd * src = myLVal -> flatten(proc);
	BinOp op = ADD64;
	BinOpQuad * quad = new BinOpQuad(src, op, src, lit);
	proc -> addQuad(quad);
}

void PostDecStmtNode::to3AC(Procedure * proc){
	LitOpd* lit = new LitOpd("1", 8);
	Opd * src = myLVal -> flatten(proc);
	BinOp op = SUB64;
	BinOpQuad * quad = new BinOpQuad(src, op, src, lit);
	proc -> addQuad(quad);
}

void ReadStmtNode::to3AC(Procedure * proc){
	Opd * dst = myDst -> flatten(proc);
	const DataType * t = proc -> getProg() -> nodeType(myDst);
	ReceiveQuad * rQuad = new ReceiveQuad(dst, t);
	proc -> addQuad(rQuad);
}

void WriteStmtNode::to3AC(Procedure * proc){
	Opd * src = mySrc -> flatten(proc);
	const DataType * t = proc -> getProg() -> nodeType(mySrc);
	ReportQuad * quad = new ReportQuad(src, t);
	proc -> addQuad(quad);
}

void IfStmtNode::to3AC(Procedure * proc){
	Opd* cond = myCond -> flatten(proc);
	Label * skip = proc -> makeLabel();
	IfzQuad * ifzQuad = new IfzQuad(cond, skip);
	proc -> addQuad(ifzQuad);

	for (auto stmt : *myBody) {
		stmt -> to3AC(proc);
	}

	NopQuad * nop = new NopQuad();
	nop -> addLabel(skip);
	proc -> addQuad(nop);
}

void IfElseStmtNode::to3AC(Procedure * proc){
	Opd* cond = myCond -> flatten(proc);
	Label * skip = proc -> makeLabel();
	Label * end = proc -> makeLabel();
	IfzQuad * ifzQuad = new IfzQuad(cond, skip);
	proc -> addQuad(ifzQuad);

	for (auto stmt : *myBodyTrue) {
		stmt -> to3AC(proc);
	}

	GotoQuad * gotoQuad = new GotoQuad(end);
	proc -> addQuad(gotoQuad);
	
	NopQuad * nop = new NopQuad();
	nop -> addLabel(skip);
	proc -> addQuad(nop);

	for (auto stmt : *myBodyFalse) {
		stmt -> to3AC(proc);
	}

	NopQuad * nop2 = new NopQuad();
	nop2 -> addLabel(end);
	proc -> addQuad(nop2);
}

void WhileStmtNode::to3AC(Procedure * proc){
	Label * start = proc -> makeLabel();
	Label * end = proc -> makeLabel();
	NopQuad * nop1 = new NopQuad();

	nop1 -> addLabel(start);
	proc -> addQuad(nop1);

	Opd* cond = myCond -> flatten(proc);
	IfzQuad * ifzQuad = new IfzQuad(cond, end);
	proc -> addQuad(ifzQuad);

	for (auto stmt : *myBody) {
		stmt -> to3AC(proc);
	}

	GotoQuad * gotoQuad = new GotoQuad(start);
	NopQuad * nop2 = new NopQuad();
	proc -> addQuad(gotoQuad);
	nop2 -> addLabel(end);
	proc -> addQuad(nop2);
}

void CallStmtNode::to3AC(Procedure * proc){
	myCallExp -> flattenAsStmt(proc);
}

void ReturnStmtNode::to3AC(Procedure * proc){
	Opd * src = myExp -> flatten(proc);
	SetRetQuad * srQuad = new SetRetQuad(src);
	GotoQuad * gotoQuad = new GotoQuad(proc -> getLeaveLabel());
	proc -> addQuad(srQuad);
	proc -> addQuad(gotoQuad);
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
