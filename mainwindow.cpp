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
#include <QAudioDeviceInfo>


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
//    , pRevealCheckBox(new QCheckBox("Show Note"))
    , pRevealCheckBox(new QPushButton("Show Note"))
    , bRevealChecked(false)
    , pScoreLabel(new QLabel("Score"))
    , pScoreEdit(new QLabel("999"))
    , score(999)
    , pElapsedTimeLabel(new QLabel("Time"))
    , pElapsedTimeEdit(new QLabel("00:00:00"))
    , pInputLabel(new QLabel("Input Device"))
    , pData(nullptr)
    , chunkSize(sampleRate*sampleSeconds)
    , pRandomGenerator(QRandomGenerator::system())
    , threshold(5.0)
    , nDetections(0)
    , nFrets(12) // Only first 12 Frets (22 on Guitars Like Fender Stratocaster)
    , bFontBuilt(false)

{
    pRandomGenerator->securelySeeded();
//    pRevealCheckBox->setLayoutDirection(Qt::RightToLeft);
    pRevealCheckBox->setCheckable(true);
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
    deviceInfo = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    for(int i=0; i<deviceInfo.count(); i++) {
        pDeviceBox->addItem(deviceInfo.at(i).deviceName(), QVariant::fromValue(deviceInfo.at(i)));
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

    OnRevealCheckBoxStateChanged(int(bRevealChecked));

    pScoreLabel->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    pScoreEdit->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    pScoreEdit->setText(QString("%1").arg(score));

    elapsedTime = QTime(0, 0, 0, 0);
    pElapsedTimeLabel->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    pElapsedTimeEdit->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    pElapsedTimeEdit->setText(elapsedTime.toString());

    // MainWindow Layout
    QGridLayout *mainLayout = new QGridLayout;

    mainLayout->addWidget(pStaffArea,        0, 0, 3, 6);

    mainLayout->addWidget(pStringLabel,      3, 0, 1, 1, Qt::AlignHCenter|Qt::AlignBottom);
    mainLayout->addWidget(pScoreLabel,       3, 2, 1, 2, Qt::AlignHCenter);
    mainLayout->addWidget(pElapsedTimeLabel, 3, 4, 1, 2, Qt::AlignHCenter|Qt::AlignBottom);

    mainLayout->addWidget(pStringBox,        4, 0, 1, 1, Qt::AlignHCenter|Qt::AlignTop);
    mainLayout->addWidget(pScoreEdit,        4, 2, 1, 2);
    mainLayout->addWidget(pElapsedTimeEdit,  4, 4, 1, 2, Qt::AlignHCenter|Qt::AlignTop);

//    mainLayout->addWidget(pRevealCheckBox,   5, 0, 1, 2, Qt::AlignRight);
    mainLayout->addWidget(pRevealCheckBox,   5, 0, 1, 1);//, Qt::AlignHCenter);
    mainLayout->addWidget(pSensitivityLabel, 5, 2, 1, 2, Qt::AlignRight);
    mainLayout->addWidget(pSensitivityBox,   5, 4, 1, 2, Qt::AlignLeft);

    mainLayout->addWidget(pInputLabel,       6, 0, 1, 1, Qt::AlignRight);
    mainLayout->addWidget(pDeviceBox,        6, 1, 1, 5);

    mainLayout->addWidget(pExitButton,       7, 0, 1, 1);
    mainLayout->addWidget(pStartButton,      7, 4, 1, 2);


    setLayout(mainLayout);

    // UI Message handlers
    connect(pStartButton, SIGNAL(clicked()),
            this, SLOT(onStartStopPushed()));
    connect(pDeviceBox, SIGNAL(activated(int)),
            this, SLOT(onInputDeviceChanged(int)));
    connect(pSensitivityBox, SIGNAL(activated(int)),
            this, SLOT(onSensitivityChanged(int)));
    connect(pStringBox, SIGNAL(activated(int)),
            this, SLOT(onStringChanged(int)));
    connect(pRevealCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(OnRevealCheckBoxStateChanged(int)));

    // Define requested Audio Format
    formatAudio.setSampleRate(sampleRate);
    formatAudio.setChannelCount(1);
    formatAudio.setSampleType(QAudioFormat::SignedInt);
    formatAudio.setByteOrder(QAudioFormat::LittleEndian);
    formatAudio.setSampleSize(16);
    formatAudio.setCodec("audio/pcm");

    // Create the AudioInput object
    pAudioInput = new QAudioInput(deviceInfo.at(pDeviceBox->currentIndex()), formatAudio, this);
    pAudioInput->setBufferSize(sampleRate*sampleSeconds);

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

    connect(&updateTimer, SIGNAL(timeout()),
            this, SLOT(onUpdateTimerElapsed()));
}


void
MainWindow::closeEvent(QCloseEvent *event) {
    updateTimer.stop();
    if(pAudioInput) {
        pAudioInput->stop();
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
    settings.setValue(QString("Input_Device"),      pDeviceBox->currentText());
    settings.setValue(QString("Sensitivity"),       pSensitivityBox->currentIndex());
    settings.setValue(QString("String"),            pStringBox->currentIndex());
    settings.setValue(QString("Reveal"),            pRevealCheckBox->isChecked());
}


void
MainWindow::resizeEvent(QResizeEvent *event) {
    if(!bFontBuilt) {
        bFontBuilt = true;
        buildFontSizes();
        event->setAccepted(true);
    }
    else
        event->setAccepted(false);
}


void
MainWindow::buildFontSizes() {

    QFont font;
    int iFontSize;
    int hMargin, vMargin;
    QMargins margins;

    font = pScoreEdit->font();
    margins = pScoreEdit->contentsMargins();
    vMargin = margins.bottom() + margins.top();
    hMargin = margins.left() + margins.right();
    font.setCapitalization(QFont::Capitalize);
    iFontSize = qMin((pScoreEdit->width()/3)-hMargin,
                     pScoreEdit->height()-vMargin);
    font.setPixelSize(iFontSize*2);
    pScoreEdit->setFont(font);
    font.setPixelSize(iFontSize);
    pScoreLabel->setFont(font);

    font.setCapitalization(QFont::MixedCase);
    pStringLabel->setFont(font);
    pStringBox->setFont(font);
    pElapsedTimeLabel->setFont(font);
    pElapsedTimeEdit->setFont(font);

    pSensitivityLabel->setFont(font);
    pSensitivityBox->setFont(font);
    pRevealCheckBox->setFont(font);

    pInputLabel->setFont(font);
    pStartButton->setFont(font);
    pExitButton->setFont(font);
}



void
MainWindow::onStartStopPushed() {
    if(pStartButton->text().contains("Stop")) {
        updateTimer.stop();
        pStartButton->setText("Start");
        pAudioInput->stop();
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
    pAudioInput->start(pBuffer);
    startTime = QTime::currentTime();
    elapsedTime = QTime(0, 0, 0, 0);
    updateTimer.start(1000);
}


void
MainWindow::OnBufferFull() {
    //////////////////////////////////////////////////////////////
    /// Calcoliamo la funzione di autocorrelazione del segnale ///
    /// solo nei punti corrispondenti ai periodi delle note.   ///
    //////////////////////////////////////////////////////////////
    for(int t=0; t<nData-acorLags[1]; t++) {
        double ft = double(dataPointer[t])/32768.0;
        for(int tau=0; tau<Lags; tau++) {
            double ftau = double(dataPointer[t+acorLags[tau]])/32768.0;
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
        qDebug() << "nDetections" << nDetections << "R[0]" << R[0] << "threshold" << threshold;
        nDetections = 0;
        // Find the Autocorrelatione Max and prepare for the next Audio Buffer
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
            score++;
            pScoreEdit->setText(QString("%1").arg(score));
            currentNote = pRandomGenerator->bounded(startNote, endNote);
            pStaffArea->setNote(notes[currentNote], currentNote);
            pScoreEdit->setStyleSheet(sSuccessStyle);
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
    pAudioInput = new QAudioInput(deviceInfo.at(pDeviceBox->currentIndex()), formatAudio, this);
    pAudioInput->setBufferSize(sampleRate*sampleSeconds);

//    qDebug() << "onInputDeviceChanged(" << index << ")";
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
MainWindow::OnRevealCheckBoxStateChanged(int isChecked) {
    bRevealChecked = bool(isChecked);
    pRevealCheckBox->setChecked(bRevealChecked);
    pStaffArea->setRevealNote(bRevealChecked);
    pStaffArea->update();
}


void
MainWindow::onUpdateTimerElapsed() {
    int mSecs = startTime.msecsTo(QTime::currentTime());
    elapsedTime = elapsedTime.addSecs(mSecs/1000);
    startTime = QTime::currentTime();
    pElapsedTimeEdit->setText(elapsedTime.toString());
}

