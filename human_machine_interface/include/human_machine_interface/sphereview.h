/*
 * OpenSceneGraph + Qt + Offscreen rendering example.
 * Copyright (C) 2011  Gonzalo Exequiel Pedone
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with This program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Email   : hipersayan DOT x AT gmail DOT com
 * Web-Site: http://hipersayanx.blogspot.com/
 */

#ifndef SPHEREVIEW_H
#define SPHEREVIEW_H

#include "qt4/QtOpenGL/QGLWidget"

#include "osgDB/ReadFile"
#include "osgText/Text"
#include "osgGA/TrackballManipulator"
#include "osg/MatrixTransform"
#include "osgViewer/Viewer"
#include "telemetryStateReceiver.h"

class SphereView: public QGLWidget
{
    Q_OBJECT

    public:
        explicit SphereView(QWidget *parent, telemetryStateReceiver *telemetryReceiver=0);
    void onResize( int width, int height );

    private:
        void initializeGL();
        void resizeGL(int width, int height);
        void paintGL();
        void shapeDrawer(osg::MatrixTransform* mainXtrans, osg::MatrixTransform* faxisXtrans);
        void axisGenerator(osg::Geometry* axis, bool fixed);
        void BuildWall(osg::MatrixTransform* root);
        void createMap(osg::MatrixTransform* root);
        void mainLines(osg::Geometry* line);

    public Q_SLOTS:
    private:
        float cow_rot;
        float pitch;
        float yaw;
        float roll;
        telemetryStateReceiver* telemReceiver;

        osg::ref_ptr<osgViewer::Viewer> viewer;
        osg::observer_ptr<osgViewer::GraphicsWindowEmbedded> window;
        osg::ref_ptr<osg::MatrixTransform> mainXtrans;
         osg::ref_ptr<osg::MatrixTransform> labelXtrans;
        osg::ref_ptr<osg::MatrixTransform> root;
};

#endif // SPHEREVIEW_H
