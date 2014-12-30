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

#ifndef __SCRIPT_EDIT_H__
#define __SCRIPT_EDIT_H__

#include "globals.h"

namespace Ms {

class scriptHighlighter;

//---------------------------------------------------------
//   scriptEdit
//---------------------------------------------------------

class scriptEdit : public QPlainTextEdit {
      Q_OBJECT

      QWidget* lineNumberArea;
      scriptHighlighter* hl;
      ScoreState mscoreState;
      QString pickBuffer;

      virtual void focusInEvent(QFocusEvent*);
      virtual void focusOutEvent(QFocusEvent*);
      virtual void keyPressEvent(QKeyEvent*);
      void tab();

  private slots:
      void updateLineNumberAreaWidth(int);
      void highlightCurrentLine();
      void updateLineNumberArea(const QRect&, int);
      void startOfLine() { move(QTextCursor::StartOfLine); }
      void endOfLine()   { move(QTextCursor::EndOfLine); }
      void upLine()      { move(QTextCursor::Up); }
      void downLine();
      void right()       { move(QTextCursor::Right); }
      void left()        { move(QTextCursor::Left);  }
      void rightWord()   { move(QTextCursor::NextWord); }
      void start()       { move(QTextCursor::Start); }
      void end()         { move(QTextCursor::End);   }
      void leftWord();
      void pick();
      void put();
      void delLine();
      void delWord();

   protected:
      void resizeEvent(QResizeEvent*);
      void move(QTextCursor::MoveOperation);

   public:
      scriptEdit(QWidget* parent = 0);
      ~scriptEdit();
      void lineNumberAreaPaintEvent(QPaintEvent*);
      int lineNumberAreaWidth();
      enum ColorComponent { Normal, Comment, Number, String, Operator, Identifier,
         Keyword, BuiltIn, Marker };
      };

//---------------------------------------------------------
//   LineNumberArea
//---------------------------------------------------------

class LineNumberArea : public QWidget {
      Q_OBJECT
      scriptEdit* editor;

      QSize sizeHint() const {
            return QSize(editor->lineNumberAreaWidth(), 0);
            }
      void paintEvent(QPaintEvent* event) {
            editor->lineNumberAreaPaintEvent(event);
            }

   public:
      LineNumberArea(scriptEdit* parent) : QWidget(parent) { editor = parent; }
      };

//---------------------------------------------------------
//   scriptBlockData
//---------------------------------------------------------

class scriptBlockData : public QTextBlockUserData {
   public:
      QList<int> bracketPositions;
      };

//---------------------------------------------------------
//   scriptHighlighter
//---------------------------------------------------------

class scriptHighlighter : public QSyntaxHighlighter {
      QHash<scriptEdit::ColorComponent, QColor> m_colors;
      QString m_markString;
      Qt::CaseSensitivity m_markCaseSensitivity;

   protected:
      QSet<QString> m_keywords;
      QSet<QString> m_knownIds;

      void highlightBlock(const QString &text);
      void addKeywords(const char* keyWords[]);

   public:
      scriptHighlighter(QTextDocument *parent = 0);
      void colorScheme();
      void setColor(scriptEdit::ColorComponent component, const QColor &color);
      void mark(const QString &str, Qt::CaseSensitivity caseSensitivity);
      QStringList keywords() const;
      void setKeywords(const QStringList &keywords);
      };
} // namespace Ms
#endif
