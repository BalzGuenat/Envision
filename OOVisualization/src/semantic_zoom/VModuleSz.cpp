/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
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

#include "VModuleSz.h"

#include "VisualizationBase/src/layouts/PanelBorderLayout.h"
#include "VisualizationBase/src/layouts/PositionLayout.h"
#include "VisualizationBase/src/items/VText.h"
#include "VisualizationBase/src/items/Static.h"
#include "VisualizationBase/src/items/VList.h"

using namespace Visualization;
using namespace OOModel;

namespace OOVisualization {

ITEM_COMMON_DEFINITIONS(VModuleSz, "item")

VModuleSz::VModuleSz(Item* parent, NodeType* node, const StyleType* style) : Super(parent, node, style)
{
	layout()->setTop(true);
	header = new SequentialLayout(layout()->top(), &style->header());
	layout()->top()->setMiddle(header);

	//header->append(new Static(header, &style->icon()));
	name = new VText(header, node->nameNode(), &style->name());

	setDefaultMoveCursorProxy(name);

	content_ = new SequentialLayout(layout(), &style->content());
	layout()->setContent(content_);

	content_->append(name);

	body_ = new PositionLayout(content_, &style->body());
	content_->append(body_);
}

VModuleSz::~VModuleSz()
{
	// These were automatically deleted by LayoutProvider's destructor
	header = nullptr;
	name = nullptr;
	body_ = nullptr;
	content_ = nullptr;
	fields_ = nullptr;
	declarations_ = nullptr;
	libraries_ = nullptr;
}

void VModuleSz::determineChildren()
{
	// TODO: find a better way and place to determine the style of children. Is doing this causing too many updates?
	// TODO: consider the performance of this. Possibly introduce a style updated boolean for all items so that they know
	//			what's the reason they are being updated.
	// The style needs to be updated every time since if our own style changes, so will that of the children.
	layout()->setStyle(&style()->layout());
	header->setStyle(&style()->header());
	name->setStyle(&style()->name());
	body_->setStyle( &style()->body() );
	content_->setStyle(&style()->content());
	//header->at<Static>(0)->setStyle(&style()->icon());

	header->synchronizeLast(name, node()->nameNode(), &style()->name());

	QList<Model::Node*> bodyItems = node()->modules()->nodes().toList();
	//bodyItems << node()->classes()->nodes().toList();
	//bodyItems << node()->methods()->nodes().toList();
	body_->synchronizeWithNodes(bodyItems, renderer());

	content_->synchronizeFirst(libraries_, node()->libraries(), &style()->libraries());
	auto fieldsIndex = node()->subDeclarations()->size() > 0 ? 2 : 1;
	content_->synchronizeMid(declarations_, fieldsIndex > 1 ? node()->subDeclarations() :nullptr,
		&style()->declarations(), 1);
	content_->synchronizeMid(fields_, node()->fields()->size() > 0 ? node()->fields() : nullptr, &style()->fields(),
			fieldsIndex);
}

}
