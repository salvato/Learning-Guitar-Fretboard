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

#include "datastream2d.h"
#include <float.h>

DataStream2D::DataStream2D(int Id, int PenWidth, QColor Color, int Symbol, QString Title)
{
    Properties.SetId(Id);
    Properties.Color    = Color;
    Properties.PenWidth = PenWidth;
    Properties.Symbol   = Symbol;
    if(Title != QString())
        Properties.Title = Title;
    else
        Properties.Title = QString("Data Set %1").arg(Properties.GetId());
    isShown         = false;
    bShowCurveTitle = false;
    maxPoints = 100;
}


DataStream2D::DataStream2D(DataSetProperties myProperties) {
    Properties = myProperties;
    if(myProperties.Title == QString())
        Properties.Title = QString("Data Set %1").arg(Properties.GetId());
    isShown         = false;
    bShowCurveTitle = false;
    maxPoints = 100;
}


DataStream2D::~DataStream2D() {
}


void
DataStream2D::SetShow(bool show) {
   isShown = show;
}


void
DataStream2D::AddPoint(double x, double y) {
    m_pointArrayX.append(x);
    m_pointArrayY.append(y);
    if(m_pointArrayX.count() == 1) {
        minx = x-DBL_MIN;
        maxx = x+DBL_MIN;
        miny = y-DBL_MIN;
        maxy = y+DBL_MIN;
    }
    if(m_pointArrayX.count() > maxPoints) {
        m_pointArrayX.remove(0, maxPoints/4);
        m_pointArrayY.remove(0, maxPoints/4);
        minx = x-DBL_MIN;
        maxx = x+DBL_MIN;
        miny = y-DBL_MIN;
        maxy = y+DBL_MIN;
        for(int i=0; i<m_pointArrayX.count(); i++) {
            if(m_pointArrayX.at(i) < minx) minx = m_pointArrayX.at(i);
            if(m_pointArrayX.at(i) > maxx) maxx = m_pointArrayX.at(i);
            if(m_pointArrayY.at(i) < miny) miny = m_pointArrayY.at(i);
            if(m_pointArrayY.at(i) > maxy) maxy = m_pointArrayY.at(i);
        }
    }
    else {
        minx = minx < x ? minx : x-DBL_MIN;
        maxx = maxx > x ? maxx : x+DBL_MIN;
        miny = miny < y ? miny : y-DBL_MIN;
        maxy = maxy > y ? maxy : y+DBL_MIN;
    }
}


void
DataStream2D::SetColor(QColor Color) {
   Properties.Color = Color;
}


int
DataStream2D::GetId() {
   return Properties.GetId();
}


void
DataStream2D::RemoveAllPoints() {
    m_pointArrayX.clear();
    m_pointArrayY.clear();
}


void
DataStream2D::SetTitle(QString myTitle) {
   Properties.Title = myTitle;
}


void
DataStream2D::SetShowTitle(bool show) {
    bShowCurveTitle = show;
}


DataSetProperties
DataStream2D::GetProperties() {
    return Properties;
}


void
DataStream2D::SetProperties(DataSetProperties newProperties) {
    Properties = newProperties;
}



QString
DataStream2D::GetTitle() {
    return Properties.Title;
}


void
DataStream2D::setMaxPoints(int nPoints) {
    maxPoints = nPoints;
}


int
DataStream2D::getMaxPoints() {
    return maxPoints;
}

