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
SOFTWARE.*
*/
#pragma once

#include <QDialog>

#include "AxisLimits.h"


QT_FORWARD_DECLARE_CLASS(QDialogButtonBox)
QT_FORWARD_DECLARE_CLASS(QLineEdit)
QT_FORWARD_DECLARE_CLASS(QCheckBox)


class AxesDialog : public QDialog
{
  Q_OBJECT

public:
    explicit AxesDialog(QWidget *parent = Q_NULLPTR);
    ~AxesDialog();
    void initDialog(AxisLimits AxisLimits);

private:
    QLineEdit        *pEditXMin;
    QLineEdit        *pEditXMax;
    QLineEdit        *pEditYMin;
    QLineEdit        *pEditYMax;
    QCheckBox        *pAutoX;
    QCheckBox        *pAutoY;
    QCheckBox        *pLogX;
    QCheckBox        *pLogY;
    QDialogButtonBox *pButtonBox;

public:
    AxisLimits newLimits;

private slots:
    void onButtonBoxAccepted();
    void onButtonBoxRejected();
};

