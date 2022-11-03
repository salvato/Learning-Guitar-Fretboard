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
SOFTWARE.*
*/
#pragma once

#include <QVector>
#include <QColor>

#include "DataSetProperties.h"

class DataStream2D
{
public:
    DataStream2D(int Id, int PenWidth, QColor Color, int Symbol, QString Title);
    DataStream2D(DataSetProperties Properties);
    virtual ~DataStream2D();
    // Operations
    void setMaxPoints(int nPoints);
    int  getMaxPoints();
    void AddPoint(double pointX, double pointY);
    void RemoveAllPoints();
    int  GetId();
    QString GetTitle();
    DataSetProperties GetProperties();
    void SetProperties(DataSetProperties newProperties);
    void SetColor(QColor Color);
    void SetShowTitle(bool show);
    void SetTitle(QString myTitle);
    void SetShow(bool);

 // Attributes
 public:
    QVector<double> m_pointArrayX;
    QVector<double> m_pointArrayY;
    double minx;
    double maxx;
    double miny;
    double maxy;
    bool bShowCurveTitle;
    bool isShown;

 protected:
    DataSetProperties Properties;
    int maxPoints;
};
