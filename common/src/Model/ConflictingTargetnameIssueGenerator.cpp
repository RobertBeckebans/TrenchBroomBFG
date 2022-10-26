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

#include "ConflictingTargetnameIssueGenerator.h"

#include "Model/Entity.h"
#include "Model/EntityNode.h"
#include "Model/EntityProperties.h"
#include "Model/Issue.h"
#include "Model/IssueQuickFix.h"
#include "Model/MapFacade.h"
#include "Model/PushSelection.h"

#include <string>

namespace TrenchBroom
{
namespace Model
{
class ConflictingTargetnameIssueGenerator::ConflictingTargetnameIssue : public Issue
{
public:
  static const IssueType Type;

public:
  explicit ConflictingTargetnameIssue(EntityNodeBase* node)
    : Issue(node)
  {
  }

  IssueType doGetType() const override { return Type; }

  std::string doGetDescription() const override
  {
    const EntityNodeBase* entityNode = static_cast<EntityNodeBase*>(node());
    return entityNode->name() + " has non-unique name key";
  }
};

const IssueType ConflictingTargetnameIssueGenerator::ConflictingTargetnameIssue::Type =
  Issue::freeType();

class ConflictingTargetnameIssueGenerator::ConflictingTargetnameIssueQuickFix
  : public IssueQuickFix
{
public:
  ConflictingTargetnameIssueQuickFix()
    : IssueQuickFix(ConflictingTargetnameIssue::Type, "Make name unique")
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

    // give entity a unique target name
    const EntityNodeBase* propertyNode = static_cast<EntityNodeBase*>(issue->node());
    std::string uniqueName;
    propertyNode->generateUniqueTargetname(uniqueName);
    facade->setProperty(EntityPropertyKeys::Targetname, uniqueName);
  }
};

ConflictingTargetnameIssueGenerator::ConflictingTargetnameIssueGenerator()
  : IssueGenerator(ConflictingTargetnameIssue::Type, "Duplicated name")
{
  addQuickFix(new ConflictingTargetnameIssueQuickFix());
}

void ConflictingTargetnameIssueGenerator::doGenerate(
  EntityNodeBase* node, IssueList& issues) const
{
  if (node->hasConflictingTargetname())
    issues.push_back(new ConflictingTargetnameIssue(node));
}
} // namespace Model
} // namespace TrenchBroom
