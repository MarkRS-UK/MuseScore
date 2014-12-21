//=============================================================================
//  MuseScore
//  Music Composition & Notation
//
//  Copyright (C) 2014 Werner Schweer
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License version 2
//  as published by the Free Software Foundation and appearing in
//  the file LICENCE.GPL
//=============================================================================

#ifndef PLUGIN_H
#define PLUGIN_H

#include "config.h"

#ifdef SCRIPT_INTERFACE
#include "libmscore/mscore.h"
#include "musescoreCore.h"

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
//   Plugin
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

class plugin {

   protected:
      MuseScoreCore* msc;
      QString _menuPath;
      QString _pluginType;
      QString _dockArea;
      QString _version;
      QString _description;

   public:
      plugin();
      ~plugin();

      void setMenuPath(const QString& s)   { _menuPath = s;    }
      QString menuPath() const             { return _menuPath; }
      void setVersion(const QString& s)    { _version = s; }
      QString version() const              { return _version; }
      void setDescription(const QString& s) { _description = s; }
      QString description() const          { return _description; }
      void setPluginType(const QString& s) { _pluginType = s;    }
      QString pluginType() const           { return _pluginType; }
      void setDockArea(const QString& s)   { _dockArea = s;    }
      QString dockArea() const             { return _dockArea; }

      virtual void runPlugin()            { }

      int division() const                { return MScore::division; }
      int mscoreVersion() const           { return Ms::version();      }
      int mscoreMajorVersion() const      { return majorVersion();  }
      int mscoreMinorVersion() const      { return minorVersion();  }
      int mscoreUpdateVersion() const     { return updateVersion(); }
      qreal mscoreDPI() const             { return MScore::DPI;     }

      Score* curScore() const;
      QList<Score *>& scores()            { return msc->scores(); }

      Ms::Score* newScore(const QString& name, const QString& part, int measures);
      Ms::Element* newElement(int);
      void cmd(const QString&);
      Ms::MsProcess* newQProcess();
      bool writeScore(Ms::Score*, const QString& name, const QString& ext);
      Ms::Score* readScore(const QString& name);

      };
} // namespace Ms
#endif

#endif // PLUGIN_H
