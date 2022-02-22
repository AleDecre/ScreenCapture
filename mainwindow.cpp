#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "QFileDialog"
#include "QMessageBox"
#include "QRubberBand"
#include "transparent.h"

using namespace std;

void resetRegisters(QLabel *label){
    QScreen *screen = QGuiApplication::primaryScreen();
    QSettings settings("HKEY_CURRENT_USER\\Software\\screen-capture-recorder",
                       QSettings::NativeFormat);
    settings.setValue("capture_height", screen->geometry().height());
    settings.setValue("capture_width", screen->geometry().width());
    settings.setValue("start_x", 0);
    settings.setValue("start_y", 0);

    QString x = "Area Selezionata: origine (0,0), dimensione: " +
            QString::number(screen->geometry().width()) +
            "x" +
            QString::number(screen->geometry().height());

    label->setText(x);
}


MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    resetRegisters(ui->label_4);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    this->fileNameOutput = ui->lineEdit->text();

    if(this->dir == ""){
        QMessageBox::information(this, tr("File di output"), "Scegliere una directory valida");
        return;
    }
    if(this->fileNameOutput == ""){
        QMessageBox::information(this, tr("File di output"), "Inserire un file di output valido");
        return;
    }

    this->fullPathOutput = this->dir + "/" + this->fileNameOutput + ".mp4";
    QString x = ui->percorsoSelezionato->text() + " " + this->fullPathOutput;

    ui->percorsoSelezionato->setText(x);

    ui->frame->setDisabled(true);

    sc.init_outputfile_AV(this->fullPathOutput.toStdString());
    sc.openScreen();
    sc.setVideoDecoder();
    sc.setVideoEncoder();


    sc.openMic();
    sc.setAudioDecoder();
    sc.setAudioEncoder();

    sc.closeAudio();
    sc.closeVideo();

    sc.create_outputfile();
    sc.close_outputfile();


}


void MainWindow::on_pushButton_2_clicked()
{
    this->dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                    "C://",
                                                    QFileDialog::ShowDirsOnly
                                                   | QFileDialog::DontResolveSymlinks);
    ui->lineEdit_2->setText(this->dir);
}

void MainWindow::on_pushButton_3_clicked()
{
    drag = new Transparent(ui->label_4);
    drag->setWindowOpacity(0.2);
    drag->showFullScreen();
    drag->show();
}

void MainWindow::on_pushButton_4_clicked()
{
    resetRegisters(ui->label_4);
}



