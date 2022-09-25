/*
 Copyright (C) 2010-2017 Kristian Duske

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

#include "View/Actions.h"

#include "IO/Path.h"
#include "KeyStrings.h"
#include "PreferenceManager.h"
#include "Preferences.h"

#include <QApplication>
#include <QFileInfo>
#include <QSettings>
#include <QTextStream>

#include <array>
#include <iostream>
#include <tuple>

namespace TrenchBroom {
namespace View {
static QString escapeString(const QString& str) {
  if (str == "'") {
    return "\\'";
  } else if (str == "\\") {
    return "\\\\";
  } else {
    return str;
  }
}

static void printKeys(QTextStream& out) {
  const auto keyStrings = KeyStrings();

  out << "const keys = {\n";
  for (const auto& [portable, native] : keyStrings) {
    out << "    '" << escapeString(portable) << "': '" << escapeString(native) << "',\n";
  }
  out << "};\n";
}

static QString toString(const IO::Path& path, const QString& suffix) {
  QString result;
  result += "[";
  for (const auto& component : path.components()) {
    result += "'" + QString::fromStdString(component) + "', ";
  }
  result += "'" + suffix + "'";
  result += "]";
  return result;
}

static QString toString(const QKeySequence& keySequence) {
  static const std::array<std::tuple<int, QString>, 4> Modifiers = {
    std::make_tuple(static_cast<int>(Qt::CTRL), QString::fromLatin1("Ctrl")),
    std::make_tuple(static_cast<int>(Qt::ALT), QString::fromLatin1("Alt")),
    std::make_tuple(static_cast<int>(Qt::SHIFT), QString::fromLatin1("Shift")),
    std::make_tuple(static_cast<int>(Qt::META), QString::fromLatin1("Meta")),
  };

  QString result;
  result += "{ ";

  if (keySequence.count() > 0) {
    const int keyWithModifier = keySequence[0];
    const int key = keyWithModifier & ~(static_cast<int>(Qt::KeyboardModifierMask));

    const QString keyPortableText = QKeySequence(key).toString(QKeySequence::PortableText);

    result += "key: '" + escapeString(keyPortableText) + "', ";
    result += "modifiers: [";
    for (const auto& [modifier, portableText] : Modifiers) {
      if ((keyWithModifier & modifier) != 0) {
        result += "'" + escapeString(portableText) + "', ";
      }
    }
    result += "]";
  } else {
    result += "key: '', modifiers: []";
  }
  result += " }";
  return result;
}

static QString toYamlString(const QKeySequence& keySequence) {
  static const std::array<std::tuple<int, QString>, 4> Modifiers = {
    std::make_tuple(static_cast<int>(Qt::CTRL), QString::fromLatin1("Ctrl")),
    std::make_tuple(static_cast<int>(Qt::ALT), QString::fromLatin1("Alt")),
    std::make_tuple(static_cast<int>(Qt::SHIFT), QString::fromLatin1("Shift")),
    std::make_tuple(static_cast<int>(Qt::META), QString::fromLatin1("Meta")),
  };

  QString result;
  // result += "{ ";

  if (keySequence.count() > 0) {
    const int keyWithModifier = keySequence[0];
    const int key = keyWithModifier & ~(static_cast<int>(Qt::KeyboardModifierMask));

    const QString keyPortableText = QKeySequence(key).toString(QKeySequence::PortableText);

    for (const auto& [modifier, portableText] : Modifiers) {
      if ((keyWithModifier & modifier) != 0) {
        result += escapeString(portableText) + "+";
      }
    }
    result += escapeString(keyPortableText);
  }
  // result += " }";
  return result;
}

class PrintMenuVisitor : public TrenchBroom::View::MenuVisitor {
private:
  QTextStream& m_out;
  IO::Path m_path;

public:
  PrintMenuVisitor(QTextStream& out)
    : m_out(out) {}

  void visit(const Menu& menu) override {
    m_path = m_path + IO::Path(menu.name());
    menu.visitEntries(*this);
    m_path = m_path.deleteLastComponent();
  }

  void visit(const MenuSeparatorItem&) override {}

  void visit(const MenuActionItem& item) override {
    m_out << "    '" << QString::fromStdString(item.action().preferencePath().asString("/"))
          << "': ";
    m_out << "{ path: " << toString(m_path, item.label())
          << ", shortcut: " << toString(item.action().keySequence()) << " },\n";
  }
};

class CarnacPrintMenuVisitor : public TrenchBroom::View::MenuVisitor {
private:
  QTextStream& m_out;
  IO::Path m_path;

public:
  CarnacPrintMenuVisitor(QTextStream& out)
    : m_out(out) {}

  void visit(const Menu& menu) override {
    m_path = m_path + IO::Path(menu.name());
    menu.visitEntries(*this);
    m_path = m_path.deleteLastComponent();
  }

  void visit(const MenuSeparatorItem&) override {}

  void visit(const MenuActionItem& item) override {

    if (item.action().keySequence().count() > 0) {
      m_out << "    - name:      "
            << QString::fromStdString(item.action().preferencePath().asString("/")) << "\n";
      m_out << "      keys:\n      - " << toYamlString(item.action().keySequence()) << "\n\n";
    }
  }
};

static void printMenuShortcuts(QTextStream& out, bool yaml) {
  if (!yaml) {
    out << "const menu = {\n";
  }

  const auto& actionManager = ActionManager::instance();
  if (yaml) {
    CarnacPrintMenuVisitor visitor(out);
    actionManager.visitMainMenu(visitor);
  } else {
    PrintMenuVisitor visitor(out);
    actionManager.visitMainMenu(visitor);
  }

  if (!yaml) {
    out << "};\n";
  }
}

static void printActionShortcuts(QTextStream& out) {
  out << "const actions = {\n";

  auto printPref = [&out](const IO::Path& prefPath, const QKeySequence& keySequence) {
    out << "    '" << QString::fromStdString(prefPath.asString("/")) << "': ";
    out << toString(keySequence) << ",\n";
  };

  class ToolbarVisitor : public MenuVisitor {
  public:
    std::vector<const Action*> toolbarActions;

    void visit(const Menu& menu) override { menu.visitEntries(*this); }

    void visit(const MenuSeparatorItem&) override {}

    void visit(const MenuActionItem& item) override {
      const Action* tAction = &item.action();
      toolbarActions.push_back(tAction);
    }
  };

  const auto& actionManager = ActionManager::instance();
  ToolbarVisitor visitor;
  actionManager.visitToolBarActions(visitor);
  for (const Action* action : visitor.toolbarActions) {
    printPref(action->preferencePath(), action->keySequence());
  }
  actionManager.visitMapViewActions([&printPref](const auto& action) {
    printPref(action.preferencePath(), action.keySequence());
  });

  // some keys are just Preferences (e.g. WASD)
  for (Preference<QKeySequence>* keyPref : Preferences::keyPreferences()) {
    printPref(keyPref->path(), keyPref->defaultValue());
  }

  out << "};\n";
}

static void printYamlActionShortcuts(QTextStream& out) {

  auto printPref = [&out](const IO::Path& prefPath, const QKeySequence& keySequence) {
    if (keySequence.count() > 0) {
      out << "    - name:      " << QString::fromStdString(prefPath.asString("/")) << "\n";
      out << "      keys:\n      - " << toYamlString(keySequence) << "\n\n";
    }
  };

  class ToolbarVisitor : public MenuVisitor {
  public:
    std::vector<const Action*> toolbarActions;

    void visit(const Menu& menu) override { menu.visitEntries(*this); }

    void visit(const MenuSeparatorItem&) override {}

    void visit(const MenuActionItem& item) override {
      const Action* tAction = &item.action();
      toolbarActions.push_back(tAction);
    }
  };

  const auto& actionManager = ActionManager::instance();
  ToolbarVisitor visitor;
  actionManager.visitToolBarActions(visitor);
  for (const Action* action : visitor.toolbarActions) {
    printPref(action->preferencePath(), action->keySequence());
  }
  actionManager.visitMapViewActions([&printPref](const auto& action) {
    printPref(action.preferencePath(), action.keySequence());
  });

  // some keys are just Preferences (e.g. WASD)
  for (Preference<QKeySequence>* keyPref : Preferences::keyPreferences()) {
    printPref(keyPref->path(), keyPref->defaultValue());
  }
}
} // namespace View
} // namespace TrenchBroom

extern void qt_set_sequence_auto_mnemonic(bool b);

int main(int argc, char* argv[]) {
  if (argc != 2 && argc != 3) {
    std::cout << "Usage: dump-shortcuts <path-to-output-file> -carnac\n";
    return 1;
  }

  QSettings::setDefaultFormat(QSettings::IniFormat);

  // We can't use auto mnemonics in TrenchBroom. e.g. by default with Qt, Alt+D opens the "Debug"
  // menu, Alt+S activates the "Show default properties" checkbox in the entity inspector. Flying
  // with Alt held down and pressing WASD is a fundamental behaviour in TB, so we can't have
  // shortcuts randomly activating.
  qt_set_sequence_auto_mnemonic(false);

  const auto path = QString(argv[1]);
  auto file = QFile(path);
  const auto fileInfo = QFileInfo(file.fileName());
  const auto absPath = fileInfo.absoluteFilePath().toStdString();

  if (!file.open(
        QIODevice::WriteOnly |
        QIODevice::Text)) { // QIODevice::WriteOnly implies truncate, which we want
    std::cout << "Could not open output file for writing: " << absPath << "\n";
    return 1;
  }

  bool yaml = false;
  if (argc == 3) {
    yaml = true;
  }

  QTextStream out(&file);
  out.setCodec("UTF-8");

  TrenchBroom::PreferenceManager::createInstance<TrenchBroom::AppPreferenceManager>();

  // QKeySequence requires that an application instance is created!
  QApplication app(argc, argv);
  app.setApplicationName("TrenchBroomBFG");
  // Needs to be "" otherwise Qt adds this to the paths returned by QStandardPaths
  // which would cause preferences to move from where they were with wx
  app.setOrganizationName("");
  app.setOrganizationDomain("io.github.trenchbroom");

  if (yaml) {
    out << "group:          TrenchBroom\n";
    out << "process:        trenchbroom\n\n";
    out << "shortcuts:\n\n";
  } else {
    TrenchBroom::View::printKeys(out);
  }

  TrenchBroom::View::printMenuShortcuts(out, yaml);
  if (yaml) {
    TrenchBroom::View::printYamlActionShortcuts(out);
  } else {
    TrenchBroom::View::printActionShortcuts(out);
  }

  out.flush();
  if (out.status() == QTextStream::Ok) {
    return 0;
  } else {
    return 1;
  }
}
