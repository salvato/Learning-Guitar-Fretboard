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

#include "mainwindow.h"

#include <QtWidgets>
#include <QMediaDevices>


// Mio cell 360x717
// Tablet Acer 1280x752

MainWindow::MainWindow()
    : QWidget()
    , sampleRate(48000)
    , sampleSeconds(0.3)
    , pStaffArea(new StaffArea())
    , pDeviceBox(new QComboBox())
    , pStartButton(new QPushButton("Start"))
    , pExitButton(new QPushButton("Exit"))
    , pSensitivityLabel(new QLabel("   Sensitivity"))
    , pSensitivityBox(new QComboBox())
    , pStringLabel(new QLabel("String"))
    , pStringBox(new QComboBox())
    , pAudioInput(nullptr)
    , pRevealButton(new QPushButton("Show Note"))
    , bRevealChecked(false)
    , pScoreLabel(new QLabel("Score"))
    , pScoreEdit(new QLabel(""))
    , score(0)
    , pElapsedTimeLabel(new QLabel("Time"))
    , pElapsedTimeEdit(new QLabel("00:00:00"))
    , pInputLabel(new QLabel("Input Device"))
    , pData(nullptr)
    , chunkSize(sampleRate*sampleSeconds)
    , pRandomGenerator(QRandomGenerator::system())
    , threshold(5.0)
    , updateTime(1000)
    , timeToWait(1000)
    , nDetections(0)
    , nFrets(12) // Only first 12 Frets (22 on Guitars Like Fender Stratocaster)
{
    pRandomGenerator->securelySeeded();
    pRevealButton->setCheckable(true);
    setWindowTitle(tr("Note Learning"));

    // Notes definition (on an external File to simplify program reading)
    #include "noteDefinition.h" // IWYU pragma: keep (To Suppress Unused Warning)

    // Get the last Saved Settings
    getSettings();

    // Setup the styles
    sNormalStyle  = pScoreEdit->styleSheet();
    sErrorStyle   = "QLabel { color: rgb(255, 255, 255); background: rgb(255, 0, 0); selection-background-color: rgb(128, 128, 255); }";
    sDarkStyle    = "QLabel { color: rgb(255, 255, 255); background: rgb(0, 0, 0); selection-background-color: rgb(128, 128, 255); }";
    sSuccessStyle = "QLabel { color: rgb(0, 0, 0); background: rgb(255, 255, 0); selection-background-color: rgb(128, 128, 255); }";

    // Setup Audio Data Buffer
    pData = new char[chunkSize];
    dataPointer = (int16_t*)(pData);
    nData = chunkSize/int(sizeof(int16_t));
    pBuffer = new IOBuffer(pData, chunkSize, this);
    connect(pBuffer, SIGNAL(bufferFull()),
            this, SLOT(OnBufferFull()));

    // Audio Devices ComboBox handling
    // Fills the ComboBox with a list of audio devices that support AudioInput
    deviceInfo = QMediaDevices::audioInputs();
    for(int i=0; i<deviceInfo.count(); i++) {
        pDeviceBox->addItem(deviceInfo.at(i).description(), QVariant::fromValue(deviceInfo.at(i)));
    }
    // if still connected, use the previous saved device
    int index = pDeviceBox->findText(sInputDevice);
    if(index == -1) index = 0;
    pDeviceBox->setCurrentIndex(index);

    // Sensitivity ComboBox handling
    pSensitivityLabel->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    for(int i=1; i<10; i++)
        pSensitivityBox->addItem(QString("%1").arg(10-i));
    pSensitivityBox->setCurrentIndex(sensitivityIndex);
    onSensitivityChanged(sensitivityIndex);

    // Starting Octave ComboBox handling
    pStringLabel->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    strings = {"E", "A", "D", "G", "B", "e"};
    pStringBox->addItems(strings);
    pStringBox->setCurrentIndex(currentString);
    onStringChanged(currentString);

    pRevealButton->setChecked(bRevealChecked);
    pStaffArea->setRevealNote(bRevealChecked);

    pScoreLabel->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    pScoreEdit->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    pScoreEdit->setText(QString("%1").arg(score));

    elapsedTime = QTime(0, 0, 0, 0);
    pElapsedTimeLabel->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    pElapsedTimeEdit->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    pElapsedTimeEdit->setText(elapsedTime.toString());

    // MainWindow Layout
    QGridLayout *mainLayout = new QGridLayout;

    QWidget *lineB = new QWidget;
    lineB->setFixedHeight(2);
    lineB->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    lineB->setStyleSheet(QString("background-color: rgb(192, 192, 192);"));

    mainLayout->addWidget(pInputLabel,       0, 0, 1, 1, Qt::AlignRight);
    mainLayout->addWidget(pDeviceBox,        0, 1, 1, 5);

    mainLayout->addWidget(pStaffArea,        1, 0, 3, 6);

    mainLayout->addWidget(pStringLabel,      4, 0, 1, 1, Qt::AlignHCenter|Qt::AlignBottom);
    mainLayout->addWidget(pScoreLabel,       4, 2, 1, 2, Qt::AlignHCenter);
    mainLayout->addWidget(pElapsedTimeLabel, 4, 4, 1, 2, Qt::AlignHCenter|Qt::AlignBottom);

    mainLayout->addWidget(pStringBox,        5, 0, 1, 1, Qt::AlignHCenter|Qt::AlignTop);
    mainLayout->addWidget(pScoreEdit,        5, 2, 1, 2);
    mainLayout->addWidget(pElapsedTimeEdit,  5, 4, 1, 2, Qt::AlignHCenter|Qt::AlignTop);

    mainLayout->addWidget(pRevealButton,     6, 0, 1, 1);
    mainLayout->addWidget(pSensitivityLabel, 6, 2, 1, 2, Qt::AlignRight);
    mainLayout->addWidget(pSensitivityBox,   6, 4, 1, 2, Qt::AlignLeft);

    mainLayout->addWidget(lineB,             7, 0, 1, 6);

    mainLayout->addWidget(pExitButton,       8, 0, 1, 1);
    mainLayout->addWidget(pStartButton,      8, 4, 1, 2);

    setLayout(mainLayout);

    // UI Message handlers
    connect(pExitButton, SIGNAL(clicked()),
            this, SLOT(onExitPushed()));
    connect(pStartButton, SIGNAL(clicked()),
            this, SLOT(onStartStopPushed()));
    connect(pDeviceBox, SIGNAL(activated(int)),
            this, SLOT(onInputDeviceChanged(int)));
    connect(pSensitivityBox, SIGNAL(activated(int)),
            this, SLOT(onSensitivityChanged(int)));
    connect(pStringBox, SIGNAL(activated(int)),
            this, SLOT(onStringChanged(int)));
    connect(pRevealButton, SIGNAL(clicked()),
            this, SLOT(OnRevealCheckBoxStateChanged()));

    // Create the QAudioInput object that represents an input channel.
    // It enables the selection of the physical input device to be used.
    // It allows muting the channel and changing the channel's volume.
    pAudioInput = new QAudioInput(deviceInfo.at(pDeviceBox->currentIndex()), this);

    // Define requested Audio Format
    formatAudio.setSampleRate(sampleRate);
    formatAudio.setChannelCount(1);
    formatAudio.setSampleFormat(QAudioFormat::Int16);

    // Create the Audio Input Source with the specified QAudioDevice
    // also sending the QAudioFormat to be used for the acquisition.
    pAudioSource = new  QAudioSource(deviceInfo.at(pDeviceBox->currentIndex()), formatAudio);
    pAudioSource->setBufferSize(sampleRate*sampleSeconds);

    // Computing the delays where calculate the autocorrelation function
    // and zeroing the autocorrelation function
    Lags        = notes.size() + 1;
    acorLags    = new int[Lags];
    R           = new double[Lags];
    acorLags[0] = 0;
    R[0]        = 0; // R[0]= Energia del Segnale
    for(int i=1; i<Lags; i++) {
        // Autororrelation Indexes corresponding to Note Periods
        acorLags[i] = int((double)sampleRate/notes[i-1].frequency+0.5);
        R[i]        = 0;
    }

    // Setup of the timer for update the Running Time
    updateTimer.setTimerType(Qt::PreciseTimer);
    connect(&updateTimer, SIGNAL(timeout()),
            this, SLOT(onUpdateTimerElapsed()));
    connect(&waitTimer, SIGNAL(timeout()),
            this, SLOT(onWaitTimerElapsed()));
}


void
MainWindow::closeEvent(QCloseEvent *event) {
    updateTimer.stop();
    waitTimer.stop();
    if(pAudioInput) {
        pAudioSource->stop();
        delete pAudioInput;
    }
    saveSettings();
    if(pBuffer) {
        pBuffer->close();
        delete pBuffer;
    }
    if(pData) delete[] pData;
    delete acorLags;
    delete R;
    QWidget::closeEvent(event);// Propagate the event
}


void
MainWindow::getSettings() {
    restoreGeometry(settings.value(QString("MainWindow_Dialog")).toByteArray());
    sInputDevice     = settings.value(QString("Input_Device"), QString("default")).toString();
    sensitivityIndex = settings.value(QString("Sensitivity"),  QString("4")).toInt();
    currentString    = settings.value(QString("String"),       QString("0")).toInt();
    bRevealChecked   = settings.value(QString("Reveal"),       QString("true")).toBool();
}


void
MainWindow::saveSettings() {
    settings.setValue(QString("MainWindow_Dialog"), saveGeometry());
    settings.setValue(QString("Input_Device"), pDeviceBox->currentText());
    settings.setValue(QString("Sensitivity"),  pSensitivityBox->currentIndex());
    settings.setValue(QString("String"),       pStringBox->currentIndex());
    settings.setValue(QString("Reveal"),       pRevealButton->isChecked());
}


// When resizeEvent() is called, the widget already has its new geometry.
void
MainWindow::resizeEvent(QResizeEvent *event) {
    buildFontSizes();
    event->setAccepted(true);
}


void
MainWindow::buildFontSizes() {
    QFont font = pScoreEdit->font();
    int iFontSize = qMin(width()/9, height());
    font.setPixelSize(iFontSize);
    pScoreEdit->setFont(font);

    iFontSize = qMin(width()/30, height());
    font.setPixelSize(iFontSize);

    pScoreLabel->setFont(font);
    pStringLabel->setFont(font);
    pStringBox->setFont(font);
    pElapsedTimeLabel->setFont(font);
    pElapsedTimeEdit->setFont(font);
    pSensitivityLabel->setFont(font);
    pSensitivityBox->setFont(font);
    pRevealButton->setFont(font);
    pInputLabel->setFont(font);
    pStartButton->setFont(font);
    pExitButton->setFont(font);
}


void
MainWindow::onStartStopPushed() {
    if(pStartButton->text().contains("Stop")) {
        updateTimer.stop();
        pStartButton->setText("Start");
        pAudioSource->stop();
        pBuffer->close();
        pInputLabel->setEnabled(true);
        pDeviceBox->setEnabled(true);
        pScoreEdit->setStyleSheet(sNormalStyle);
        return;
    }
    pStartButton->setText("Stop");
    pInputLabel->setDisabled(true);
    pDeviceBox->setDisabled(true);
    pBuffer->open(QIODevice::WriteOnly);
    currentNote = pRandomGenerator->bounded(startNote, endNote);
    pStaffArea->setNote(notes[currentNote], currentNote);
    score = 0;
    pScoreEdit->setText(QString("%1").arg(score));
    pAudioSource->start(pBuffer);
    startTime = QTime::currentTime();
    elapsedTime = QTime(0, 0, 0, 0);
    updateTimer.start(updateTime);
}


void
MainWindow::OnBufferFull() {
    //////////////////////////////////////////////////////////////
    /// Calcoliamo la funzione di autocorrelazione del segnale ///
    /// solo nei punti corrispondenti ai periodi delle note.   ///
    //////////////////////////////////////////////////////////////
    for(int t=0; t<nData-acorLags[1]; t++) {
        double ft = double(dataPointer[t])/double(SHRT_MAX);
        for(int tau=0; tau<Lags; tau++) {
            double ftau = double(dataPointer[t+acorLags[tau]])/double(SHRT_MAX);
            R[tau] += ft*ftau;
        }
    }
    // If the Signal energy is not enough...
    if(R[0] < threshold) {
        nDetections = 0;
//        pStaffArea->setNote(notes[0], -1);
        return;
    }
    // The Signal Energy is greater than the treshold
    nDetections++;
    if(nDetections > 1) { // To avoid nDetections false detections
//        qDebug() << "nDetections" << nDetections << "R[0]" << R[0] << "threshold" << threshold;
        nDetections = 0;
        // Find the Autocorrelation Max and prepare for the next Audio Buffer
        R[0] = 0.0;
        double rMax = R[1];
        int iMax = 1;
        for(int i=1; i<Lags; i++) {
            if(R[i] > rMax) {
                rMax = R[i];
                iMax = i-1;
            }
            R[i] = 0.0;
        }
        if(iMax == currentNote) {
            disconnect(pBuffer, SIGNAL(bufferFull()),
                       this, SLOT(OnBufferFull()));
            waitTimer.start(timeToWait);
            elapsedTime = elapsedTime.addSecs(updateTimer.remainingTime()/double(updateTime));
            startTime = QTime::currentTime();
            updateTimer.stop();
            score++;
            pScoreEdit->setText(QString("%1").arg(score));
            pScoreEdit->setStyleSheet(sSuccessStyle);
            pStaffArea->setNote(notes[0], -1);
        }
        else {
            pScoreEdit->setStyleSheet(sErrorStyle);
        }
    } // if(nDetections > 1)
}


void
MainWindow::onInputDeviceChanged(int index) {
    Q_UNUSED(index)
    if(pAudioInput) delete pAudioInput;
    pAudioInput = new QAudioInput(deviceInfo.at(pDeviceBox->currentIndex()), this);
    pAudioSource = new  QAudioSource(deviceInfo.at(pDeviceBox->currentIndex()), formatAudio);
    pAudioSource->setBufferSize(sampleRate*sampleSeconds);
}


void
MainWindow::onSensitivityChanged(int index) {
    threshold = double(index+1)*1.0;
//    qDebug() << "Treshold:" << threshold;
}


// nFrets Frets Guitars
void
MainWindow::onStringChanged(int index) {
    currentString = index;
    switch (currentString) {
        case 0: // E String
            startNote = 29;                // E#2
            endNote   = startNote+nFrets;  // E3
            pStaffArea->setOctaveBase(startNote/12 - 2);
            break;
        case 1: // A String
            startNote = 34;                // A#2
            endNote   = startNote+nFrets;  // A3
            pStaffArea->setOctaveBase(startNote/12 - 2);
            break;
        case 2: // D String
            startNote = 39;                // D#3
            endNote   = startNote+nFrets;  // D4
            pStaffArea->setOctaveBase(startNote/12 - 2);
            break;
        case 3: // G String
            startNote = 44;                // G#3
            endNote   = startNote+nFrets;  // G4
            pStaffArea->setOctaveBase(startNote/12 - 2);
            break;
        case 4: // B String
            startNote = 48;                // C4
            endNote   = startNote+nFrets;  // B4
            pStaffArea->setOctaveBase(startNote/12 - 1); // Da controllare !!! (-2 ?)
            break;
        case 5: // e String
            startNote = 53;                // E#4
            endNote   = startNote+nFrets;  // E5
            pStaffArea->setOctaveBase(startNote/12 - 2);
            break;
        default: // Never Executed !!!
            exit(EXIT_FAILURE);
    }
    if(pStartButton->text() == QString("Stop")) { // We are Running: Generate a New Note
        currentNote = pRandomGenerator->bounded(startNote, endNote);
        pStaffArea->setNote(notes[currentNote], currentNote);
    }

}


void
MainWindow::OnRevealCheckBoxStateChanged() {
    bRevealChecked = pRevealButton->isChecked();
    pStaffArea->setRevealNote(bRevealChecked);
    pStaffArea->update();
}


void
MainWindow::onUpdateTimerElapsed() {
#ifndef Q_OS_ANDROID
    int mSecs = startTime.msecsTo(QTime::currentTime());
    elapsedTime = elapsedTime.addSecs(mSecs/double(updateTime));
    startTime = QTime::currentTime();
#else
    elapsedTime = elapsedTime.addSecs(1);
#endif
    pElapsedTimeEdit->setText(elapsedTime.toString());
}


void
MainWindow::onWaitTimerElapsed() {
    waitTimer.stop();
    currentNote = pRandomGenerator->bounded(startNote, endNote);
    pStaffArea->setNote(notes[currentNote], currentNote);
    connect(pBuffer, SIGNAL(bufferFull()),
            this, SLOT(OnBufferFull()));
    updateTimer.start(updateTime);
}


void
MainWindow::onExitPushed() {
    close();
}
