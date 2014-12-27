//=============================================================================
//  MuseScore
//  Music Composition & Notation
//
//  Copyright (C) 2002-2011 Werner Schweer
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License version 2
//  as published by the Free Software Foundation and appearing in
//  the file LICENCE.GPL
//=============================================================================


#include "pluginCreator.h"
#include "musescore.h"
#include "icons.h"
#include "helpBrowser.h"
#include "preferences.h"
#include "libmscore/score.h"

#ifdef QML_SCRIPT_INTERFACE
#include "qmlplugin.h"
#endif
#ifdef LUA_SCRIPT_INTERFACE
#include <QtLua/State>
#include "luaplugin.h"
#endif

namespace Ms {

extern bool useFactorySettings;

//static const char* states[] = {
//      "S_INIT", "S_EMPTY", "S_CLEAN", "S_DIRTY"
//      };


//---------------------------------------------------------
//   PluginCreator
//---------------------------------------------------------

PluginCreator::PluginCreator(QWidget* parent)
   : QMainWindow(parent)
      {
      state       = PCState::INIT;
      item        = 0;
      view        = 0;
      dock        = 0;
      manualDock  = 0;
      helpBrowser = 0;

      setIconSize(QSize(preferences.iconWidth * guiScaling, preferences.iconHeight * guiScaling));

      setupUi(this);

      QToolBar* fileTools = addToolBar(tr("File Operations"));
      fileTools->setObjectName("FileOperations");

      actionNew->setIcon(*icons[int(Icons::fileNew_ICON)]);
      actionNew->setShortcut(QKeySequence(QKeySequence::New));
      fileTools->addAction(actionNew);

      actionOpen->setIcon(*icons[int(Icons::fileOpen_ICON)]);
      actionOpen->setShortcut(QKeySequence(QKeySequence::Open));
      fileTools->addAction(actionOpen);

      actionReload->setIcon(*icons[int(Icons::viewRefresh_ICON)]);
      fileTools->addAction(actionReload);

      actionSave->setIcon(*icons[int(Icons::fileSave_ICON)]);
      actionSave->setShortcut(QKeySequence(QKeySequence::Save));
      fileTools->addAction(actionSave);

      actionQuit->setShortcut(QKeySequence(QKeySequence::Close));

      actionManual->setIcon(QIcon(*icons[int(Icons::helpContents_ICON)]));
      actionManual->setShortcut(QKeySequence(QKeySequence::HelpContents));
      fileTools->addAction(actionManual);

      QToolBar* editTools = addToolBar(tr("Edit Operations"));
      editTools->setObjectName("EditOperations");
      actionUndo->setIcon(*icons[int(Icons::undo_ICON)]);
      actionUndo->setShortcut(QKeySequence(QKeySequence::Undo));
      editTools->addAction(actionUndo);
      actionRedo->setIcon(*icons[int(Icons::redo_ICON)]);
      actionRedo->setShortcut(QKeySequence(QKeySequence::Redo));
      editTools->addAction(actionRedo);
      actionUndo->setEnabled(false);
      actionRedo->setEnabled(false);

      log->setReadOnly(true);
      log->setMaximumBlockCount(1000);

      readSettings();
      setState(PCState::EMPTY);

      connect(run,        SIGNAL(clicked()),     SLOT(runClicked()));
      connect(stop,       SIGNAL(clicked()),     SLOT(stopClicked()));
      connect(actionOpen, SIGNAL(triggered()),   SLOT(loadPlugin()));
      connect(actionReload, SIGNAL(triggered()), SLOT(load()));
      connect(actionSave, SIGNAL(triggered()),   SLOT(savePlugin()));
      connect(actionNew,  SIGNAL(triggered()),   SLOT(newPlugin()));
      connect(actionQuit, SIGNAL(triggered()),   SLOT(close()));
      connect(actionManual, SIGNAL(triggered()), SLOT(showManual()));
      connect(actionUndo, SIGNAL(triggered()),         textEdit,   SLOT(undo()));
      connect(actionRedo, SIGNAL(triggered()),         textEdit,   SLOT(redo()));
      connect(textEdit,   SIGNAL(undoAvailable(bool)), actionUndo, SLOT(setEnabled(bool)));
      connect(textEdit,   SIGNAL(redoAvailable(bool)), actionRedo, SLOT(setEnabled(bool)));
      connect(textEdit,   SIGNAL(textChanged()), SLOT(textChanged()));
      }

//---------------------------------------------------------
//   manualPath
//---------------------------------------------------------

QString PluginCreator::manualPath()
      {
      QString path = mscoreGlobalShare;
      path += "/manual/plugins/plugins.html";
      return path;
      }

//---------------------------------------------------------
//   setState
//---------------------------------------------------------

void PluginCreator::setState(PCState newState)
      {
      if (state == newState)
            return;
      switch(state) {
            case PCState::INIT:
                  switch(newState) {
                        case PCState::INIT:
                              break;
                        case PCState::EMPTY:
                              setTitle("");
                              actionSave->setEnabled(false);
                              run->setEnabled(false);
                              stop->setEnabled(false);
                              textEdit->setEnabled(false);
                              break;
                        case PCState::CLEAN:
                        case PCState::DIRTY:
                              break;
                        }
                  break;
            case PCState::EMPTY:
                  switch(newState) {
                        case PCState::INIT:
                        case PCState::EMPTY:
                              break;
                        case PCState::CLEAN:
                              setTitle(path);
                              run->setEnabled(true);
                              textEdit->setEnabled(true);
                              break;
                        case PCState::DIRTY:
                              return;
                        }
                  break;
            case PCState::CLEAN:
                  switch(newState) {
                        case PCState::INIT:
                        case PCState::EMPTY:
                        case PCState::CLEAN:
                              break;
                        case PCState::DIRTY:
                              actionSave->setEnabled(true);
                              break;
                        }
                  break;
            case PCState::DIRTY:
                  switch(newState) {
                        case PCState::INIT:
                        case PCState::EMPTY:
                        case PCState::CLEAN:
                              actionSave->setEnabled(false);
                        case PCState::DIRTY:
                              break;
                        }
                  break;
            }
      state = newState;
      }

//---------------------------------------------------------
//   setTitle
//---------------------------------------------------------

void PluginCreator::setTitle(const QString& s)
      {
      QString t(tr("MuseScore Plugin Creator"));
      if (s.isEmpty())
            setWindowTitle(t);
      else
            setWindowTitle(t + " - " + s);
      }

//---------------------------------------------------------
//   writeSettings
//---------------------------------------------------------

void PluginCreator::writeSettings()
      {
      QSettings settings;
      settings.beginGroup("PluginCreator");
      settings.setValue("geometry", saveGeometry());
      settings.setValue("windowState", saveState());
      settings.setValue("splitter", splitter->saveState());
      settings.endGroup();
      }

//---------------------------------------------------------
//   readSettings
//---------------------------------------------------------

void PluginCreator::readSettings()
      {
      if (!useFactorySettings) {
            QSettings settings;
            settings.beginGroup("PluginCreator");
            splitter->restoreState(settings.value("splitter").toByteArray());
            restoreGeometry(settings.value("geometry").toByteArray());
            restoreState(settings.value("windowState").toByteArray());
            settings.endGroup();
            }
      }

//---------------------------------------------------------
//   closeEvent
//---------------------------------------------------------

void PluginCreator::closeEvent(QCloseEvent* ev)
      {
      if (state == PCState::DIRTY) {
            QMessageBox::StandardButton n = QMessageBox::warning(this, tr("MuseScore"),
               tr("Plugin \"%1\" has changes.\n"
               "Save before closing?").arg(path),
               QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
               QMessageBox::Save);
            if (n == QMessageBox::Save)
                  savePlugin();
            else if (n == QMessageBox::Cancel) {
                  ev->ignore();
                  return;
                  }
            }
      emit closed(false);
      QWidget::closeEvent(ev);
      }

//---------------------------------------------------------
//   scMsgHandler
//---------------------------------------------------------
//

void scMsgHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
      QString s;
      switch (type) {
            case QtDebugMsg:
                  s = QString("Debug");
                  break;
            case QtWarningMsg:
                  s = QString("Warning");
                  break;
            case QtCriticalMsg:
                  s = QString("Critical");
                  break;
            case QtFatalMsg:
                  s = QString("Fatal");
                  break;
            }
       QString r = QString("%1: %2 (%3:%4, %5)\n").arg(s).arg(msg).arg(context.file).arg(context.line).arg(context.function);
       mscore->pluginCreator()->msg(r);
                  dock->show();
}

#ifdef LUA_SCRIPT_INTERFACE
void PluginCreator::runClicked()
      {
      log->clear();
      luaState = new QtLua::State();

      if (mscore->currentScore() && item->pluginType() != "dock")
            mscore->currentScore()->startCmd();
      lua_State->exec_statements(textEdit->toPlainText().toUtf8());
      if (mscore->currentScore() && item->pluginType() != "dock")
            mscore->currentScore()->endCmd();
      mscore->endCmd();
      }
#endif

//---------------------------------------------------------
//   closePlugin
//---------------------------------------------------------

void PluginCreator::closePlugin()
      {
      stop->setEnabled(false);
      run->setEnabled(true);
      if (view)
            view->close();
      if (dock)
            dock->close();
      qInstallMsgHandler(0);
      }

//---------------------------------------------------------
//   stopClicked
//---------------------------------------------------------

void PluginCreator::stopClicked()
      {
      closePlugin();
      }

//---------------------------------------------------------
//   loadPlugin
//---------------------------------------------------------

void PluginCreator::loadPlugin()
      {
      if (state == PCState::DIRTY) {
            QMessageBox::StandardButton n = QMessageBox::warning(this, tr("MuseScore"),
               tr("Plugin \"%1\" has changes.\n"
               "Save before closing?").arg(path),
               QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
               QMessageBox::Save);
            if (n == QMessageBox::Save)
                  savePlugin();
            else if (n == QMessageBox::Cancel)
                  return;
            }
      path = mscore->getPluginFilename(true);
      load();
      }

void PluginCreator::load()
      {
      if (path.isEmpty())
            return;
      QFile f(path);
      QFileInfo fi(f);
      if (f.open(QIODevice::ReadOnly)) {
            textEdit->setPlainText(f.readAll());
            run->setEnabled(true);
            f.close();
            }
      else {
            path = QString();
            }
      created = false;
      setState(PCState::CLEAN);
      setTitle( fi.baseName() );
      setToolTip(path);
      raise();
      }

//---------------------------------------------------------
//   savePlugin
//---------------------------------------------------------

void PluginCreator::savePlugin()
      {
      if (created) {
            path = mscore->getPluginFilename(false);
            if (path.isEmpty())
                  return;
            }
      QFile f(path);
      QFileInfo fi(f);
      if(fi.suffix() != "qml" ) {   // TODO - remove direct qml reference
            QMessageBox::critical(mscore, tr("MuseScore: Save Plugin"), tr("cannot determine file type"));
            return;
      }

      if (f.open(QIODevice::WriteOnly)) {
            f.write(textEdit->toPlainText().toUtf8());
            f.close();
            textEdit->document()->setModified(false);
            created = false;
            setState(PCState::CLEAN);
            setTitle( fi.baseName() );
            setToolTip(path);
            }
      else {
            // TODO
            }
      raise();
      }

//---------------------------------------------------------
//   newPlugin
//---------------------------------------------------------

void PluginCreator::newPlugin()
      {
      if (state == PCState::DIRTY) {
            QMessageBox::StandardButton n = QMessageBox::warning(this, tr("MuseScore"),
               tr("Plugin \"%1\" has changes.\n"
               "Save before closing?").arg(path),
               QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
               QMessageBox::Save);
            if (n == QMessageBox::Save)
                  savePlugin();
            else if (n == QMessageBox::Cancel)
                  return;
            }
      path    = tr("untitled");
      created = true;
      QString s(        // TODO remove direct QtQuick reference
         "import QtQuick 2.0\n"
         "import MuseScore 1.0\n"
         "\n"
         "MuseScore {\n"
         "      menuPath: \"Plugins.pluginName\"\n"
         "      onRun: {\n"
         "            console.log(\"hello world\")\n"
         "            Qt.quit()\n"
         "            }\n"
         "      }\n");
      textEdit->setPlainText(s);
      setState(PCState::CLEAN);
      setTitle(path);
      setToolTip(path);
      raise();
      }

//---------------------------------------------------------
//   textChanged
//---------------------------------------------------------

void PluginCreator::textChanged()
      {
      actionSave->setEnabled(textEdit->document()->isModified());
      setState(PCState::DIRTY);
      }

//---------------------------------------------------------
//   msg
//---------------------------------------------------------

void PluginCreator::msg(const QString& s)
      {
      log->moveCursor(QTextCursor::End);
      log->textCursor().insertText(s);
      }

//---------------------------------------------------------
//   showManual
//---------------------------------------------------------

void PluginCreator::showManual()
      {
      if (helpBrowser == 0) {
            helpBrowser = new HelpBrowser;
            manualDock = new QDockWidget(tr("Manual"), 0);
            manualDock->setObjectName("Manual");

            manualDock->setWidget(helpBrowser);
            Qt::DockWidgetArea area = Qt::RightDockWidgetArea;
            addDockWidget(area, manualDock);
            helpBrowser->setContent(manualPath());
            }
      manualDock->setVisible(!manualDock->isVisible());
      }
}
