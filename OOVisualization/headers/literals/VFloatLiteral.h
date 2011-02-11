/***********************************************************************************************************************
 * VFloatLiteral.h
 *
 *  Created on: Feb 11, 2011
 *      Author: Dimitar Asenov
 **********************************************************************************************************************/

#ifndef VFLOATLITERAL_H_
#define VFLOATLITERAL_H_

#include "../oovisualization_api.h"

#include "VisualizationBase/headers/items/ModelItem.h"
#include "VisualizationBase/headers/items/TextStyle.h"
#include "VisualizationBase/headers/Styles.h"

#include "OOModel/headers/expressions/FloatLiteral.h"

namespace OOVisualization {

class OOVISUALIZATION_API VFloatLiteral : public Visualization::ModelItem
{
	ITEM_COMMON_CUSTOM_STYLENAME(VFloatLiteral, Visualization::ModelItem, Visualization::TextStyle)

	public:
		VFloatLiteral(Item* parent, OOModel::FloatLiteral *literal, const Visualization::TextStyle *style = Visualization::Styles::item<VFloatLiteral>("default"));
		virtual ~VFloatLiteral();

		virtual bool focusChild(FocusTarget location);

	protected:
		virtual void determineChildren();
		virtual void updateGeometry(int availableWidth, int availableHeight);

	private:
		Visualization::ModelItem* vis_;
};

}

#endif /* VFLOATLITERAL_H_ */