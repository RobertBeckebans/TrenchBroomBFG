/*
 Copyright (C) 2010-2017 Kristian Duske
 Copyright (C) 2022 Robert Beckebans

 This file is part of TrenchBroom.

 TrenchBroom is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 TrenchBroom is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with TrenchBroom. If not, see <http://www.gnu.org/licenses/>.
 */

#include "BrushEntityWithoutModelKeyIssueGenerator.h"

#include "Assets/EntityDefinition.h"
#include "Ensure.h"
#include "Model/BrushNode.h"
#include "Model/Entity.h"
#include "Model/EntityNode.h"
#include "Model/Issue.h"
#include "Model/IssueQuickFix.h"
#include "Model/MapFacade.h"
#include "Model/PushSelection.h"

#include <kdl/vector_utils.h>

#include <map>
#include <vector>

namespace TrenchBroom
{
namespace Model
{
class BrushEntityWithoutModelKeyIssueGenerator::BrushEntityWithoutModelKeyIssue
  : public Issue
{
public:
  static const IssueType Type;

public:
  explicit BrushEntityWithoutModelKeyIssue(EntityNode* entity)
    : Issue(entity)
  {
  }

private:
  IssueType doGetType() const override { return Type; }

  std::string doGetDescription() const override
  {
    const EntityNode* entity = static_cast<EntityNode*>(node());
    return entity->name() + " is brush entity and misses model key";
  }
};

const IssueType
  BrushEntityWithoutModelKeyIssueGenerator::BrushEntityWithoutModelKeyIssue::Type =
    Issue::freeType();

class BrushEntityWithoutModelKeyIssueGenerator::BrushEntityWithoutModelKeyIssueQuickFix
  : public IssueQuickFix
{
public:
  BrushEntityWithoutModelKeyIssueQuickFix()
    : IssueQuickFix(BrushEntityWithoutModelKeyIssue::Type, "Autoassign model key")
  {
  }

private:
  void doApply(MapFacade* facade, const Issue* issue) const override
  {
    const PushSelection push(facade);

    // If world node is affected, the selection will fail, but if nothing is selected,
    // the removeProperty call will correctly affect worldspawn either way.

    facade->deselectAll();
    facade->selectNodes({issue->node()});

    const EntityNodeBase* propertyNode = static_cast<EntityNodeBase*>(issue->node());

    // grab unique entity name
    std::string targetname;
    const std::string* name =
      propertyNode->entity().property(EntityPropertyKeys::Targetname);
    if (name != nullptr && !name->empty())
    {
      targetname = *name;
    }
    else
    {
      // give entity a unique target name
      propertyNode->generateUniqueTargetname(targetname);
      facade->setProperty(EntityPropertyKeys::Targetname, targetname);
    }

    // "model" has to be the same as "name" because dmap stores the model into the BSP
    // using the "name" value
    facade->setProperty(EntityPropertyKeys::Model, targetname);
  }
};

BrushEntityWithoutModelKeyIssueGenerator::BrushEntityWithoutModelKeyIssueGenerator()
  : IssueGenerator(
    BrushEntityWithoutModelKeyIssue::Type, "Brush entity without model key")
{
  addQuickFix(new BrushEntityWithoutModelKeyIssueQuickFix());
}

void BrushEntityWithoutModelKeyIssueGenerator::doGenerate(
  EntityNode* entityNode, IssueList& issues) const
{
  ensure(entityNode != nullptr, "entity is null");
  const Assets::EntityDefinition* definition = entityNode->entity().definition();
  if (
    definition != nullptr
    && definition->type() == Assets::EntityDefinitionType::BrushEntity
    && entityNode->hasChildren()
    && !entityNode->entity().hasProperty(EntityPropertyKeys::Model))
    issues.push_back(new BrushEntityWithoutModelKeyIssue(entityNode));
}
} // namespace Model
} // namespace TrenchBroom
