/**
 * @file /include/GroundControlSystem/collector.hpp
 *
 * @brief ROS node thread to subscribe topics.
 *
 * @date March 2015
 **/
/*****************************************************************************
** Ifdefs
*****************************************************************************/

#ifndef HumanMachineInterface_ODOMETRYSTATERECEIVER_H_
#define HumanMachineInterface_ODOMETRYSTATERECEIVER_H_

/*****************************************************************************
** Includes
*****************************************************************************/

#include <ros/ros.h>
#include <string>
#include <QString>
#include "std_msgs/String.h"
#include "std_msgs/Float32MultiArray.h"
#include "geometry_msgs/Point.h"
#include "droneMsgsROS/droneSpeeds.h"
#include "droneMsgsROS/AliveSignal.h"
#include "droneMsgsROS/ProcessError.h"
#include "droneMsgsROS/ErrorType.h"
#include "droneMsgsROS/dronePose.h"
#include "droneMsgsROS/obsVector.h"
#include "droneMsgsROS/ProcessDescriptorList.h"
#include "droneMsgsROS/dronePositionTrajectoryRefCommand.h"
#include "droneMsgsROS/droneTrajectoryControllerControlMode.h"
#include "communication_definition.h"
#include <QThread>
#include <QString>
#include <QStringListModel>


/*****************************************************************************
** Class
*****************************************************************************/

class OdometryStateReceiver: public QThread {
    Q_OBJECT
public:
         OdometryStateReceiver();
	virtual ~ OdometryStateReceiver();

        void run();

        droneMsgsROS::droneSpeeds DroneSpeedsMsgs;
        droneMsgsROS::dronePose DronePoseMsgs;
        droneMsgsROS::droneSpeeds DroneControllerSpeedsMsgs;
        droneMsgsROS::dronePose DroneControllerPoseMsgs;

        std::vector<droneMsgsROS::ProcessDescriptor> listProcessState;


	/*********************
	** Logging
	**********************/
	enum LogLevel {
	         Debug,
	         Info,
	         Warn,
	         Error,
	         Fatal
	 };

	QStringListModel* loggingModel() { return &logging_model; }
	void log( const LogLevel &level, const std::string &msg);
        void openSubscriptions(ros::NodeHandle nodeHandle);
        void readParams(ros::NodeHandle nodeHandle);

Q_SIGNALS:
	void loggingUpdated();
        void rosShutdown();
        void parameterReceived();
        void updateStatus();
        void supervisorStateReceived( );


private:

        std::string drone_trajectory_controller_pose_subscription_gmr;
        std::string drone_trajectory_controller_speeds_subscription_gmr;
        std::string drone_state_estimator_interface_pose_subscription_lmrt;
        std::string drone_state_estimator_interface_speeds_subscription_lmrt;
        std::string drone_trajectory_planner_pose_subscription;
        std::string drone_trajectory_controller_speeds_subscription_gmr_aruco;
        std::string drone_logger_aruco_eye_observationvec_list;
        std::string drone_logger_position_ref_rebroadcast_subscription;
        std::string drone_logger_speed_ref_rebroadcast_subscription;


	int init_argc;
        int real_time;
        char** init_argv;

        ros::Subscriber DroneGMREstimatedPoseSubs;
        void droneGMREstimatedPoseCallback(const droneMsgsROS::dronePose::ConstPtr &msg);

        ros::Subscriber DroneGMREstimatedSpeedSubs;
        void droneGMREstimatedSpeedCallback(const droneMsgsROS::droneSpeeds::ConstPtr &msg);

        ros::Subscriber DroneSOEstimatedPoseSubs;
        void droneSOEstimatedPoseCallback(const droneMsgsROS::dronePose::ConstPtr &msg);

        ros::Subscriber DroneSOEstimatedSpeedsSubs;
         void droneSOEstimatedSpeedsCallback(const droneMsgsROS::droneSpeeds::ConstPtr &msg);

        ros::Subscriber DroneArucoEstimatedPoseSubs;
        void droneArucoEstimatedPoseCallback(const droneMsgsROS::dronePose::ConstPtr &msg);

        ros::Subscriber DroneArucoEstimatedSpeedSubs;
        void droneArucoEstimatedSpeedCallback(const droneMsgsROS::droneSpeeds::ConstPtr &msg);

        ros::Subscriber DroneArucoEyeObservationSubs;
        void droneArucoEyeObservationCallback(const droneMsgsROS::obsVector::ConstPtr &msg);


        ros::Subscriber DroneTrajectoryControlSubs;
        void droneTrajectoryControllerControlModeCallback(const droneMsgsROS::droneTrajectoryControllerControlMode::ConstPtr &msg);

        ros::Subscriber DroneTrajectoryPositionSubs;
        void dronePoseCallback(const droneMsgsROS::dronePose::ConstPtr &msg);

        ros::Subscriber DroneTrajectorySpeedsSubs;
        void droneSpeedsCallback(const droneMsgsROS::droneSpeeds::ConstPtr &msg);

        ros::Subscriber DroneTrajectoryTjReferenceSubs;
        void dronePositionTrajectoryRefCommandCallback(const droneMsgsROS::dronePositionTrajectoryRefCommand::ConstPtr &msg);


        void processPerformanceListCallback(const droneMsgsROS::ProcessDescriptorList::ConstPtr& msg);

        QStringListModel logging_model;

};


#endif /* HumanMachineInterface_ODOMETRYSTATERECEIVER_H_ */
