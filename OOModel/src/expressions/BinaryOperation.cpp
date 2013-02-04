/***********************************************************************************************************************
**
** Copyright (c) 2011, 2013 ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
**    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
**      disclaimer.
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
***********************************************************************************************************************/

/***********************************************************************************************************************
 * BinaryOperation.cpp
 *
 *  Created on: Jan 31, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "expressions/BinaryOperation.h"
#include "../types/PrimitiveType.h"
#include "../types/ErrorType.h"

namespace OOModel {

EXTENDABLENODE_DEFINE_EMPTY_CONSTRUCTORS(BinaryOperation, Expression)
EXTENDABLENODE_DEFINE_TYPE_REGISTRATION_METHODS(BinaryOperation, Expression)

REGISTER_ATTRIBUTE(BinaryOperation, left, Expression, false, false, true)
REGISTER_ATTRIBUTE(BinaryOperation, right, Expression, false, false, true)
REGISTER_ATTRIBUTE(BinaryOperation, opr, Integer, false, false, true)

BinaryOperation::BinaryOperation(OperatorTypes op, Expression* left, Expression* right)
: Expression (nullptr, BinaryOperation::getMetaData())
{
	setOp(op);
	if (left) setLeft(left);
	if (right) setRight(right);
}

Type* BinaryOperation::type()
{
	auto lt = left()->type();
	auto rt = right()->type();

	auto ltp = dynamic_cast<PrimitiveType*>(lt);
	auto rtp = dynamic_cast<PrimitiveType*>(rt);

	Type* res = nullptr;
	if (ltp && rtp)
	{
		PrimitiveType::PrimitiveTypes primitive = PrimitiveType::resultFromBinaryOperation(ltp->type(), rtp->type());
		if (primitive != PrimitiveType::VOID)
		res = new PrimitiveType(primitive, ltp->isValueType());
	}

	if (!res) res = new ErrorType("Incompatible types for binary operation");

	SAFE_DELETE(lt);
	SAFE_DELETE(rt);

	return res;
}

}
