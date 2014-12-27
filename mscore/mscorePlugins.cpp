//=============================================================================
//  MuseScore
//  Linux Music Score Editor
//
//  Copyright (C) 2009-2012 Werner Schweer and others
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License version 2.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//=============================================================================

#include "musescore.h"
#include "libmscore/score.h"
#include "libmscore/undo.h"
// #include "globals.h"
#include "config.h"
#include "preferences.h"
#include "libmscore/chord.h"
#include "libmscore/note.h"
#include "libmscore/utils.h"
#include "libmscore/mscore.h"
#include "libmscore/measurebase.h"
#include "libmscore/measure.h"
#include "libmscore/segment.h"
#include "libmscore/rest.h"
#include "libmscore/stafftext.h"
// #include "plugins.h"
#include "libmscore/cursor.h"
#include "libmscore/page.h"
#include "libmscore/system.h"
#include "libmscore/staff.h"
#include "libmscore/part.h"
#include "libmscore/timesig.h"
#include "libmscore/keysig.h"
#include "libmscore/harmony.h"
#include "libmscore/slur.h"
#include "libmscore/tie.h"
#include "libmscore/notedot.h"
#include "libmscore/figuredbass.h"
#include "libmscore/accidental.h"
#include "libmscore/lyrics.h"
#include "libmscore/layoutbreak.h"
#include "qmlplugin.h"

namespace Ms {

extern QString localeName;

void MuseScore::unregisterPlugin(PluginDescription* plugin)
      {
      QString pluginPath = plugin->path;
      QFileInfo np(pluginPath);
      if (np.suffix() != "qml")
            return;
      QString baseName = np.baseName();

      bool found = false;
      foreach(QString s, plugins) {
            QFileInfo fi(s);
            if (fi.baseName() == baseName) {
                  found = true;
                  break;
                  }
            }
      if (!found) {
            if (MScore::debugMode)
                  qDebug("  Plugin <%s> not registered", qPrintable(pluginPath));
            return;
            }
      plugins.removeAll(pluginPath);


      removeMenuEntry(plugin);
      QAction* a = plugin->shortcut.action();
      pluginActions.removeAll(a);

      disconnect(a, SIGNAL(triggered()), pluginMapper, SLOT(map()));
      pluginMapper->removeMappings(a);

      }

//---------------------------------------------------------
//   createMenuEntry
//    syntax: "entry.entry.entry"
//---------------------------------------------------------

void MuseScore::createMenuEntry(PluginDescription* plugin)
      {
      if (!pluginMapper)
            return;

      QString menu = plugin->menuPath;
      QStringList ml;
      QString s;
      bool escape = false;
      foreach (QChar c, menu) {
            if (escape) {
                  escape = false;
                  s += c;
                  }
            else {
                  if (c == '\\')
                        escape = true;
                  else {
                        if (c == '.') {
                              ml += s;
                              s = "";
                              }
                        else {
                              s += c;
                              }
                        }
                  }
            }
      if (!s.isEmpty())
            ml += s;

      int n            = ml.size();
      QWidget* curMenu = menuBar();

      for(int i = 0; i < n; ++i) {
            QString m  = ml[i];
            bool found = false;
            QList<QObject*> ol = curMenu->children();
            foreach(QObject* o, ol) {
                  QMenu* menu = qobject_cast<QMenu*>(o);
                  if (!menu)
                        continue;
                  if (menu->objectName() == m || menu->title() == m) {
                        curMenu = menu;
                        found = true;
                        break;
                        }
                  }
            if (!found) {
                  if (i == 0) {
                        curMenu = new QMenu(m, menuBar());
                        curMenu->setObjectName(m);
                        menuBar()->insertMenu(menuBar()->actions().back(), (QMenu*)curMenu);
                        if (MScore::debugMode)
                              qDebug("add Menu <%s>", qPrintable(m));
                        }
                  else if (i + 1 == n) {
                        QStringList sl = m.split(":");
                        QAction* a = plugin->shortcut.action();
                        QMenu* cm = static_cast<QMenu*>(curMenu);
                        if (sl.size() == 2) {
                              QList<QAction*> al = cm->actions();
                              QAction* ba = 0;
                              foreach(QAction* ia, al) {
                                    if (ia->text() == sl[0]) {
                                          ba = ia;
                                          break;
                                          }
                                    }
                              a->setText(sl[1]);
                              cm->insertAction(ba, a);
                              }
                        else {
                              a->setText(m);
                              cm->addAction(a);
                              }

                        if (MScore::debugMode)
                              qDebug("plugins: add action <%s>", qPrintable(m));
                        }
                  else {
                        curMenu = ((QMenu*)curMenu)->addMenu(m);
                        if (MScore::debugMode)
                              qDebug("add menu <%s>", qPrintable(m));
                        }
                  }
            }
      }


void MuseScore::removeMenuEntry(PluginDescription* plugin)
      {
      if (!pluginMapper)
            return;

      QString menu = plugin->menuPath;
      QStringList ml;
      QString s;
      bool escape = false;
      foreach (QChar c, menu) {
            if (escape) {
                  escape = false;
                  s += c;
                  }
            else {
                  if (c == '\\')
                        escape = true;
                  else {
                        if (c == '.') {
                              ml += s;
                              s = "";
                              }
                        else {
                              s += c;
                              }
                        }
                  }
            }
      if (!s.isEmpty())
            ml += s;

      if(ml.isEmpty())
            return;

      int n            = ml.size();
      QWidget* curMenu = menuBar();

      for(int i = 0; i < n-1; ++i) {
            QString m  = ml[i];
            QList<QObject*> ol = curMenu->children();
            foreach(QObject* o, ol) {
                  QMenu* menu = qobject_cast<QMenu*>(o);
                  if (!menu)
                        continue;
                  if (menu->objectName() == m || menu->title() == m) {
                        curMenu = menu;
                        break;
                        }
                  }
            }
      QString m  = ml[n-1];
      QStringList sl = m.split(":");
      QAction* a = plugin->shortcut.action();
      QMenu* cm = static_cast<QMenu*>(curMenu);
      cm->removeAction(a);
      for(int i = n-2; i >= 0; --i) {

            QMenu* menu = qobject_cast<QMenu*>(cm->parent());
            if (cm->isEmpty())
                  if(cm->isEmpty()) {
                        delete cm;
                        }
            cm = menu;
            }
      }

//---------------------------------------------------------
//   pluginIdxFromPath
//---------------------------------------------------------

int MuseScore::pluginIdxFromPath(QString pluginPath) {
      QFileInfo np(pluginPath);
      QString baseName = np.baseName();
      int idx = 0;
      foreach(QString s, plugins) {
            QFileInfo fi(s);
            if (fi.baseName() == baseName)
                  return idx;
            idx++;
            }
      return -1;
      }

//---------------------------------------------------------
//   loadPlugins
//---------------------------------------------------------

void MuseScore::loadPlugins()
      {
      pluginMapper = new QSignalMapper(this);
      connect(pluginMapper, SIGNAL(mapped(int)), SLOT(pluginTriggered(int)));
      for (int i = 0; i < preferences.pluginList.size(); ++i) {
            PluginDescription* d = &preferences.pluginList[i];
            if (d->load)
                  registerPlugin(d);
            }
      }

//---------------------------------------------------------
//   unloadPlugins
//---------------------------------------------------------

void MuseScore::unloadPlugins()
      {
      for (int idx = 0; idx < plugins.size() ; idx++) {
            ; // TODO
            }
      }

//---------------------------------------------------------
//   loadPlugin
//---------------------------------------------------------

bool MuseScore::loadPlugin(const QString& filename)
      {
      bool result = false;

      if (!pluginMapper) {
            pluginMapper = new QSignalMapper(this);
            connect(pluginMapper, SIGNAL(mapped(int)), SLOT(pluginTriggered(int)));
            }

      QDir pluginDir(mscoreGlobalShare + "plugins");
      if (MScore::debugMode)
            qDebug("Plugin Path <%s>", qPrintable(mscoreGlobalShare + "plugins"));

      if (filename.endsWith(".qml")){
            QFileInfo fi(pluginDir, filename);
            if (fi.exists()) {
                  QString path(fi.filePath());
                  PluginDescription* p = new PluginDescription;
                  p->path = path;
                  p->load = false;
                  collectPluginMetaInformation(p);
                  registerPlugin(p);
                  result = true;
                  }
            }
      return result;
      }
}

