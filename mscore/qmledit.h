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

#ifndef __QML_EDIT_H__
#define __QML_EDIT_H__

#include "scriptedit.h"

namespace Ms {

class JSHighlighter;

//---------------------------------------------------------
//   QmlEdit
//---------------------------------------------------------

class QmlEdit : public scriptEdit {
      Q_OBJECT

   public:
      QmlEdit(QWidget* parent = 0);
      ~QmlEdit();

      };

class JSHighlighter : public scriptHighlighter {
   public:
      JSHighlighter(QTextDocument *parent = 0);
      };

} // namespace Ms
#endif
