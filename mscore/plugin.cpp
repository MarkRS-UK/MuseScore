//=============================================================================
//  MuseScore
//  Music Composition & Notation
//
//  Copyright (C) 2002-2014 Werner Schweer
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License version 2
//  as published by the Free Software Foundation and appearing in
//  the file LICENCE.GPL
//=============================================================================

#include "plugin.h"
#include "shortcut.h"
#include "musescoreCore.h"
#include "libmscore/score.h"

namespace Ms {

extern MuseScoreCore* mscoreCore;

//---------------------------------------------------------
//   plugin
//---------------------------------------------------------

#ifdef SCRIPT_INTERFACE
plugin::plugin( /* QQuickItem* parent */) {
      msc = mscoreCore;
      }

plugin::~plugin() {
      }

//---------------------------------------------------------
//   curScore
//---------------------------------------------------------

Score* plugin::curScore() const {
      return msc->currentScore();
      }

//---------------------------------------------------------
//   writeScore
//---------------------------------------------------------

bool plugin::writeScore(Score* s, const QString& name, const QString& ext) {
      if(!s)
            return false;
      return msc->saveAs(s, true, name, ext);
      }

//---------------------------------------------------------
//   readScore
//---------------------------------------------------------

Score* plugin::readScore(const QString& name) {
      Score * score = msc->openScore(name);

      return score;
      }

//---------------------------------------------------------
//   newElement
//---------------------------------------------------------

Ms::Element* plugin::newElement(int t) {
      Score* score = curScore();
      if (score == 0)
            return 0;
      Element* e = Element::create(Element::Type(t), score);

      return e;
      }

//---------------------------------------------------------
//   newScore
//---------------------------------------------------------

Score* plugin::newScore(const QString& name, const QString& part, int measures) {
      if (msc->currentScore()) {
            msc->currentScore()->endCmd();
            msc->endCmd();
            }
      Score* score = new Score(MScore::defaultStyle());
      score->setName(name);
      score->appendPart(part);
      score->appendMeasures(measures);
      score->doLayout();
      int view = msc->appendScore(score);
      msc->setCurrentView(0, view);
      qApp->processEvents();
      score->startCmd();
      return score;
      }

//---------------------------------------------------------
//   cmd
//---------------------------------------------------------

void plugin::cmd(const QString& s) {
      Shortcut* sc = Shortcut::getShortcut(qPrintable(s));
      if (sc)
            msc->cmd(sc->action());
      else
            qDebug("plugin:cmd: not found <%s>", qPrintable(s));
      }

//---------------------------------------------------------
//   newQProcess
//---------------------------------------------------------

MsProcess* plugin::newQProcess()
      {
      return 0; // TODO: new MsProcess(this);
      }


};

#endif
