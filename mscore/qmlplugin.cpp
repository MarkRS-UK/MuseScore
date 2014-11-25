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

#include "config.h"
#ifdef SCRIPT_INTERFACE

#include "qmlplugin.h"
#include "shortcut.h"
#include "libmscore/musescoreCore.h"
#include "libmscore/score.h"

#ifdef QML_SCRIPT_INTERFACE
#include <QQmlEngine>
#endif

namespace Ms {

// extern MuseScoreCore* mscoreCore;

//---------------------------------------------------------
//   QmlPlugin
//---------------------------------------------------------

#ifdef QML_SCRIPT_INTERFACE
QmlPlugin::QmlPlugin(QQuickItem* parent)
   : QQuickItem(parent)
      {
      msc = MuseScoreCore::mscoreCore;
      }
#endif

#ifdef LUA_SCRIPT_INTERFACE
QmlPlugin::QmlPlugin(QObject* parent)
      {
      msc = mscoreCore;
      }
#endif

QmlPlugin::~QmlPlugin()
      {
      }

//---------------------------------------------------------
//   curScore
//---------------------------------------------------------

Score* QmlPlugin::curScore() const
      {
      return msc->currentScore();
      }

//---------------------------------------------------------
//   scores
//---------------------------------------------------------

#ifdef QML_SCRIPT_INTERFACE
QQmlListProperty<Score> QmlPlugin::scores()
      {
      return QQmlListProperty<Score>(this, msc->scores());
      }
#endif
#ifdef LUA_SRIPT_INTERFACE
QList<Score> QmlPlugin::scores() {
      return QList<Score>(this, msc->scores());
      }
#endif

//---------------------------------------------------------
//   writeScore
//---------------------------------------------------------

bool QmlPlugin::writeScore(Score* s, const QString& name, const QString& ext)
      {
      if(!s)
            return false;
      return msc->saveAs(s, true, name, ext);
      }

//---------------------------------------------------------
//   readScore
//---------------------------------------------------------

Score* QmlPlugin::readScore(const QString& name)
      {
      Score * score = msc->openScore(name);
#ifdef QML_SCRIPT_INTERFACE
      // tell QML not to garbage collect this score
      if (score)
            QQmlEngine::setObjectOwnership(score, QQmlEngine::CppOwnership);
      return score;
#endif
      }

//---------------------------------------------------------
//   newElement
//---------------------------------------------------------

Ms::Element* QmlPlugin::newElement(int t)
      {
      Score* score = curScore();
      if (score == 0)
            return 0;
      Element* e = Element::create(Element::Type(t), score);
#ifdef QML_SCRIPT_INTERFACE
      // tell QML not to garbage collect this score
      Ms::MScore::qml()->setObjectOwnership(e, QQmlEngine::CppOwnership);
#endif
      return e;
      }

//---------------------------------------------------------
//   newScore
//---------------------------------------------------------

Score* QmlPlugin::newScore(const QString& name, const QString& part, int measures)
      {
      if (msc->currentScore())
            msc->currentScore()->endCmd();
      Score* score = new Score(MScore::defaultStyle());
      score->setName(name);
      score->appendPart(part);
      score->appendMeasures(measures);
      score->doLayout();
      int view = msc->appendScore(score);
      msc->setCurrentView(0, view);
      qApp->processEvents();
#ifdef QML_SCRIPT_INTERFACE
      // tell QML not to garbage collect this score
      QQmlEngine::setObjectOwnership(score, QQmlEngine::CppOwnership);
#endif
      score->startCmd();
      return score;
      }

//---------------------------------------------------------
//   cmd
//---------------------------------------------------------

void QmlPlugin::cmd(const QString& s)
      {
      Shortcut* sc = Shortcut::getShortcut(qPrintable(s));
      if (sc)
            msc->cmd(sc->action());
      else
            qDebug("QmlPlugin:cmd: not found <%s>", qPrintable(s));
      }

//---------------------------------------------------------
//   newQProcess
//---------------------------------------------------------

MsProcess* QmlPlugin::newQProcess()
      {
      return 0; // TODO: new MsProcess(this);
      }
}
#endif
