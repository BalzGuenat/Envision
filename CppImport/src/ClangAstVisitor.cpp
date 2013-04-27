/***********************************************************************************************************************
 **
 ** Copyright (c) 2011, 2013 ETH Zurich
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
 ** following conditions are met:
 **
 **    * Redistributions of source code must retain the above copyright notice, this list of conditions and the
 **      following disclaimer.
 **    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
 **      following disclaimer in the documentation and/or other materials provided with the distribution.
 **    * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
 **      derived from this software without specific prior written permission.
 **
 **
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 ** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 ** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 ** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 ** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **
 **********************************************************************************************************************/

#include "ClangAstVisitor.h"
#include "CppImportUtilities.h"

namespace CppImport {

ClangAstVisitor::ClangAstVisitor(Model::Model* model, OOModel::Project* currentProject, CppImportLogger* logger) :
	currentModel_(model) , currentProject_(currentProject) , log_(logger)
{
	trMngr_ = new TranslateManager(model,currentProject);
	ooStack_.push(currentProject_);
}

ClangAstVisitor::~ClangAstVisitor()
{
	delete trMngr_;
}

bool ClangAstVisitor::TraverseNamespaceDecl(clang::NamespaceDecl* namespaceDecl)
{
	OOModel::Module* ooModule = nullptr;
	// insert it in model
	if(OOModel::Project* curProject = dynamic_cast<OOModel::Project*>(ooStack_.top()))
	{
		ooModule = trMngr_->insertNamespace(namespaceDecl, ooStack_.size()-1);
		// if the module is not yet in the list add it
		if(!curProject->modules()->contains(ooModule))
			curProject->modules()->append(ooModule);
	}
	else if(OOModel::Module* curModel = dynamic_cast<OOModel::Module*>(ooStack_.top()))
	{
		ooModule = trMngr_->insertNamespace(namespaceDecl, ooStack_.size());
		// check if the namespace is already the one which is active
		if(curModel == ooModule)
			return true;
		// if the module is not yet in the list add it
		if(!curModel->modules()->contains(ooModule))
			curModel->modules()->append(ooModule);
	}
	else
	{
		log_->writeError(className_,QString("uknown where to put namespace"),
							  QString("NamespaceDecl"),namespaceDecl);
		// this is a severe error which should not happen therefore stop visiting
		return false;
	}
	// visit the body of the namespace
	ooStack_.push(ooModule);
	clang::DeclContext::decl_iterator it = namespaceDecl->decls_begin();
	for(;it!=namespaceDecl->decls_end();++it)
	{
		TraverseDecl(*it);
	}
	ooStack_.pop();
	return true;
}

bool ClangAstVisitor::TraverseCXXRecordDecl(clang::CXXRecordDecl* recordDecl)
{
	if(recordDecl->isClass() || recordDecl->isStruct())
	{
		// TODO might be better to handle structs and classes separately
		OOModel::Class* ooClass;
		ooClass = trMngr_->insertClass(recordDecl);
		if(!ooClass)
			return false;

		// insert in model
		if(OOModel::Project* curProject = dynamic_cast<OOModel::Project*>(ooStack_.top()))
			curProject->modules()->append(ooClass);
		else if(OOModel::Module* curModel = dynamic_cast<OOModel::Module*>(ooStack_.top()))
			curModel->modules()->append(ooClass);
		else if(OOModel::Class* curClass = dynamic_cast<OOModel::Class*>(ooStack_.top()))
			curClass->classes()->append(ooClass);
		else
			log_->writeError(className_,QString("uknown where to put class"),
								  QString("CXXRecordDecl"),recordDecl);
		// visit child decls
		ooStack_.push(ooClass);
		clang::DeclContext::decl_iterator it = recordDecl->decls_begin();
		for(;it!=recordDecl->decls_end();++it)
		{
			TraverseDecl(*it);
		}
		ooStack_.pop();

		// visit base classes
		// can only query base classes if there is a definition otherwise a clang assertion is failed
		if(recordDecl->isThisDeclarationADefinition())
		{
			for(auto base_itr = recordDecl->bases_begin(); base_itr!=recordDecl->bases_end(); ++base_itr)
			{
				if(auto baseClass = base_itr->getType().getTypePtr()->getAsCXXRecordDecl())
				{
					OOModel::ClassTypeExpression* parent = new OOModel::ClassTypeExpression();
					parent->typeExpression()->ref()->setName(QString::fromStdString(baseClass->getNameAsString()));
					ooClass->baseClasses()->append(parent);
				}
			}
		}

		// set visibility
		ooClass->setVisibility(CppImportUtilities::convertAccessSpecifier(recordDecl->getAccess()));
	}
	else if(recordDecl->isUnion())
	{
		log_->writeWarning(className_,QString("Unions not supported"),
								 QString("CXXRecordDecl"),recordDecl);
		return Base::TraverseCXXRecordDecl(recordDecl);
	}
	else
	{
		std::cout << "Neither class nor Struct " << recordDecl->getNameAsString() << std::endl;
		return Base::TraverseCXXRecordDecl(recordDecl);
	}
	return true;
}

bool ClangAstVisitor::TraverseCXXMethodDecl(clang::CXXMethodDecl* methodDecl)
{
	// Constructors not yet handled
	if(llvm::isa<clang::CXXConstructorDecl>(methodDecl))
		return true;
	OOModel::Method* ooMethod = trMngr_->insertMethodDecl(methodDecl);
	if(!ooMethod)
	{
		// TODO is this correct?
		// only consider a method where the parent has been visited
		if(trMngr_->containsClass(methodDecl->getParent()))
			log_->writeError(className_,QString("no ooModel::method found"),
								  QString("CXXMethodDecl"),methodDecl);
		return true;
	}
	// only visit the body if we are at the definition
	if(methodDecl->isThisDeclarationADefinition())
	{
		ooStack_.push(ooMethod->items());
		bool inBody = inBody_;
		inBody_ = true;
		TraverseStmt(methodDecl->getBody());
		inBody_ = inBody;
		ooStack_.pop();
	}
	// specify the visibility of the method
	ooMethod->setVisibility(CppImportUtilities::convertAccessSpecifier(methodDecl->getAccess()));
	return true;
}

bool ClangAstVisitor::TraverseFunctionDecl(clang::FunctionDecl* functionDecl)
{
	if(llvm::isa<clang::CXXMethodDecl>(functionDecl))
		// already handled
		return true;
	OOModel::Method* ooFunction = trMngr_->insertFunctionDecl(functionDecl);
	if(ooFunction)
	{
		if(ooFunction->parent())
		{
			// TODO find out why this happens
			std::cout << "FUNCTION HAS OOPARENT " << functionDecl->getNameAsString() << std::endl;
			return true;
		}
		// insert in model
		if(OOModel::Project* curProject = dynamic_cast<OOModel::Project*>(ooStack_.top()))
			curProject->methods()->append(ooFunction);
		else if(OOModel::Module* curModel = dynamic_cast<OOModel::Module*>(ooStack_.top()))
			curModel->methods()->append(ooFunction);
		else if(OOModel::Class* curClass = dynamic_cast<OOModel::Class*>(ooStack_.top()))
		{
			// function Probably is a friend of class
			std::cout<< "FRIEND FUNCTION ??? CLASS ON TOP OF STACK " << curClass->name().toStdString() <<
							" Function name: " << functionDecl->getNameAsString() << std::endl;

		}
		else
			log_->writeError(className_,QString("uknown where to put function"),
								  QString("FunctionDecl"),functionDecl);

		// only visit the body if we are at the definition
		if(functionDecl->isThisDeclarationADefinition())
		{
			ooStack_.push(ooFunction->items());
			bool inBody = inBody_;
			inBody_ = true;
			if(auto body = functionDecl->getBody())
				TraverseStmt(body);
			inBody_ = inBody;
			ooStack_.pop();
		}
	}
	else
	{
		log_->writeError(className_,QString("could no insert function"),
							  QString("FunctionDecl"),functionDecl);
	}
	return true;
}

bool ClangAstVisitor::TraverseIfStmt(clang::IfStmt* ifStmt)
{
	OOModel::IfStatement* ooIfStmt = new OOModel::IfStatement();
	OOModel::StatementItemList* itemList = dynamic_cast<OOModel::StatementItemList*>(ooStack_.top());
	if(itemList)
	{
		// append the if stmt to current stmt list
		itemList->append(ooIfStmt);
		// condition
		bool inBody = inBody_;
		inBody_ = false;
		TraverseStmt(ifStmt->getCond());
		inBody_ = true;
		ooIfStmt->setCondition(ooExprStack_.pop());
		// then branch
		ooStack_.push(ooIfStmt->thenBranch());
		TraverseStmt(ifStmt->getThen());
		ooStack_.pop();
		// else branch
		ooStack_.push(ooIfStmt->elseBranch());
		TraverseStmt(ifStmt->getElse());
		inBody_ = inBody;
		ooStack_.pop();
	}
	return true;
}

bool ClangAstVisitor::TraverseWhileStmt(clang::WhileStmt* whileStmt)
{
	OOModel::LoopStatement* ooLoop = new OOModel::LoopStatement();
	OOModel::StatementItemList* itemList = dynamic_cast<OOModel::StatementItemList*>(ooStack_.top());
	if(itemList)
	{
		// append the loop to current stmt list
		itemList->append(ooLoop);
		// condition
		bool inBody = inBody_;
		inBody_ = false;
		TraverseStmt(whileStmt->getCond());
		inBody_ = true;
		ooLoop->setCondition(ooExprStack_.pop());
		// body
		ooStack_.push(ooLoop->body());
		TraverseStmt(whileStmt->getBody());
		inBody_ = inBody;
		ooStack_.pop();
	}
	return true;
}

bool ClangAstVisitor::TraverseForStmt(clang::ForStmt* forStmt)
{
	OOModel::LoopStatement* ooLoop = new OOModel::LoopStatement();
	OOModel::StatementItemList* itemList = dynamic_cast<OOModel::StatementItemList*>(ooStack_.top());
	if(itemList)
	{
		itemList->append(ooLoop);
		bool inBody = inBody_;
		inBody_ = false;
		// init
		TraverseStmt(forStmt->getInit());
		ooLoop->setInitStep(ooExprStack_.pop());
		// condition
		TraverseStmt(forStmt->getCond());
		ooLoop->setCondition(ooExprStack_.pop());
		// update
		TraverseStmt(forStmt->getInc());
		ooLoop->setUpdateStep(ooExprStack_.pop());
		inBody_ = true;
		// body
		ooStack_.push(ooLoop->body());
		TraverseStmt(forStmt->getBody());
		inBody_ = inBody;
		ooStack_.pop();
	}
	return true;
}

bool ClangAstVisitor::TraverseReturnStmt(clang::ReturnStmt* returnStmt)
{
	OOModel::ReturnStatement* ooReturn = new OOModel::ReturnStatement();
	OOModel::StatementItemList* itemList = dynamic_cast<OOModel::StatementItemList*>(ooStack_.top());
	if(itemList)
	{
		itemList->append(ooReturn);
		// return expression
		bool inBody = inBody_;
		inBody_ = false;
		TraverseStmt(returnStmt->getRetValue());
		if(!ooExprStack_.empty())
			ooReturn->values()->append(ooExprStack_.pop());
		else
			log_->writeError(className_,QString("Return expr not supported"),
								  QString("Expr"),returnStmt->getRetValue());
		inBody_ = inBody;
	}
	return true;
}

bool ClangAstVisitor::TraverseStmt(clang::Stmt* S)
{
	return Base::TraverseStmt(S);
}

bool ClangAstVisitor::VisitStmt(clang::Stmt* S)
{
	//    std::cout << "VISITING STMT" << std::endl;
	//    llvm::errs() << "VISITING STMT" << "\n";
	//			  S->dump();
	return Base::VisitStmt(S);
}

bool ClangAstVisitor::TraverseVarDecl(clang::VarDecl* varDecl)
{
	//    std::cout << "Visiting VarDecl " << vd->getNameAsString() <<std::endl;

	OOModel::StatementItemList* itemList = dynamic_cast<OOModel::StatementItemList*>(ooStack_.top());
	if(itemList)
	{
		OOModel::VariableDeclaration* ooVarDecl = trMngr_->insertVar(varDecl);
		if(varDecl->getType().getTypePtr()->isArrayType())
		{
			//TODO this array section is very nasty
			const clang::ArrayType* arrType =  varDecl->getType().getTypePtr()->getAsArrayTypeUnsafe();
			if(arrType)
			{
				if(llvm::isa<clang::ConstantArrayType>(arrType))
				{
					const clang::ConstantArrayType* constArr = llvm::dyn_cast<clang::ConstantArrayType>(arrType);
					std::cout << "Const Array Size: " << constArr->getSize().getLimitedValue() << std::endl;
					OOModel::ArrayTypeExpression* varType = new OOModel::ArrayTypeExpression();
					if(OOModel::Expression* expr = CppImportUtilities::convertClangType(arrType->getElementType()))
						varType->setTypeExpression(expr);
					ooVarDecl->setVarType(varType);
				}
			}
		}
		else
		{
			OOModel::Expression* type = CppImportUtilities::convertClangType(varDecl->getType());
			if(type) ooVarDecl->setVarType(type);
		}

		if(varDecl->hasInit())
		{
			TraverseStmt(varDecl->getInit());
			if(!ooExprStack_.empty())
				ooVarDecl->setInitialValue(ooExprStack_.pop());
			else
				log_->writeError(className_,QString("Var Init Expr no supported"),
									  QString("Expr"),varDecl->getInit());
		}
		if(inBody_)
			itemList->append(ooVarDecl);
		else
			ooExprStack_.push(ooVarDecl);
	}
	else
	{
		if(!llvm::isa<clang::ParmVarDecl>(varDecl))
			log_->writeWarning(className_,QString("this variable is not supported"),
									 QString("VarDecl"),varDecl);
	}
	return true;
}

bool ClangAstVisitor::TraverseEnumDecl(clang::EnumDecl* enumDecl)
{
	OOModel::Class* ooEnumClass = new OOModel::Class(QString::fromStdString(enumDecl->getNameAsString()));
	// insert in model
	if(OOModel::Project* curProject = dynamic_cast<OOModel::Project*>(ooStack_.top()))
		curProject->classes()->append(ooEnumClass);
	else if(OOModel::Module* curModel = dynamic_cast<OOModel::Module*>(ooStack_.top()))
		curModel->classes()->append(ooEnumClass);
	else if(OOModel::Class* curClass = dynamic_cast<OOModel::Class*>(ooStack_.top()))
		curClass->classes()->append(ooEnumClass);
	else
	{
		log_->writeWarning(className_,QString("Enums are only supported global, in namespaces or in classes"),
								 QString("EnumDecl"),enumDecl);
		// no need to further process this enum
		return true;
	}
	clang::EnumDecl::enumerator_iterator it = enumDecl->enumerator_begin();
	bool inBody = inBody_;
	inBody_ = false;
	for(;it!=enumDecl->enumerator_end();++it)
	{
		// check if there is an initializing expression if so visit it first and then add it to the enum
		if(auto e = it->getInitExpr())
		{
			TraverseStmt(e);
			ooEnumClass->enumerators()->append(new OOModel::Enumerator
														  (QString::fromStdString(it->getNameAsString()),ooExprStack_.pop()));
		}
		else
			ooEnumClass->enumerators()->append(new OOModel::Enumerator(QString::fromStdString(it->getNameAsString())));
	}
	inBody_ = inBody;
	return true;
}

bool ClangAstVisitor::VisitFieldDecl(clang::FieldDecl* fieldDecl)
{
	OOModel::Field* field = trMngr_->insertField(fieldDecl);
	if(!field)
	{
		log_->writeError(className_,QString("no parent found for this field"),
							  QString("FieldDecl"),fieldDecl);
		// TODO
		// return false;
		return true;
	}
	clang::QualType ctype = fieldDecl->getType();
	OOModel::Expression* type = CppImportUtilities::convertClangType(ctype);
	if(type) field->setTypeExpression(type);
	field->setVisibility(CppImportUtilities::convertAccessSpecifier(fieldDecl->getAccess()));
	return true;
}

bool ClangAstVisitor::TraverseCXXMemberCallExpr(clang::CXXMemberCallExpr* callExpr)
{
	OOModel::MethodCallExpression* ooMCall = new OOModel::MethodCallExpression
			(QString::fromStdString(callExpr->getMethodDecl()->getNameAsString()));

	// visit arguments
	bool inBody = inBody_;
	inBody_ = false;
	clang::ExprIterator argIt = callExpr->arg_begin();
	for(;argIt!=callExpr->arg_end();++argIt)
	{
		TraverseStmt(*argIt);
		ooMCall->arguments()->append(ooExprStack_.pop());
	}

	// set target
	// TODO multiple levels possible
	if(auto callee = llvm::dyn_cast<clang::MemberExpr>(callExpr->getCallee()))
	{
		TraverseStmt(callee->getBase());
		if(!ooExprStack_.empty())
			ooMCall->ref()->setPrefix(ooExprStack_.pop());
	}

	// set the correct parent
	inBody_ = inBody;
	if(inBody_)
	{
		OOModel::StatementItemList* itemList = dynamic_cast<OOModel::StatementItemList*>(ooStack_.top());
		if(itemList) itemList->append(ooMCall);
	}
	else
		ooExprStack_.push(ooMCall);
	return true;
}

bool ClangAstVisitor::VisitIntegerLiteral(clang::IntegerLiteral* intLit)
{
	OOModel::IntegerLiteral* ooIntLit = new OOModel::IntegerLiteral();
	ooIntLit->setValue(intLit->getValue().getLimitedValue());
	ooExprStack_.push(ooIntLit);
	return true;
}

bool ClangAstVisitor::VisitCXXBoolLiteralExpr(clang::CXXBoolLiteralExpr* boolLitExpr)
{
	OOModel::BooleanLiteral* ooBoolLit = new OOModel::BooleanLiteral();
	ooBoolLit->setValue(boolLitExpr->getValue());
	ooExprStack_.push(ooBoolLit);
	return true;
}

bool ClangAstVisitor::VisitFloatingLiteral(clang::FloatingLiteral* floatLiteral)
{
	OOModel::FloatLiteral* ooFloatLit = new OOModel::FloatLiteral();
	ooFloatLit->setValue(floatLiteral->getValueAsApproximateDouble());
	ooExprStack_.push(ooFloatLit);
	return true;
}

bool ClangAstVisitor::VisitDeclRefExpr(clang::DeclRefExpr* declRefExpr)
{
	OOModel::ReferenceExpression* refExpr = new OOModel::ReferenceExpression();
	refExpr->setName(QString::fromStdString(declRefExpr->getNameInfo().getName().getAsString()));
	ooExprStack_.push(refExpr);
	return true;
}

bool ClangAstVisitor::VisitCXXUnresolvedConstructorExpr(clang::CXXUnresolvedConstructExpr* unresolvedConstructorExpr)
{
	unresolvedConstructorExpr->getBitField();
	ooExprStack_.push(new OOModel::Expression());
	return true;
}

bool ClangAstVisitor::TraverseParenExpr(clang::ParenExpr* parenthesizedExpr)
{
	OOModel::UnaryOperation* ooParenExpr = new OOModel::UnaryOperation();
	ooParenExpr->setOp(OOModel::UnaryOperation::PARENTHESIS);
	// save inBody_ value for recursive expressions
	bool inBody = inBody_;
	inBody_ = false;
	TraverseStmt(parenthesizedExpr->getSubExpr());
	ooParenExpr->setOperand(ooExprStack_.pop());

	if(inBody)
	{
		OOModel::StatementItemList* itemList = dynamic_cast<OOModel::StatementItemList*>(ooStack_.top());
		if(itemList) itemList->append(ooParenExpr);
		else std::cout << "ERROR INSERT Parenthesized Expression" << std::endl;
	}
	else
		ooExprStack_.push(ooParenExpr);
	inBody_ = inBody;
	return true;
}

bool ClangAstVisitor::VisitCXXThisExpr(clang::CXXThisExpr* thisExpr)
{
	if(!thisExpr->isImplicit())
	{
		if(inBody_)
		{
			// TODO ..
		}
		else
		{
			ooExprStack_.push(new OOModel::ThisExpression());
		}
	}
	return true;
}

bool ClangAstVisitor::VisitMemberExpr(clang::MemberExpr* memberExpr)
{
	if(memberExpr->isImplicitAccess())
	{
		OOModel::ReferenceExpression* ooRef = new OOModel::ReferenceExpression();
		ooRef->ref()->setName(QString::fromStdString(memberExpr->getMemberDecl()->getNameAsString()));
		ooExprStack_.push(ooRef);
	}
	return true;
}

bool ClangAstVisitor::VisitBreakStmt(clang::BreakStmt* breakStmt)
{
	OOModel::BreakStatement* ooBreak = new OOModel::BreakStatement();
	OOModel::StatementItemList* itemList = dynamic_cast<OOModel::StatementItemList*>(ooStack_.top());
	if(itemList) itemList->append(ooBreak);
	else
		log_->writeError(className_,QString("not able to put break stmt"),
							  QString("BreakStmt"),breakStmt);
	return true;
}

bool ClangAstVisitor::VisitContinueStmt(clang::ContinueStmt* continueStmt)
{
	OOModel::ContinueStatement* ooContinue = new OOModel::ContinueStatement();
	OOModel::StatementItemList* itemList = dynamic_cast<OOModel::StatementItemList*>(ooStack_.top());
	if(itemList) itemList->append(ooContinue);
	else
		log_->writeError(className_,QString("not able to put continue stmt"),
							  QString("ContinueStmt"),continueStmt);
	return true;
}

bool ClangAstVisitor::shouldUseDataRecursionFor(clang::Stmt* S)
{
	Q_UNUSED(S)
	return false;
}

bool ClangAstVisitor::TraverseBinaryOp(clang::BinaryOperator* binaryOperator)
{
	OOModel::BinaryOperation::OperatorTypes ooOperatorType =
			CppImportUtilities::convertClangOpcode(binaryOperator->getOpcode());
	OOModel::BinaryOperation* ooBinOp = new OOModel::BinaryOperation();
	// save inBody_ value for recursive expressions
	bool inBody = inBody_;
	inBody_ = false;
	ooBinOp->setOp(ooOperatorType);
	TraverseStmt(binaryOperator->getLHS());
	if(!ooExprStack_.empty())
		ooBinOp->setLeft(ooExprStack_.pop());
	else
		log_->writeError(className_,QString("BOP: LHSExpr not supported"),
							  QString("Expr"),binaryOperator->getLHS());
	TraverseStmt(binaryOperator->getRHS());
	if(!ooExprStack_.empty())
		ooBinOp->setRight(ooExprStack_.pop());
	else
		log_->writeError(className_,QString("BOP: RHSExpr not supported"),
							  QString("Expr"),binaryOperator->getRHS());

	if(inBody)
	{
		OOModel::StatementItemList* itemList = dynamic_cast<OOModel::StatementItemList*>(ooStack_.top());
		if(itemList) itemList->append(ooBinOp);
		else std::cout << "ERROR INSERT BINOP" << std::endl;
	}
	else
		ooExprStack_.push(ooBinOp);
	inBody_ = inBody;
	return true;
}

bool ClangAstVisitor::TraverseAssignment(clang::BinaryOperator* binaryOperator)
{
	OOModel::AssignmentExpression::AssignmentTypes ooOperatorType =
			CppImportUtilities::convertClangAssignOpcode(binaryOperator->getOpcode());
	OOModel::AssignmentExpression* ooBinOp = new OOModel::AssignmentExpression();
	// save inBody_ value for recursive expressions
	bool inBody = inBody_;
	inBody_ = false;
	ooBinOp->setOp(ooOperatorType);
	TraverseStmt(binaryOperator->getLHS());
	if(!ooExprStack_.empty())
		ooBinOp->setLeft(ooExprStack_.pop());
	else
		log_->writeError(className_,QString("BOP: LHSExpr not supported"),
							  QString("Expr"),binaryOperator->getLHS());
	TraverseStmt(binaryOperator->getRHS());
	if(!ooExprStack_.empty())
		ooBinOp->setRight(ooExprStack_.pop());
	else
		log_->writeError(className_,QString("BOP: RHSExpr not supported"),
							  QString("Expr"),binaryOperator->getRHS());

	if(inBody)
	{
		OOModel::StatementItemList* itemList = dynamic_cast<OOModel::StatementItemList*>(ooStack_.top());
		if(itemList) itemList->append(ooBinOp);
		else std::cout << "ERROR INSERT BINOP" << std::endl;
	}
	else
		ooExprStack_.push(ooBinOp);
	inBody_ = inBody;
	return true;
}

bool ClangAstVisitor::TraverseUnaryOp(clang::UnaryOperator* unaryOperator)
{
	OOModel::UnaryOperation::OperatorTypes ooOperatorType =
			CppImportUtilities::convertUnaryOpcode(unaryOperator->getOpcode());
	OOModel::UnaryOperation* ooUnaryOp = new OOModel::UnaryOperation();
	// save inBody_ value for recursive expressions
	bool inBody = inBody_;
	inBody_ = false;
	ooUnaryOp->setOp(ooOperatorType);
	TraverseStmt(unaryOperator->getSubExpr());
	if(!ooExprStack_.empty())
		ooUnaryOp->setOperand(ooExprStack_.pop());
	else
		log_->writeError(className_,QString("UOP: SubExpr not supported"),
							  QString("Expr"),unaryOperator->getSubExpr());

	if(inBody)
	{
		OOModel::StatementItemList* itemList = dynamic_cast<OOModel::StatementItemList*>(ooStack_.top());
		if(itemList) itemList->append(ooUnaryOp);
		else std::cout << "ERROR INSERT Unary" << std::endl;
	}
	else
		ooExprStack_.push(ooUnaryOp);
	inBody_ = inBody;
	return true;
}

} // namespace cppimport
