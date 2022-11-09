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

#include "staffarea.h"

#include <QPainter>
#include <QPainterPath>
#include <QMessageBox>
#include <QDebug>


// Per rendere trasparente lo sfondo bianco di una immagine
// tramite linea di comando:
// $ convert original.png -transparent white transparent.png

StaffArea::StaffArea(QWidget *parent)
    : QWidget(parent)
    , xBound(10)
    , yTop(20)
    , lineSpace(20)
    , noteNum(-1)
    , octaveBase(1)
    , bRevealNote(false)
{
    chiave.load(":/ChiaveViolino.png");
    chiave = chiave.scaled(4*lineSpace, 5*lineSpace);

    semibreve.load(":/Semibreve.png");
    semibreve = semibreve.scaled(lineSpace, lineSpace);

    diesis.load(":/Diesis.png");
    diesis = diesis.scaled(lineSpace, lineSpace);

    brushStyle = Qt::BrushStyle(Qt::BrushStyle::SolidPattern);
    brush = QBrush(Qt::black, brushStyle);

    penWidth = 1;
    penStyle = Qt::PenStyle(Qt::PenStyle::SolidLine);
    penCap   = Qt::PenCapStyle(Qt::PenCapStyle::FlatCap);
    penJoin  = Qt::PenJoinStyle(Qt::PenJoinStyle::RoundJoin);
    pen = QPen(Qt::black, penWidth, penStyle, penCap, penJoin);

    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
}


QSize
StaffArea::minimumSizeHint() const
{
    return QSize(10*lineSpace+2*xBound, 12*lineSpace+2*yTop);
}


QSize
StaffArea::sizeHint() const
{
    return QSize(600, 12*lineSpace+2*yTop);
}


void
StaffArea::setOctaveBase(int iOctave) {
    octaveBase = iOctave;
}


void
StaffArea::setNote(Note newNote, int noteIndex) {
    noteNum = noteIndex;
    if(noteNum >= 0) {
        note = newNote;
        octave = noteIndex/12-octaveBase;
    }
    update();
}


void
StaffArea::paintEvent(QPaintEvent* /* event */) {
    QPainter painter(this);
    painter.setPen(pen);
    painter.setBrush(brush);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QFont font = painter.font();
    font.setPixelSize(height()/12);
    painter.setFont(font);

    // Draw the staff
    painter.drawImage(xBound, 3*lineSpace, chiave);
    for(int i=2; i<7; i++) {
        y = i*lineSpace+yTop;
        painter.drawLine(QPoint(xBound, y), QPoint(width()-xBound, y));
    }

    if(noteNum < 0) return; // noteNum < 0 means No Note To Display...

    switch(noteNum % 12) { // The Big Switch to Handle the Musical Notes
        case 0: // C
            handleC(&painter);
            break;
        case 1: // C#
            handleCsharp(&painter);
            break;
        case 2: // D
            handleD(&painter);
            break;
        case 3: // D#
            handleDsharp(&painter);
            break;
        case 4: // E
            handleE(&painter);
            break;
        case 5: // F
            handleF(&painter);
            break;
        case 6: // F#
            handleFsharp(&painter);
            break;
        case 7: // G
            handleG(&painter);
            break;
        case 8: // G#
            handleGsharp(&painter);
            break;
        case 9: // A
            handleA(&painter);
            break;
        case 10: // A#
            handleAsharp(&painter);
            break;
        case 11: // B
            handleB(&painter);
            break;
        default:
            errorMessage(QString("%1 Line %2").arg(__FUNCTION__).arg(__LINE__));
            return;
    }
    if(bRevealNote) {
        painter.drawText(QRect(4*lineSpace, height()-(height()/12), width(), 3*lineSpace),
                         Qt::AlignLeft,
                         note.sname);
    }
}


// Sbaglia a disegnare C3 con corda E
void
StaffArea::handleC(QPainter* painter){
    if(octave < 2) {
        return;
    }
    x = (width()+xBound)/2;
    if(octave == 2) {
        y = 7*lineSpace+yTop;
        painter->drawLine(QPoint(x-lineSpace, y),
                          QPoint(x+2*lineSpace, y));
    }
    else if(octave == 3) {
        y = 3.5*lineSpace+yTop;
    }
    else if(octave == 4) {
        y = yTop;
        painter->drawLine(QPoint(x-lineSpace, y),
                          QPoint(x+2*lineSpace, y));
        painter->drawLine(QPoint(x-lineSpace, y+lineSpace),
                          QPoint(x+2*lineSpace, y+lineSpace));
    }
    else {
        errorMessage(QString("%1 Line %2").arg(__FUNCTION__).arg(__LINE__));
        return;
    }
    painter->drawImage(x, y-lineSpace/2, semibreve);
}

// Sbaglia a disegnare C#3 con corda E
void
StaffArea::handleCsharp(QPainter* painter) {
    if(octave < 2) {
        return;
    }
    x = (width()+xBound)/2 - lineSpace;
    if(octave == 2) {
        y = 7*lineSpace+yTop;
        painter->drawLine(QPoint(x-lineSpace, y),
                          QPoint(x+3*lineSpace, y));
    }
    else if(octave == 3) {
        y = 3.5*lineSpace+yTop;
    }
    else if(octave == 4) {
        y = yTop;
        painter->drawLine(QPoint(x-lineSpace, y),
                          QPoint(x+3*lineSpace, y));
        painter->drawLine(QPoint(x-lineSpace, y+lineSpace),
                          QPoint(x+3*lineSpace, y+lineSpace));
    }
    else {
        errorMessage(QString("%1 Line %2").arg(__FUNCTION__).arg(__LINE__));
        return;
    }
    painter->drawImage(x, y-lineSpace/2, diesis);
    x = (width()+xBound)/2;
    painter->drawImage(x, y-lineSpace/2, semibreve);
}


void
StaffArea::handleD(QPainter* painter) {
    if(octave < 2) {
        return;
    }
    x = (width()+xBound)/2;
    if(octave == 2) {
        y = 6.5*lineSpace+yTop;
    }
    else if(octave == 3) {
        y = 3*lineSpace+yTop;
    }
    else {//(octave == 4)
        errorMessage(QString("%1 Line %2").arg(__FUNCTION__).arg(__LINE__));
        return;
    }
    painter->drawImage(x, y-lineSpace/2, semibreve);
}


void
StaffArea::handleDsharp(QPainter* painter) {
    if(octave < 2) {
        return;
    }
    x = (width()+xBound)/2 - lineSpace;
    if(octave == 2) {
        y = 6.5*lineSpace+yTop;
    }
    else if(octave == 3) {
        y = 3*lineSpace+yTop;
    }
    else {//(octave == 4)
        errorMessage(QString("%1 Line %2").arg(__FUNCTION__).arg(__LINE__));
        return;
    }
    painter->drawImage(x, y-lineSpace/2, diesis);
    x = (width()+xBound)/2;
    painter->drawImage(x, y-lineSpace/2, semibreve);
}


void
StaffArea::handleE(QPainter* painter) {
    if(octave < 2) {
        return;
    }
    x = (width()+xBound)/2;
    if(octave == 2) {
        y = 6*lineSpace+yTop;
    }
    else if(octave == 3) {
        y = 2.5*lineSpace+yTop;
    }
    else {//(octave == 4)
        errorMessage(QString("%1 Line %2").arg(__FUNCTION__).arg(__LINE__));
        return;
    }
    painter->drawImage(x, y-lineSpace/2, semibreve);
}


void
StaffArea::handleF(QPainter* painter) {
    if(octave < 2) {
        return;
    }
    x = (width()+xBound)/2;
    if(octave == 2) {
        y = 5.5*lineSpace+yTop;
    }
    else if(octave == 3) {
        y = 2*lineSpace+yTop;
    }
    else {//(octave == 4)
        errorMessage(QString("%1 Line %2").arg(__FUNCTION__).arg(__LINE__));
        return;
    }
    painter->drawImage(x, y-lineSpace/2, semibreve);
}


void
StaffArea::handleFsharp(QPainter* painter) {
    if(octave < 2) {
        return;
    }
    x = (width()+xBound)/2 - lineSpace;
    if(octave == 2) {
        y = 5.5*lineSpace+yTop;
    }
    else if(octave == 3) {
        y = 2*lineSpace+yTop;
    }
    else {//(octave == 4)
        errorMessage(QString("%1 Line %2").arg(__FUNCTION__).arg(__LINE__));
        return;
    }
    painter->drawImage(x, y-lineSpace/2, diesis);
    x = (width()+xBound)/2;
    painter->drawImage(x, y-lineSpace/2, semibreve);
}


void
StaffArea::handleG(QPainter* painter) {
    if(octave < 2) {
        return;
    }
    x = (width()+xBound)/2;
    if(octave == 2) {
        y = 5*lineSpace+yTop;
    }
    else if(octave == 3) {
        y = 1.5*lineSpace+yTop;
    }
    else {//(octave == 4)
        errorMessage(QString("%1 Line %2").arg(__FUNCTION__).arg(__LINE__));
        return;
    }
    painter->drawImage(x, y-lineSpace/2, semibreve);
}


void
StaffArea::handleGsharp(QPainter* painter) {
    if(octave < 2) {
        return;
    }
    x = (width()+xBound)/2 - lineSpace;
    if(octave == 2) {
        y = 5*lineSpace+yTop;
    }
    else if(octave == 3) {
        y = 1.5*lineSpace+yTop;
    }
    else {//(octave == 4)
        errorMessage(QString("%1 Line %2").arg(__FUNCTION__).arg(__LINE__));
        return;
    }
    painter->drawImage(x, y-lineSpace/2, diesis);
    x = (width()+xBound)/2;
    painter->drawImage(x, y-lineSpace/2, semibreve);
}


void
StaffArea::handleA(QPainter* painter) {
    if(octave < 1) {
        return;
    }
    x = (width()+xBound)/2;
    if(octave == 1) {
        y = 8*lineSpace+yTop;
        painter->drawLine(QPoint(x-lineSpace, y-lineSpace),
                          QPoint(x+2*lineSpace, y-lineSpace));
        painter->drawLine(QPoint(x-lineSpace, y),
                          QPoint(x+2*lineSpace, y));
    }
    else if(octave == 2) {
        y = 4.5*lineSpace+yTop;
    }
    else if(octave == 3) {
        y = lineSpace+yTop;
        painter->drawLine(QPoint(x-lineSpace, y),
                          QPoint(x+2*lineSpace, y));
    }
    else {//(octave == 4)
        errorMessage(QString("%1 Line %2").arg(__FUNCTION__).arg(__LINE__));
        return;
    }
    painter->drawImage(x, y-lineSpace/2, semibreve);
}


void
StaffArea::handleAsharp(QPainter* painter) {
    if(octave < 1) {
        return;
    }
    x = (width()+xBound)/2 - lineSpace;
    if(octave == 1) {
        y = 8*lineSpace+yTop;
        painter->drawLine(QPoint(x-lineSpace, y-lineSpace),
                          QPoint(x+3*lineSpace, y-lineSpace));
        painter->drawLine(QPoint(x-lineSpace, y),
                          QPoint(x+3*lineSpace, y));
    }
    else if(octave == 2) {
        y = 4.5*lineSpace+yTop;
    }
    else if(octave == 3) {
        y = lineSpace+yTop;
        painter->drawLine(QPoint(x-lineSpace, y),
                          QPoint(x+3*lineSpace, y));
    }
    else { // (octave == 4)
        errorMessage(QString("%1 Line %2").arg(__FUNCTION__).arg(__LINE__));
        return;
    }
    painter->drawImage(x, y-lineSpace/2, diesis);
    x = (width()+xBound)/2;
    painter->drawImage(x, y-lineSpace/2, semibreve);
}


void
StaffArea::handleB(QPainter* painter) {
    if(octave < 1) {
        return;
    }
    x = (width()+xBound)/2;
    if(octave == 1) {
        y = 7.5*lineSpace+yTop;
        painter->drawLine(QPoint(x-lineSpace, y-lineSpace/2),
                          QPoint(x+2*lineSpace,y-lineSpace/2));
    }
    else if(octave == 2) {
        y = 4*lineSpace+yTop;
    }
    else if(octave == 3) {
        y = lineSpace/2+yTop;
        painter->drawLine(QPoint(x-lineSpace, y+lineSpace/2),
                          QPoint(x+2*lineSpace, y+lineSpace/2));
    }
    else {
        errorMessage(QString("%1 Line %2").arg(__FUNCTION__).arg(__LINE__));
        return;
    }
    painter->drawImage(x, y-lineSpace/2, semibreve);
}


void
StaffArea::setRevealNote(bool bReveal) {
    bRevealNote = bReveal;
}


void
StaffArea::errorMessage(QString sError) {
    qDebug() << sError;
//    QMessageBox msgBox;
//    msgBox.setInformativeText("Arresta il Programma:");
//    msgBox.setText(sError);
//    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
//    msgBox.setDefaultButton(QMessageBox::No);
//    int ret = msgBox.exec();
//    if(ret == QMessageBox::Yes) {
//        exit(EXIT_FAILURE);
//    }
}
