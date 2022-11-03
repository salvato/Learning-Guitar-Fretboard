/*
MIT License

Copyright (c) 2022 salvato

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include "note.h"

#include <QWidget>
#include <QBrush>
#include <QPen>
#include <QPixmap>
#include <QImage>


class StaffArea : public QWidget
{
    Q_OBJECT
public:
    explicit StaffArea(QWidget *parent = nullptr);
    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;
    void setNote(Note note, int noteIndex);
    void setSensitivity(double sensitivity);
    void setOctaveBase(int iOctave);
    void setRevealNote(bool bReveal);

signals:

protected:
    void paintEvent(QPaintEvent *event) override;
    void errorMessage(QString sError);
    void handleC(QPainter* painter);
    void handleCsharp(QPainter* painter);
    void handleD(QPainter* painter);
    void handleDsharp(QPainter* painter);
    void handleE(QPainter* painter);
    void handleF(QPainter* painter);
    void handleFsharp(QPainter* painter);
    void handleG(QPainter* painter);
    void handleGsharp(QPainter* painter);
    void handleA(QPainter* painter);
    void handleAsharp(QPainter* painter);
    void handleB(QPainter* painter);

private:
    QImage chiave;
    QImage semibreve;
    QImage diesis;
    QPen pen;
    QBrush brush;
    QImage pixmap;
    int penWidth;
    Qt::PenStyle penStyle;
    Qt::PenCapStyle penCap;
    Qt::PenJoinStyle penJoin;
    Qt::BrushStyle brushStyle;
    int xBound;
    int yTop;
    int lineSpace;
    Note note;
    int noteNum;
    int octave;
    int octaveBase;
    bool bRevealNote;
    int x, y;
};
