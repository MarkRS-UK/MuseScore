//=============================================================================
//  MuseScore
//  Music Composition & Notation
//
//  Copyright (C) 2002-2012 Werner Schweer
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License version 2
//  as published by the Free Software Foundation and appearing in
//  the file LICENCE.GPL
//=============================================================================

#include "plugin.h"
#ifdef SCRIPT_INTERFACE

#include "qmlplugin.h"
#include "shortcut.h"
#include "libmscore/musescoreCore.h"
#include "libmscore/score.h"
#include "musescore.h"
#include "preferences.h"
#include "pluginCreator.h"
#include <QtQml>

namespace Ms {

// extern MuseScoreCore* mscoreCore;
extern void scMsgHandler(QtMsgType, const QMessageLogContext &, const QString &);

//---------------------------------------------------------
//   QmlPlugin
//---------------------------------------------------------

QmlPlugin::QmlPlugin(QQuickItem* parent)
   : QQuickItem(parent)
      {
      msc = MuseScoreCore::mscoreCore;
      }

QmlPlugin::~QmlPlugin()
      {
      }

//---------------------------------------------------------
//   scores
//---------------------------------------------------------

QQmlListProperty<Score> QmlPlugin::scores()
      {
      return QQmlListProperty<Score>(this, plugin::scores());
      }

//---------------------------------------------------------
//   readScore
//---------------------------------------------------------

Score* QmlPlugin::readScore(const QString& name)
      {
//      Score * score = plugin::openScore(name);
      Score * score = plugin::readScore(name);
      // tell QML not to garbage collect this score
      if (score)
            QQmlEngine::setObjectOwnership(score, QQmlEngine::CppOwnership);
      return score;
      }

//---------------------------------------------------------
//   newElement
//---------------------------------------------------------

Ms::Element* QmlPlugin::newElement(int t)
      {
      Element* e = plugin::newElement(t);

      // tell QML not to garbage collect this score
      Ms::MScore::scEng()->setObjectOwnership(e, QQmlEngine::CppOwnership);
      return e;
      }

//---------------------------------------------------------
//   newScore
//---------------------------------------------------------

Score* QmlPlugin::newScore(const QString& name, const QString& part, int measures)
      {
      Score* score = plugin::newScore(name, part, measures);

      // tell QML not to garbage collect this score
//      score->startCmd();
      return score;
      }

//---------------------------------------------------------
//   registerPlugin
//---------------------------------------------------------

void MuseScore::registerPlugin(PluginDescription* plugin)
      {
      QString pluginPath = plugin->path;
      QFileInfo np(pluginPath);
      if (np.suffix() != "qml")
            return;
      QString baseName = np.baseName();

      foreach(QString s, plugins) {
            QFileInfo fi(s);
            if (fi.baseName() == baseName) {
                  if (MScore::debugMode)
                        qDebug("  Plugin <%s> already registered", qPrintable(pluginPath));
                  return;
                  }
            }

      QFile f(pluginPath);
      if (!f.open(QIODevice::ReadOnly)) {
            if (MScore::debugMode)
                  qDebug("Loading Plugin <%s> failed", qPrintable(pluginPath));
            return;
            }
      if (MScore::debugMode)
            qDebug("Register Plugin <%s>", qPrintable(pluginPath));
      f.close();
      QObject* obj = 0;
      QQmlComponent component(Ms::MScore::scEng(), QUrl::fromLocalFile(pluginPath));
      obj = component.create();
      if (obj == 0) {
            qDebug("creating component <%s> failed", qPrintable(pluginPath));
            foreach(QQmlError e, component.errors()) {
                  qDebug("   line %d: %s", e.line(), qPrintable(e.description()));
                  }
            return;
            }
      //
      // load translation
      //
      QLocale locale;
      QString t = np.absolutePath() + "/translations/locale_" + MuseScore::getLocaleISOCode().left(2) + ".qm";
      QTranslator* translator = new QTranslator;
      if (!translator->load(t)) {
//            qDebug("cannot load qml translations <%s>", qPrintable(t));
            delete translator;
            }
      else {
//            qDebug("load qml translations <%s>", qPrintable(t));
            qApp->installTranslator(translator);
            }

      QmlPlugin* item = qobject_cast<QmlPlugin*>(obj);
      QString menuPath = item->menuPath();
      plugin->menuPath = menuPath;
      plugins.append(pluginPath);
      createMenuEntry(plugin);

      QAction* a = plugin->shortcut.action();
      pluginActions.append(a);
      int pluginIdx = plugins.size() - 1; // plugin is already appended
      connect(a, SIGNAL(triggered()), pluginMapper, SLOT(map()));
      pluginMapper->setMapping(a, pluginIdx);

      delete obj;
      }

//---------------------------------------------------------
//   scEng - sc-ript-Eng-ine
//---------------------------------------------------------

QQmlEngine* MScore::scEng()
      {
      if (_scEng == 0) {
            //-----------some qt bindings
            _scEng = new QQmlEngine;
            msRegister();
            }
      return _scEng;
      }

//---------------------------------------------------------
//   collectPluginMetaInformation
//---------------------------------------------------------

void collectPluginMetaInformation(PluginDescription* d)
      {
      qDebug("Collect meta for <%s>", qPrintable(d->path));

      QQmlComponent component(Ms::MScore::scEng(), QUrl::fromLocalFile(d->path));
      QObject* obj = component.create();
      if (obj == 0) {
            qDebug("creating component <%s> failed", qPrintable(d->path));
            foreach(QQmlError e, component.errors()) {
                  qDebug("   line %d: %s", e.line(), qPrintable(e.description()));
                  }
            return;
            }
      QmlPlugin* item = qobject_cast<QmlPlugin*>(obj);
      if (item) {
            d->version      = item->version();
            d->description  = item->description();
            }
      delete obj;
      }



//---------------------------------------------------------
//   pluginTriggered
//---------------------------------------------------------

void MuseScore::pluginTriggered(int idx)
      {
      QString pp = plugins[idx];

      QQmlEngine* engine = Ms::MScore::scEng();

      QQmlComponent component(engine);
      component.loadUrl(QUrl::fromLocalFile(pp));
      QObject* obj = component.create();
      if (obj == 0) {
            foreach(QQmlError e, component.errors())
                  qDebug("   line %d: %s", e.line(), qPrintable(e.description()));
            return;
            }

      QmlPlugin* p = qobject_cast<QmlPlugin*>(obj);

      if (p->pluginType() == "dock" || p->pluginType() == "dialog") {
            QQuickView* view = new QQuickView(engine, 0);
            view->setSource(QUrl::fromLocalFile(pp));
            view->setTitle(p->menuPath().mid(p->menuPath().lastIndexOf(".") + 1));
            view->setColor(QApplication::palette().color(QPalette::Window));
            //p->setParentItem(view->contentItem());
            //view->setWidth(p->width());
            //view->setHeight(p->height());
            view->setResizeMode(QQuickView::SizeRootObjectToView);
            if (p->pluginType() == "dock") {
                  QDockWidget* dock = new QDockWidget("Plugin", 0);
                  dock->setAttribute(Qt::WA_DeleteOnClose);
                  Qt::DockWidgetArea area = Qt::RightDockWidgetArea;
                  if (p->dockArea() == "left")
                        area = Qt::LeftDockWidgetArea;
                  else if (p->dockArea() == "top")
                        area = Qt::TopDockWidgetArea;
                  else if (p->dockArea() == "bottom")
                        area = Qt::BottomDockWidgetArea;
                  QWidget* w = QWidget::createWindowContainer(view);
                  dock->setWidget(w);
                  addDockWidget(area, dock);
                  connect(engine, SIGNAL(quit()), dock, SLOT(close()));
                  view->show();
                  }
            else {
                  connect(engine, SIGNAL(quit()), view, SLOT(close()));
                  view->show();
                  }
            }

      // dont call startCmd for non modal dialog
      if (cs && p->pluginType() != "dock")
            cs->startCmd();
      p->runPlugin();
      if (cs && p->pluginType() != "dock")
            cs->endCmd();
      endCmd();
      }

//---------------------------------------------------------
//   runClicked
//---------------------------------------------------------

void PluginCreator::runClicked()
      {
      log->clear();
      QQmlEngine* qml = Ms::MScore::scEng();
      connect(qml, SIGNAL(warnings(const QList<QQmlError>&)),
         SLOT(qmlWarnings(const QList<QQmlError>&)));

      item = 0;
      QQmlComponent component(qml);
      component.setData(textEdit->toPlainText().toUtf8(), QUrl());
      QObject* obj = component.create();
      if (obj == 0) {
            msg("creating component failed\n");
            foreach(QQmlError e, component.errors())
                  msg(QString("   line %1: %2\n").arg(e.line()).arg(e.description()));
            stop->setEnabled(false);
            return;
            }
      qInstallMessageHandler(scMsgHandler);
      stop->setEnabled(true);
      run->setEnabled(false);

      item = qobject_cast<QmlPlugin*>(obj);

      if (item->pluginType() == "dock" || item->pluginType() == "dialog") {
            view = new QQuickView(qml, 0);
            view->setTitle(item->menuPath().mid(item->menuPath().lastIndexOf(".") + 1));
            view->setColor(QApplication::palette().color(QPalette::Window));
            view->setResizeMode(QQuickView::SizeRootObjectToView);
            view->setWidth(item->width());
            view->setHeight(item->height());
            item->setParentItem(view->contentItem());

            if (item->pluginType() == "dock") {
                  dock = new QDockWidget("Plugin", 0);
                  dock->setAttribute(Qt::WA_DeleteOnClose);
                  dock->setWidget(QWidget::createWindowContainer(view));
                  dock->widget()->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
                  Qt::DockWidgetArea area = Qt::RightDockWidgetArea;
                  if (item->dockArea() == "left")
                        area = Qt::LeftDockWidgetArea;
                  else if (item->dockArea() == "top")
                        area = Qt::TopDockWidgetArea;
                  else if (item->dockArea() == "bottom")
                        area = Qt::BottomDockWidgetArea;
                  addDockWidget(area, dock);
                  connect(dock, SIGNAL(destroyed()), SLOT(closePlugin()));
                  dock->widget()->setAttribute(Qt::WA_DeleteOnClose);
                  }
            view->show();
            view->raise();
            connect(view, SIGNAL(destroyed()), SLOT(closePlugin()));
            }

      connect(qml,  SIGNAL(quit()), SLOT(closePlugin()));

      if (mscore->currentScore() && item->pluginType() != "dock")
            mscore->currentScore()->startCmd();
      item->runPlugin();
      if (mscore->currentScore() && item->pluginType() != "dock")
            mscore->currentScore()->endCmd();
      mscore->endCmd();
      }

//---------------------------------------------------------
//   qmlWarnings
//---------------------------------------------------------

void PluginCreator::qmlWarnings(const QList<QQmlError>& el)
      {
      foreach(const QQmlError& e, el)
            msg(QString("%1:%2: %3\n").arg(e.line()).arg(e.column()).arg(e.description()));
      }

}
#endif
