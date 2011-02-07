/***********************************************************************************************************************
 * oovisualization.cpp
 *
 *  Created on: Feb 01, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "oovisualization.h"
#include "SelfTest/headers/SelfTestSuite.h"

#include "vis/top_level/VProject.h"
#include "vis/top_level/VModule.h"
#include "vis/top_level/VClass.h"
#include "vis/top_level/VMethod.h"
#include "vis/elements/VField.h"
#include "vis/VOOReference.h"

#include "OOModel/headers/Project.h"
#include "OOModel/headers/Module.h"
#include "OOModel/headers/Class.h"
#include "OOModel/headers/Method.h"
#include "OOModel/headers/Field.h"

#include "VisualizationBase/headers/Scene.h"
#include "VisualizationBase/headers/node_extensions/Position.h"

#include "InteractionBase/headers/handlers/GenericHandler.h"
#include "InteractionBase/headers/handlers/HText.h"

Q_EXPORT_PLUGIN2( oovisualization, OOVisualization::OOVisualization )

using namespace OOModel;
using namespace Visualization;

namespace OOVisualization {

bool OOVisualization::initialize(Envision::EnvisionManager&)
{
	// Register extensions
	Project::registerNewExtension<Position>();
	Module::registerNewExtension<Position>();
	Class::registerNewExtension<Position>();
	Method::registerNewExtension<Position>();

	// Register visualizations
	Scene::defaultRenderer()->registerVisualization(Project::typeIdStatic(), createVisualization<VProject, Project>);
	Scene::defaultRenderer()->registerVisualization(Module::typeIdStatic(), createVisualization<VModule, Module>);
	Scene::defaultRenderer()->registerVisualization(Class::typeIdStatic(), createVisualization<VClass, Class>);
	Scene::defaultRenderer()->registerVisualization(Method::typeIdStatic(), createVisualization<VMethod, Method>);
	Scene::defaultRenderer()->registerVisualization(Field::typeIdStatic(), createVisualization<VField, Field>);

	// Register handlers
	// TODO: move this to a better place i.e. OOInteraction Plugin
	// TODO: when you do that remove the dependency in the .plugin meta file
	VProject::setInteractionHandler(Interaction::GenericHandler::instance());
	VModule::setInteractionHandler(Interaction::GenericHandler::instance());
	VClass::setInteractionHandler(Interaction::GenericHandler::instance());
	VMethod::setInteractionHandler(Interaction::GenericHandler::instance());
	VField::setInteractionHandler(Interaction::GenericHandler::instance());
	VOOReference::setInteractionHandler(Interaction::HText::instance());

	return true;
}

void OOVisualization::selfTest(QString testid)
{
	if (testid.isEmpty()) SelfTest::TestManager<OOVisualization>::runAllTests().printResultStatistics();
	else SelfTest::TestManager<OOVisualization>::runTest(testid).printResultStatistics();
}

}