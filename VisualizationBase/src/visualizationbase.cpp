/***********************************************************************************************************************
 * visualizationbase.cpp
 *
 *  Created on: Dec 6, 2010
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#include "visualizationbase.h"

#include "VisualizationManager.h"
#include "SelfTest/headers/SelfTestSuite.h"

Q_EXPORT_PLUGIN2( visualizationbase, Visualization::VisualizationBase )

namespace Visualization
{

	bool VisualizationBase::initialize(Envision::EnvisionManager& manager)
	{
		VisualizationManager::init(&manager);
		return true;
	}

	void VisualizationBase::selfTest(QString)
	{
		SelfTest::TestManager<VisualizationBase>::runAllTests().printResultStatistics();
	}

}
