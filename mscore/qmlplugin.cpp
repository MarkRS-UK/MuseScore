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

#ifdef QML_SCRIPT_INTERFACE
// #include <QQmlEngine>
#include <QtQml>
#endif

#ifdef LUA_SCRIPT_INTERFACE
#include <QtLua/String>
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
//   scores
//---------------------------------------------------------

#ifdef QML_SCRIPT_INTERFACE
QQmlListProperty<Score> QmlPlugin::scores()
      {
      return QQmlListProperty<Score>(this, plugin::scores());
      }
#endif
#ifdef LUA_SRIPT_INTERFACE
QList<Score> QmlPlugin::scores() {
      return QList<Score>(this, msc->scores());
      }
#endif

//---------------------------------------------------------
//   readScore
//---------------------------------------------------------

Score* QmlPlugin::readScore(const QString& name)
      {
//      Score * score = plugin::openScore(name);
      Score * score = plugin::readScore(name);
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
      Element* e = plugin::newElement(t);

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
      Score* score = plugin::newScore(name, part, measures);

#ifdef QML_SCRIPT_INTERFACE
      // tell QML not to garbage collect this score
      QQmlEngine::setObjectOwnership(score, QQmlEngine::CppOwnership);
#endif
//      score->startCmd();
      return score;
      }

}
#endif
