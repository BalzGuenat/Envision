TARGET = oovisualization
include(../Core/common_plugin.pri)
DEFINES += OOVISUALIZATION_LIBRARY
win32:LIBS += -llogger \
    -lselftest \
    -lmodelbase \
    -loomodel \
    -lvisualizationbase \
    -linteractionbase
HEADERS += src/elements/VModifier.h \
    src/declarations/VNameImportStyle.h \
    src/declarations/VNameImport.h \
    src/expressions/types/VFunctionType.h \
    src/expressions/types/VFunctionTypeStyle.h \
    src/expressions/types/VAutoType.h \
    src/declarations/VFieldStyle.h \
    src/declarations/VField.h \
    src/expressions/VVariableDeclarationExpressionStyle.h \
    src/expressions/VVariableDeclarationExpression.h \
    src/declarations/VClass.h \
    src/declarations/VClassStyle.h \
    src/declarations/VMethod.h \
    src/declarations/VMethodStyle.h \
    src/declarations/VModule.h \
    src/declarations/VModuleStyle.h \
    src/declarations/VProject.h \
    src/declarations/VProjectStyle.h \
    src/expressions/allOOExpressionVisualizations.h \
    src/expressions/literals/VBooleanLiteral.h \
    src/expressions/literals/VCharacterLiteral.h \
    src/expressions/literals/VFloatLiteral.h \
    src/expressions/literals/VIntegerLiteral.h \
    src/expressions/literals/VNullLiteral.h \
    src/expressions/literals/VStringLiteral.h \
    src/expressions/literals/VStringLiteralStyle.h \
    src/expressions/types/VClassType.h \
    src/expressions/types/VPrimitiveType.h \
    src/expressions/StandardExpressionDefinitions.h \
    src/expressions/StandardExpressionVisualizations.h \
    src/elements/VCatchClauseStyle.h \
    src/expressions/VExpression.h \
    src/elements/VCatchClause.h \
    src/statements/VStatementItem.h \
    src/elements/VEnumerator.h \
    src/elements/VEnumeratorStyle.h \
    src/alternative/VKeywordMethodCallStyle.h \
    src/alternative/VKeywordMethodCall.h \
    src/expressions/VLambdaExpressionStyle.h \
    src/expressions/VLambdaExpression.h \
    src/statements/VTryCatchFinallyStyle.h \
    src/statements/VTryCatchFinally.h \
    test/MethodAddOn.h \
    src/expressions/VEmptyExpressionStyle.h \
    src/elements/VStatementItemList.h \
    src/elements/VFormalTypeArgumentStyle.h \
    src/elements/VFormalTypeArgument.h \
    src/OOVisualizationException.h \
    src/allOOVisualizations.h \
    src/elements/VFormalArgument.h \
    src/elements/VFormalArgumentStyle.h \
    src/elements/VFormalResult.h \
    src/elements/VFormalResultStyle.h \
    src/expressions/OperatorStyle.h \
    src/expressions/VArrayInitializer.h \
    src/expressions/VArrayInitializerStyle.h \
    src/expressions/VBinaryOperation.h \
    src/expressions/VEmptyExpression.h \
    src/expressions/VErrorExpression.h \
    src/expressions/VErrorExpressionStyle.h \
    src/expressions/VMethodCallExpression.h \
    src/expressions/VMethodCallExpressionStyle.h \
    src/expressions/VReferenceExpression.h \
    src/expressions/VReferenceExpressionStyle.h \
    src/expressions/VThisExpression.h \
    src/expressions/VUnfinishedOperator.h \
    src/expressions/VUnfinishedOperatorStyle.h \
    src/oovisualization_api.h \
    src/precompiled.h \
    src/statements/VBlock.h \
    src/statements/VBlockStyle.h \
    src/statements/VBreakStatement.h \
    src/statements/VContinueStatement.h \
    src/statements/VExpressionStatement.h \
    src/statements/VForEachStatement.h \
    src/statements/VForEachStatementStyle.h \
    src/statements/VIfStatement.h \
    src/statements/VIfStatementStyle.h \
    src/statements/VLoopStatement.h \
    src/statements/VLoopStatementStyle.h \
    src/statements/VReturnStatement.h \
    src/statements/VReturnStatementStyle.h \
    src/oovisualization.h
SOURCES += src/elements/VModifier.cpp \
    src/declarations/VNameImportStyle.cpp \
    src/declarations/VNameImport.cpp \
    src/expressions/types/VFunctionType.cpp \
    src/expressions/types/VFunctionTypeStyle.cpp \
    src/expressions/types/VAutoType.cpp \
    src/declarations/VFieldStyle.cpp \
    src/declarations/VField.cpp \
    src/expressions/VVariableDeclarationExpressionStyle.cpp \
    src/expressions/VVariableDeclarationExpression.cpp \
    src/declarations/VClass.cpp \
    src/declarations/VClassStyle.cpp \
    src/declarations/VMethod.cpp \
    src/declarations/VMethodStyle.cpp \
    src/declarations/VModule.cpp \
    src/declarations/VModuleStyle.cpp \
    src/declarations/VProject.cpp \
    src/declarations/VProjectStyle.cpp \
    src/expressions/literals/VBooleanLiteral.cpp \
    src/expressions/literals/VCharacterLiteral.cpp \
    src/expressions/literals/VFloatLiteral.cpp \
    src/expressions/literals/VIntegerLiteral.cpp \
    src/expressions/literals/VNullLiteral.cpp \
    src/expressions/literals/VStringLiteral.cpp \
    src/expressions/literals/VStringLiteralStyle.cpp \
    src/expressions/types/VClassType.cpp \
    src/expressions/types/VPrimitiveType.cpp \
    src/expressions/StandardExpressionVisualizations.cpp \
    src/elements/VCatchClauseStyle.cpp \
    src/expressions/VExpression.cpp \
    src/elements/VCatchClause.cpp \
    src/statements/VStatementItem.cpp \
    src/elements/VEnumerator.cpp \
    src/elements/VEnumeratorStyle.cpp \
    src/alternative/VKeywordMethodCallStyle.cpp \
    src/alternative/VKeywordMethodCall.cpp \
    src/expressions/VLambdaExpressionStyle.cpp \
    src/expressions/VLambdaExpression.cpp \
    src/statements/VTryCatchFinallyStyle.cpp \
    src/statements/VTryCatchFinally.cpp \
    test/MethodAddOn.cpp \
    src/expressions/VEmptyExpressionStyle.cpp \
    src/elements/VStatementItemList.cpp \
    src/elements/VFormalTypeArgumentStyle.cpp \
    src/elements/VFormalTypeArgument.cpp \
    src/expressions/VMethodCallExpressionStyle.cpp \
    src/statements/VExpressionStatement.cpp \
    src/expressions/VEmptyExpression.cpp \
    src/expressions/VUnfinishedOperatorStyle.cpp \
    src/expressions/VUnfinishedOperator.cpp \
    src/expressions/VErrorExpressionStyle.cpp \
    src/expressions/VErrorExpression.cpp \
    src/expressions/VArrayInitializer.cpp \
    src/expressions/VArrayInitializerStyle.cpp \
    src/statements/VContinueStatement.cpp \
    src/statements/VBreakStatement.cpp \
    src/statements/VForEachStatement.cpp \
    src/statements/VForEachStatementStyle.cpp \
    src/statements/VLoopStatement.cpp \
    src/statements/VLoopStatementStyle.cpp \
    src/elements/VFormalArgument.cpp \
    src/elements/VFormalArgumentStyle.cpp \
    src/elements/VFormalResult.cpp \
    src/elements/VFormalResultStyle.cpp \
    src/statements/VIfStatement.cpp \
    src/statements/VIfStatementStyle.cpp \
    src/statements/VReturnStatement.cpp \
    src/statements/VReturnStatementStyle.cpp \
    src/statements/VBlock.cpp \
    src/statements/VBlockStyle.cpp \
    src/expressions/VBinaryOperation.cpp \
    src/expressions/OperatorStyle.cpp \
    src/expressions/VThisExpression.cpp \
    src/expressions/VMethodCallExpression.cpp \
    src/expressions/VReferenceExpression.cpp \
    src/expressions/VReferenceExpressionStyle.cpp \
    test/HelloWorldTest.cpp \
    src/OOVisualizationException.cpp \
    src/oovisualization.cpp
