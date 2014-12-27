//=============================================================================
//  MuseScore
//  Music Composition & Notation
//
//  Copyright (C) 2012 Werner Schweer
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License version 2
//  as published by the Free Software Foundation and appearing in
//  the file LICENCE.GPL
//=============================================================================

#ifndef __QMLPLUGIN_H__
#define __QMLPLUGIN_H__

#include "config.h"

#ifdef SCRIPT_INTERFACE
#include "plugin.h"
#include "libmscore/mscore.h"

namespace Ms {

class MsProcess;
class Score;
class Element;
class MScore;
class MuseScoreCore;

extern int version();
extern int majorVersion();
extern int minorVersion();
extern int updateVersion();

//---------------------------------------------------------
//   QmlPlugin
//   @@ MuseScore
//   @P menuPath             QString           where the plugin is placed in menu
//   @P version              QString
//   @P description          QString
//   @P pluginType           QString
//   @P dockArea             QString
//   @P division             int               number of MIDI ticks for 1/4 note (read only)
//   @P mscoreVersion        int               complete version number of MuseScore in the form: MMmmuu (read only)
//   @P mscoreMajorVersion   int               1st part of the MuseScore version (read only)
//   @P mscoreMinorVersion   int               2nd part of the MuseScore version (read only)
//   @P mscoreUpdateVersion  int               3rd part of the MuseScore version (read only)
//   @P mscoreDPI            qreal             (read only)
//   @P curScore             Ms::Score*        current score, if any (read only)
//   @P scores               array[Ms::Score]  all currently open scores (read only)
//---------------------------------------------------------

class QmlPlugin : public QQuickItem , public plugin {
      Q_OBJECT
      Q_PROPERTY(QString menuPath        READ menuPath WRITE setMenuPath)
      Q_PROPERTY(QString version         READ version WRITE setVersion)
      Q_PROPERTY(QString description     READ description WRITE setDescription)
      Q_PROPERTY(QString pluginType      READ pluginType WRITE setPluginType)

      Q_PROPERTY(QString dockArea        READ dockArea WRITE setDockArea)
      Q_PROPERTY(int division            READ division)
      Q_PROPERTY(int mscoreVersion       READ mscoreVersion)
      Q_PROPERTY(int mscoreMajorVersion  READ mscoreMajorVersion)
      Q_PROPERTY(int mscoreMinorVersion  READ mscoreMinorVersion)
      Q_PROPERTY(int mscoreUpdateVersion READ mscoreUpdateVersion)
      Q_PROPERTY(qreal mscoreDPI         READ mscoreDPI)
      Q_PROPERTY(Ms::Score* curScore     READ curScore)
      Q_PROPERTY(QQmlListProperty<Ms::Score> scores READ scores)

   signals:
      void run();

   public:
      QmlPlugin(QQuickItem* parent = 0);
      ~QmlPlugin();

      QQmlListProperty<Score> scores();

      void runPlugin()                     { emit run();       }

      Q_INVOKABLE Ms::Score* newScore(const QString& name, const QString& part, int measures);
      Q_INVOKABLE Ms::Element* newElement(int);
      Q_INVOKABLE Ms::Score* readScore(const QString& name);
      };

#endif

} // namespace Ms
#endif
