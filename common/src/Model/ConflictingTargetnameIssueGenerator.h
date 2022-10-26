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

#pragma once

#include "Model/IssueGenerator.h"

#include <vector>

namespace TrenchBroom
{
namespace Model
{
// Helper issue generator for Doom 3 because Doom doesn't allow entities with the same
// (target)name Also Doom 3 does not call it targetname anymore. It's just name and every
// is supposed to have a unique name
class ConflictingTargetnameIssueGenerator : public IssueGenerator
{
private:
  class ConflictingTargetnameIssue;
  class ConflictingTargetnameIssueQuickFix;

public:
  ConflictingTargetnameIssueGenerator();

private:
  void doGenerate(EntityNodeBase* node, IssueList& issues) const override;
};
} // namespace Model
} // namespace TrenchBroom
