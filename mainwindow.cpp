#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "QFileDialog"
#include "QMessageBox"
#include "QRubberBand"
#include "transparent.h"
#include <thread>
#include "QPainter"
#include "QPoint"
#include "QApplication"
#include "QWidget"

using namespace std;


void resetWindowsRegisters(int height, int width){
    QSettings settings("HKEY_CURRENT_USER\\Software\\screen-capture-recorder",
                       QSettings::NativeFormat);
    settings.setValue("capture_height", height);
    settings.setValue("capture_width", width);
    settings.setValue("start_x", 0);
    settings.setValue("start_y", 0);
}

void resetArea(QLabel *label){
    QScreen *screen = QGuiApplication::primaryScreen();
    int height = screen->geometry().height();
    int width = screen->geometry().width();

    #if defined(_WIN32)
    resetWindowsRegisters(height, width);
    #endif

    QString x = "Area Selezionata: origine (0,0), dimensione: " +
            QString::number(screen->geometry().width()) +
            "x" +
            QString::number(screen->geometry().height());

    label->setText(x);
}


MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    resetArea(ui->label_4);
    ui->label_5->setText(ui->label_4->text());

    ui->pushButton_5->setDisabled(true);
    ui->pushButton_7->setDisabled(true);
    ui->stackedWidget->setCurrentIndex(0);
    ui->radioButton_3->hide();
}

MainWindow::~MainWindow()
{
    delete ui;
    if(drag != nullptr)
        delete drag;
}

void MainWindow::on_pushButton_clicked() //CONFERMA
{
    this->fileNameOutput = ui->lineEdit->text();

    if(this->dir == ""){
        QMessageBox::information(this, tr("Errore nome directory di output"), "Scegliere una directory valida");
        return;
    }
    if(this->fileNameOutput == ""){
        QMessageBox::information(this, tr("Errore nome file di output"), "Inserire un file di output valido");
        return;
    }

    this->fullPathOutput = this->dir + "/" + this->fileNameOutput + ".mp4";
    QString x = "Percorso selezionato:\n   " + this->fullPathOutput;

    ui->percorsoSelezionato->setText(x);

    ui->frame->setDisabled(true);
    ui->stackedWidget->setCurrentIndex(1);

    this->sc = new ScreenRecorder(ui->radioButton_2->isChecked());

    #if defined (__unix__)
    if(drag == nullptr){
        QScreen *screen = QGuiApplication::primaryScreen();
        int height = screen->geometry().height();
        int width = screen->geometry().width();
        sc->origin_x = 0;
        sc->origin_y = 0;
        sc->videoSize = std::to_string(width) + "x" + std::to_string(height);
    }
    else{
        sc->origin_x = drag->rubberBand->geometry().topLeft().rx();
        sc->origin_y = drag->rubberBand->geometry().topLeft().ry();
        sc->videoSize = std::to_string(drag->rubberBand->geometry().width()) + "x" + std::to_string(drag->rubberBand->geometry().height());
    }
    #endif

    if(sc->init_outputfile_AV(this->fullPathOutput.toStdString()) != 0){QMessageBox::information(this, tr("Errore"),
                                                                                                 "Errore inizializzazione file di output"); exit(0);}

    if(sc->openScreen() != 0){QMessageBox::information(this, tr("Errore"), "Errore apertura periferica video"); exit(0);}
    sc->setVideoDecoder();
    sc->setVideoEncoder();
    sc->closeVideo();

    if(ui->radioButton_2->isChecked()){
        if(sc->openMic() != 0){QMessageBox::information(this, tr("Errore"), "Errore apertura periferica audio"); exit(0);}
        sc->setAudioDecoder();
        sc->setAudioEncoder();
        sc->closeAudio();
    }
    if(sc->create_outputfile() != 0){QMessageBox::information(this, tr("Errore"), "Errore creazione file di output"); exit(0);}
}

void MainWindow::on_pushButton_2_clicked() //OPEN DIR BUTTON
{
    this->dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                    "C://",
                                                    QFileDialog::ShowDirsOnly
                                                   | QFileDialog::DontResolveSymlinks);
    ui->lineEdit_2->setText(this->dir);
}

void MainWindow::on_pushButton_3_clicked() //SELECT AREA BUTTON
{
    drag = new Transparent(ui->label_4, ui->label_5);
    drag->setWindowOpacity(0.2);
    drag->showFullScreen();
    drag->show();
    ui->label_5->setText(ui->label_4->text());
}

void MainWindow::on_pushButton_4_clicked() //RESET FULL SCREEN BUTTON
{
    resetArea(ui->label_4);
    ui->label_5->setText(ui->label_4->text());
    #ifdef __unix__
    if(drag != nullptr){
        delete drag;
        drag = nullptr;
    }
    #endif
}

void MainWindow::on_pushButton_6_clicked()//REGISTRA BUTTON
{

    ui->pushButton_5->setDisabled(false);
    ui->pushButton_6->setDisabled(true);
    ui->pushButton_7->setDisabled(false);

    if(sc->openScreen() != 0){QMessageBox::information(this, tr("Errore"), "Errore apertura periferica video");exit(0);}
    if(ui->radioButton_2->isChecked())
       if(sc->openMic() != 0){QMessageBox::information(this, tr("Errore"), "Errore apertura periferica audio");exit(0);}

    this->thread_vector.push_back(std::thread ([&](){
        sc->recordVideo();
        }));

    if(ui->radioButton_2->isChecked()){
        this->thread_vector.push_back(std::thread ([&](){
            sc->recordAudio();
        }));
    }
    ui->radioButton_3->show();
}

void MainWindow::on_pushButton_7_clicked() //STOP BUTTON
{
        sc->stop_m.lock();
        if(sc->stop == 0){
            sc->stop=1;
            sc->stop_m.unlock();

            this->thread_vector[0].join();

            if(ui->radioButton_2->isChecked())
                this->thread_vector[1].join();

            this->thread_vector.clear();

            if(ui->radioButton_2->isChecked())
                sc->closeAudio();
            sc->closeVideo();
        }
        else{
            sc->stop_m.unlock();
        }

        if(sc->close_outputfile() != 0){QMessageBox::information(this, tr("Errore"), "Errore finalizzazione file di output");exit(0);}

        ui->pushButton_5->setDisabled(true);
        ui->pushButton_6->setDisabled(false);
        ui->pushButton_7->setDisabled(true);
        ui->stackedWidget->setCurrentIndex(0);
        ui->frame->setDisabled(false);
        ui->radioButton_3->hide();

        //METODO PER RESETTARE TUTTO
        delete this->sc;
}

void MainWindow::on_pushButton_5_clicked() //PAUSA/RIPRENDI BUTTON
{
    sc->stop_m.lock();
    if(sc->stop == 0 ){
        sc->stop=1;
        sc->stop_m.unlock();

        this->thread_vector[0].join();

        if(ui->radioButton_2->isChecked())//RADIO BUTTON
            this->thread_vector[1].join();

        this->thread_vector.clear();

        if(ui->radioButton_2->isChecked())
            sc->closeAudio();

        sc->closeVideo();
        ui->pushButton_5->setText("Riprendi");
        ui->radioButton_3->hide();

    }
    else{
        sc->stop=0;
        sc->stop_m.unlock();

        if(ui->radioButton_2->isChecked())
            if(sc->openMic() != 0){QMessageBox::information(this, tr("Errore"), "Errore apertura periferica audio");exit(0);}

        if(sc->openScreen() != 0){QMessageBox::information(this, tr("Errore"), "Errore apertura periferica video");exit(0);}

        this->thread_vector.push_back(std::thread ([&](){
            sc->recordVideo();
            }));

        if(ui->radioButton_2->isChecked())
            this->thread_vector.push_back(std::thread ([&](){
                sc->recordAudio();
                }));

        ui->pushButton_5->setText("Pausa");
        ui->radioButton_3->show();

    }
}

