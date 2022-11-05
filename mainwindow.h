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


#include "staffarea.h"
#include "note.h"
#include "iobuffer.h"
#include "edit.h"
#include <QWidget>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QAudioInput>
#include <QSettings>
#include <QTimer>
#include <QRandomGenerator>
#include <QCheckBox>
#include <QLineEdit>
#include <QDateTime>


class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow();

protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    void saveSettings();
    void getSettings();
    void buildFontSizes();

public slots:
    void onInputDeviceChanged(int index);
    void onSensitivityChanged(int index);
    void onStringChanged(int index);
    void onStartStopPushed();
    void OnRevealCheckBoxStateChanged(int);
    void OnBufferFull();
    void onUpdateTimerElapsed();

private:
    QSettings settings;
    QList<QAudioDeviceInfo> deviceInfo;
    QAudioFormat formatAudio;
    int sampleRate;
    double sampleSeconds;
    StaffArea* pStaffArea;
    QComboBox* pDeviceBox;
    QPushButton* pStartButton;
    QPushButton* pExitButton;
    QLabel* pSensitivityLabel;
    QComboBox* pSensitivityBox;
    QLabel* pStringLabel;
    QComboBox* pStringBox;
    QAudioInput* pAudioInput;
    QList<QString> strings;
//    QCheckBox* pRevealCheckBox;
    QPushButton* pRevealCheckBox;
    bool bRevealChecked;
    QLabel* pScoreLabel;
    QLabel* pScoreEdit;
    int score;
    QLabel* pElapsedTimeLabel;
    QLabel* pElapsedTimeEdit;
    QTime elapsedTime;
    QLabel* pInputLabel;
    IOBuffer* pBuffer;
    char* pData;
    int chunkSize;
    int nData;
    int16_t* dataPointer;
    std::vector<Note> notes;
    int Lags;
    int ACORSLENGTH;
    int* acorLags;
    double* R;
    QRandomGenerator* pRandomGenerator;
    double threshold;
    QString sInputDevice;
    QTimer testTimer;
    QTimer updateTimer;
    int currentNote;
    int nDetections;
    int sensitivityIndex;
    int octaveIndex;
    int stringIndex;
    int currentString;
    int startNote, endNote, nFrets;
    QTime startTime;
    bool bFontBuilt;

    QString          sNormalStyle;
    QString          sErrorStyle;
    QString          sDarkStyle;
    QString          sSuccessStyle;
};
