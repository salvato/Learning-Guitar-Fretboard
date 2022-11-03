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

#include <QObject>
#include <QDialog>
#include <QSettings>
#include <QFont>
#include <QPushButton>
#include <QLineEdit>
#include <QDialogButtonBox>

class plotPropertiesDlg : public QDialog
{
    Q_OBJECT

public:
    plotPropertiesDlg(QString sTitle, QWidget *parent=Q_NULLPTR);
    void restoreSettings();

    QColor labelColor;
    QColor gridColor;
    QColor frameColor;
    QColor painterBkColor;

    int gridPenWidth;
    int maxDataPoints;
    QFont painterFont;

signals:
    void configChanged();

public slots:
    void onChangeBkColor();
    void onChangeFrameColor();
    void onChangeGridColor();
    void onChangeLabelsColor();
    void onChangeLabelsFont();
    void onChangeGridPenWidth(const QString sNewVal);
    void onChangeMaxDataPoints(const QString sNewVal);
    void onCancel();
    void onOk();

protected:
    void saveSettings();
    void initUI();
    void connectSignals();
    void setToolTips();

private:
    QString sTitleGroup;
    QString painterFontName;
    int painterFontSize;
    QFont::Weight painterFontWeight;
    bool painterFontItalic;
    // Buttons
    QPushButton BkColorButton;
    QPushButton frameColorButton;
    QPushButton gridColorButton;
    QPushButton labelColorButton;
    QPushButton labelFontButton;
    // Line Edit
    QLineEdit   gridPenWidthEdit;
    QLineEdit   maxDataPointsEdit;
    // QLineEdit styles
    QString sNormalStyle;
    QString sErrorStyle;
    // DialogButtonBox
    QDialogButtonBox* pButtonBox;
};

