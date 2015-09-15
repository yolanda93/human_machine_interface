/*
  2D Parameter temporal series window
  Plotting of odometer parameters selected in real time.
  @author  Yolanda de la Hoz Simón
  @date    03-2015
  @version 1.0
*/

#include "../include/parameter_temporal_series.h"
#include "../.././../../../hmi_cvg_stack -build/human_machine_interface/ui_parametertemporalseries.h"


#include <qwt/qwt.h>
#include <qwt/qwt_plot.h>
#include <qwt/qwt_plot_curve.h>
#include <cmath>

#include <qt4/Qt/qlabel.h>
#include <qt4/Qt/qlayout.h>
#include <qt4/Qt/qdockwidget.h>
#include <qt4/Qt/qwidget.h>
#include <qt4/Qt/qcheckbox.h>
#include <qt4/Qt/qvariant.h>
#include <list>
#include <qt4/Qt/qmap.h>
#include <qt4/Qt/qiterator.h> //QMapIterator>
#include <qt4/Qt/qregexp.h>
#include <qt4/Qt/qtreewidget.h>  //QTreeWidget>


ParameterTemporalSeries::ParameterTemporalSeries(QWidget *parent, TelemetryStateReceiver* telemetryReceiver,OdometryStateReceiver* odometryReceiver):
    QWidget(parent),
    ui(new Ui::parameterTemporalSeries)
{
    ui->setupUi(this);
    telemReceiver=telemetryReceiver;
    odomReceiver=odometryReceiver;

    // ESTRUCTURA DE VISUALIZACIÓN
    telemetry << "yaw" << "pitch" << "roll"  << "IMU.angular.vel.x"  << "IMU.angular.vel.y"  << "IMU.angular.vel.z"  << "IMU.accel.x"  << "IMU.accel.y"   << "IMU.accel.z" << "IMU.roll" << "IMU.pitch" << "IMU.yaw" << "IMU.yawPitch" << "altitude" << "altitudeSpeed" <<  "mag.X"<< "mag.Y"<< "mag.Z" << "groundSpeed.X"<< "groundSpeed.Y"<<"temperature"<<"pressure";
    controller << "xci" << "yci" << "zci" << "yawci" << "pitchci" << "rollci" << "vxfi" << "vyfi"<< "vzfi" << "dyawfi";
    ekf << "pos.x" << "pos.y" << "pos.z" << "yaw" << "pitch" << "roll" << "dx" << "dy" << "dz" << "dyaw" << "dpitch" << "droll";

    parameters.insert("Telemetry",telemetry);//pos1
    parameters.insert("Ext.Kallman Filter",ekf);//pos3
    parameters.insert("Controller",controller);//pos2

    plot = new DataPlot(ui->plotWidget,telemReceiver,odomReceiver,&parameters);

    ui->plotWidget->resize(1200,430);

    printf ("height: %d \n",ui->plotWidget->size().height());
    printf ("width: %d \n",ui->plotWidget->size().width());
    plot->setCanvasBackground(QBrush(Qt::white));

    this->initTree(parameters,ui->treeWidget);

    setSignalHandlers();

    updateParametersValue();

}


void ParameterTemporalSeries::setSignalHandlers()
{
    connect(ui->treeWidget,SIGNAL(itemChanged(QTreeWidgetItem*,int)),plot,SLOT(clickToPlot(QTreeWidgetItem*, int)));
    connect(ui->spinBox_minAxis, SIGNAL(valueChanged(int)),plot, SLOT(resizeAxisYMinLimit(int)) );
    connect(ui->spinBox_maxAxis, SIGNAL(valueChanged(int)),plot, SLOT(resizeAxisYMaxLimit(int)) );
    connect(ui->pushButton, SIGNAL(clicked()),plot, SLOT(saveAsSVG()) );
    connect(telemReceiver, SIGNAL( parameterReceived( )), this, SLOT(updateParametersValue( )));
    //  connect(ui->lineEdit,SIGNAL(textChanged(QString)),
    //         this, SLOT(onTextFilterChange(QString)));
    //connect(ui->checkBox_4,SIGNAL(clicked(bool)),this, SLOT(onShowUnits(bool)));
    connect(ui->pushButton_2,SIGNAL(clicked()),this, SLOT(onStopButton()));
}


void ParameterTemporalSeries::resizeEvent(QResizeEvent * event)
{
    plot->resize(ui->plotWidget->size());
}


void ParameterTemporalSeries::initParameterList(QStringList list, QTreeWidget *tree){

    ui->treeWidget->setIndentation(15);
    for(int i =0;i<list.size();++i){
        QTreeWidgetItem  *itm = new QTreeWidgetItem(ui->treeWidget);
        QPixmap pixmap(15,40);
        pixmap.fill(QColor("white"));
        QIcon redIcon(pixmap);
        itm->setIcon(0, redIcon);
        itm->setText(0,list.at(i));
        itm->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled );
        itm->setCheckState(0,Qt::Unchecked);
        ui->treeWidget->addTopLevelItem(itm);
    }
}


void ParameterTemporalSeries::initTree(QMap<QString,QStringList> topicList, QTreeWidget *tree){
    QMapIterator<QString,QStringList> i(topicList);
    while (i.hasNext()) {
        i.next();
        qDebug() << i.key();
        this->addRootTree(i.key(),i.value(), tree);
    }
}


void ParameterTemporalSeries::addRootTree(QString name, QStringList list, QTreeWidget *tree){
    QTreeWidgetItem  *itm = new QTreeWidgetItem(tree);
    itm->setText(0,name);
    tree->addTopLevelItem(itm);
    tree->setIndentation(15);
    addChildTree(itm,list,"");

}


void ParameterTemporalSeries::addChildTree(QTreeWidgetItem *parent,  QStringList list, QString description)
{
    for(int i =0;i<list.size();++i){
        QTreeWidgetItem *itm = new QTreeWidgetItem();
        QPixmap pixmap(15,40);
        pixmap.fill(QColor("white"));
        QIcon redIcon(pixmap);
        itm->setIcon(0, redIcon);
        itm->setText(0,list.at(i));
        itm->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled );
        itm->setCheckState(0,Qt::Unchecked);

        parent->addChild(itm);
    }
}


void ParameterTemporalSeries::onTextFilterChange(const QString &arg1){
    QRegExp regExp(arg1, Qt::CaseInsensitive, QRegExp::Wildcard);
    ui->treeWidget->blockSignals(true);
    ui->treeWidget->clear();

    QMapIterator<QString,QStringList> i(parameters);
    while (i.hasNext()) {
        i.next();
        qDebug() << ui->treeWidget->isFirstItemColumnSpanned(ui->treeWidget->itemAt(0,0));
        this->addRootTree(i.key(),i.value().filter(regExp), ui->treeWidget);
        qDebug() << "filter data" << i.value().filter(regExp);
    }
    //ui->treeWidget->blockSignals(false);
    qDebug() << "connect";
}


void ParameterTemporalSeries::onShowUnits(bool click){
    QStringList headers;
    if(click){
        headers << "Parameters" << "Value" << "Unit";
        QMapIterator<QString,QStringList> i(parameters);
        QStringList list;
        while (i.hasNext()) {
            i.next();
            list=i.value();
            QString itemName;
            for(int j =0;j<list.size();++j){
                itemName=i.key() + "/" + list.at(j);
                if(plot->selectedItems.contains(itemName)){
                    QTreeWidgetItem* itemList = plot->items[itemName];
                    itemList->setText(2,"float");
                }
            }
        }
        ui->treeWidget->setColumnCount(3);
    }else{
        headers << "Parameters" << "Value";
        ui->treeWidget->setColumnCount(2);
    }
    ui->treeWidget->setHeaderLabels(headers);
}


void ParameterTemporalSeries::onStopButton(){
    if(plot->stopPressed)
        plot->stopPressed = false;
    else
        plot->stopPressed = true;
}


void ParameterTemporalSeries::updateParametersValue(){
    QTreeWidgetItem * EKF = ui->treeWidget->topLevelItem(1);
    QTreeWidgetItem * telemetryItem =ui->treeWidget->topLevelItem(2);
    QTreeWidgetItem * controllerItem=ui->treeWidget->topLevelItem(0);


    controllerItem->child(0)->setText(1,QString::number(((double)((int)(odomReceiver->DroneControllerPoseMsgs.x*100)))/100)+ "  m"); //xci
    controllerItem->child(1)->setText(1,QString::number(((double)((int)(odomReceiver->DroneControllerPoseMsgs.y*100)))/100)+ "  m"); //yci
    controllerItem->child(2)->setText(1,QString::number(((double)((int)(odomReceiver->DroneControllerPoseMsgs.z*100)))/100)+ "  m"); //zci
    controllerItem->child(3)->setText(1,QString::number(((double)((int)(odomReceiver->DroneControllerPoseMsgs.yaw*100)))/100)+ "  deg"); //yawci
    controllerItem->child(4)->setText(1,QString::number(((double)((int)(odomReceiver->DroneControllerPoseMsgs.pitch*100)))/100)+ "  deg"); //pitchci
    controllerItem->child(5)->setText(1,QString::number(((double)((int)(odomReceiver->DroneControllerPoseMsgs.roll*100)))/100)+ "  deg"); //rollci
    controllerItem->child(6)->setText(1,QString::number(((double)((int)(odomReceiver->DroneControllerSpeedsMsgs.dx*100)))/100)+ "  m/s"); //vxci
    controllerItem->child(7)->setText(1,QString::number(((double)((int)(odomReceiver->DroneControllerSpeedsMsgs.dy)))/100)+ "  m/s"); //vyci
    controllerItem->child(8)->setText(1,QString::number(((double)((int)(odomReceiver->DroneControllerSpeedsMsgs.dz)))/100)+ "  m/s"); //vzci
    controllerItem->child(9)->setText(1,QString::number(((double)((int)(odomReceiver->DroneControllerSpeedsMsgs.dyaw)))/100)+ "  m/s"); //dyawfi

    EKF->child(0)->setText(1,QString::number(((double)((int)(odomReceiver->DronePoseMsgs.x*100)))/100) + "  m"); //x
    EKF->child(1)->setText(1,QString::number(((double)((int)(odomReceiver->DronePoseMsgs.y*100)))/100) + "  m"); //y
    EKF->child(2)->setText(1,QString::number(((double)((int)(odomReceiver->DronePoseMsgs.z*100)))/100) + "  m"); //z
    EKF->child(3)->setText(1,QString::number(((double)((int)(odomReceiver->DronePoseMsgs.yaw*100)))/100) + "  deg"); //yaw
    EKF->child(4)->setText(1,QString::number(((double)((int)(odomReceiver->DronePoseMsgs.pitch*100)))/100) + "  deg"); //pitch
    EKF->child(5)->setText(1,QString::number(((double)((int)(odomReceiver->DronePoseMsgs.roll*100)))/100) + "  deg"); //roll
    EKF->child(6)->setText(1,QString::number(((double)((int)(odomReceiver->DroneSpeedsMsgs.dx*100)))/100) + "  m/s"); //dx
    EKF->child(7)->setText(1,QString::number(((double)((int)(odomReceiver->DroneSpeedsMsgs.dy*100)))/100) + "  m/s"); //dy
    EKF->child(8)->setText(1,QString::number(((double)((int)(odomReceiver->DroneSpeedsMsgs.dz*100)))/100) + "  m/s"); //dz
    EKF->child(9)->setText(1,QString::number(((double)((int)(odomReceiver->DroneSpeedsMsgs.dyaw*100)))/100) + "  m/s"); //dyaw
    EKF->child(10)->setText(1,QString::number(((double)((int)(odomReceiver->DroneSpeedsMsgs.dpitch*100)))/100) + "  m/s"); //dpitch
    EKF->child(11)->setText(1,QString::number(((double)((int)(odomReceiver->DroneSpeedsMsgs.droll*100)))/100) + "  m/s");  //droll


    telemetryItem->child(0)->setText(1,QString::number(((double)((int)(telemReceiver->rotationAnglesMsgs.vector.z*100)))/100) + "  deg");//yaw
    telemetryItem->child(1)->setText(1,QString::number(((double)((int)(telemReceiver->rotationAnglesMsgs.vector.y*100)))/100) + "  deg");//pitch
    telemetryItem->child(2)->setText(1,QString::number(((double)((int)(telemReceiver->rotationAnglesMsgs.vector.x*100)))/100) + "  deg");//roll
    telemetryItem->child(3)->setText(1,QString::number(((double)((int)(telemReceiver->imuMsgs.angular_velocity.x*100)))/100)  + "  m/s");//IMU angular vel.x
    telemetryItem->child(4)->setText(1,QString::number(((double)((int)(telemReceiver->imuMsgs.angular_velocity.y*100)))/100)  + "  m/s");//IMU angular vel.y
    telemetryItem->child(5)->setText(1,QString::number(((double)((int)(telemReceiver->imuMsgs.angular_velocity.z*100)))/100)  + "  m/s");//IMU angular vel.z
    telemetryItem->child(6)->setText(1,QString::number(((double)((int)(telemReceiver->imuMsgs.linear_acceleration.x*100)))/100) + "  deg");//IMU accel.x
    telemetryItem->child(7)->setText(1,QString::number(((double)((int)(telemReceiver->imuMsgs.linear_acceleration.y*100)))/100) + "  deg");//IMU accel.y
    telemetryItem->child(8)->setText(1,QString::number(((double)((int)(telemReceiver->imuMsgs.linear_acceleration.z*100)))/100) + "  deg");//IMU accel.z
    telemetryItem->child(9)->setText(1,QString::number(((double)((int)(telemReceiver->imuMsgs.orientation.x*100)))/100) + "  deg");//IMU roll
    telemetryItem->child(10)->setText(1,QString::number(((double)((int)(telemReceiver->imuMsgs.orientation.y*100)))/100) + "  deg");//IMU pitch
    telemetryItem->child(11)->setText(1,QString::number(((double)((int)(telemReceiver->imuMsgs.orientation.z*100)))/100) + "  deg");//IMU yaw
    telemetryItem->child(12)->setText(1,QString::number(((double)((int)(telemReceiver->imuMsgs.orientation.w*100)))/100) + "  deg");//IMU yawPitch
    telemetryItem->child(13)->setText(1,QString::number(((double)((int)(telemReceiver->altitudeMsgs.altitude*100)))/100) + "  m");//altitude
    telemetryItem->child(14)->setText(1,QString::number(((double)((int)(telemReceiver->altitudeMsgs.altitude_speed*100)))/100) + "  m/s");//altitudeSpeed
    telemetryItem->child(15)->setText(1,QString::number(((double)((int)(telemReceiver->magnetometerMsgs.vector.x*100)))/100));//mag.X
    telemetryItem->child(16)->setText(1,QString::number(((double)((int)(telemReceiver->magnetometerMsgs.vector.y*100)))/100));//mag.Y
    telemetryItem->child(17)->setText(1,QString::number(((double)((int)(telemReceiver->magnetometerMsgs.vector.z*100)))/100));//mag.Z
    telemetryItem->child(18)->setText(1,QString::number(((double)((int)(telemReceiver->groundSpeedMsgs.vector.x*100)))/100) + "  m/s");//groundSpeed.x
    telemetryItem->child(19)->setText(1,QString::number(((double)((int)(telemReceiver->groundSpeedMsgs.vector.y*100)))/100) + "  m/s");//groundSpeed.y
    telemetryItem->child(20)->setText(1,QString::number(((double)((int)(telemReceiver->temperature.temperature*100)))/100)+ " deg");//temperature
    telemetryItem->child(21)->setText(1,QString::number(((double)((int)(telemReceiver->fluidPressure.fluid_pressure*100)))/100) + " Pa");//fluidPressure


}


ParameterTemporalSeries::~ParameterTemporalSeries()
{
    delete ui;
}

