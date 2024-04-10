#ifndef RUNTIME_H
#define RUNTIME_H

#include <queue>
#include <set>
#include <unordered_map>
#include <QHash>
#include <QString>
#include <QStringList>
#include <QDate>
#include <QVector>
#include <QStack>
#include <QFile>
#include <QTextStream>
#include <QColor>
#include <QDebug>

using namespace std;

class SS
{
public:
    SS() : m_lng(0), m_lat(0)
    {
    }

public:
    bool matching(const QString &keyword) const
    {
        if (m_name.indexOf(keyword) != -1)
            return true;
        return false;
    }

    double distance(const SS &station) const;

public:
    bool load(QTextStream &input)
    {
        input.setRealNumberPrecision(6);
        input >> m_name;
        input >> m_lng;
        input >> m_lat;
        return !input.atEnd();
    }

    void save(QTextStream &output) const
    {
        output.setRealNumberPrecision(6);
        output << m_name << " ";
        output << m_lng << " ";
        output << m_lat << " ";
        output << "\n";
    }

public:
    double x() const;
    double y() const;

public:
    QString m_name;
    double m_lng;
    double m_lat;
    int m_line_count;
};

class SSL
{
public:
    SSL()
    {
    }

public:
    bool add(const SS &station);
    bool remove(const QString &name);
    SS *find(const QString &name);
    const SS *find(const QString &name) const;

public:
    void clearLineCount();

public:
    void load();
    void save() const;

public:
    QVector<SS> m_data;
};

class RR
{
public:
    RR()
    {
    }

public:
    void setStations(const QString &text)
    {
        m_stations.clear();
        if (!text.isEmpty())
        {
            for (auto item : text.split(QRegExp("\\s"), QString::SkipEmptyParts))
            {
                if (!m_stations.contains(item))
                {
                    m_stations.append(item);
                }
            }
        }
    }

    QString getStations() const
    {
        return m_stations.join(" ");
    }

    bool matching(const QString &keyword) const
    {
        if (m_id.indexOf(keyword) != -1)
            return true;
        if (m_name.indexOf(keyword) != -1)
            return true;
        if (m_ring.indexOf(keyword) != -1)
            return true;
        if (m_color.indexOf(keyword) != -1)
            return true;
        for (auto &item : m_stations)
        {
            if (item.indexOf(keyword) != -1)
                return true;
        }
        return false;
    }

    QColor getColor() const
    {
        QStringList rgb = m_color.split(",");
        if (rgb.size() == 3)
        {
            return QColor(rgb.at(0).toInt(), rgb.at(1).toInt(), rgb.at(2).toInt());
        }
        return Qt::black;
    }

    void setColor(QColor color)
    {
        m_color = QString("%1,%2,%3").arg(color.red()).arg(color.green()).arg(color.blue());
    }

public:
    bool load(QTextStream &input)
    {
        input >> m_id;
        input >> m_name;
        input >> m_ring;
        input >> m_enabled;
        input >> m_color;
        m_stations.clear();
        int len;
        input >> len;
        for (int index = 0; index < len; ++index)
        {
            QString name;
            input >> name;
            m_stations.append(name);
        }
        return !input.atEnd();
    }

    void save(QTextStream &output) const
    {
        output << m_id << " ";
        output << m_name << " ";
        output << m_ring << " ";
        output << m_enabled << " ";
        output << m_color << " ";
        output << "\n";
        output << m_stations.size() << " ";
        output << "\n";
        for (int index = 0; index < m_stations.size(); ++index)
        {
            output << m_stations[index] << " ";
        }
        output << "\n\n";
    }

public:
    QString m_id;
    QString m_name;
    QString m_ring;
    QString m_enabled;
    QString m_color;
    QStringList m_stations;
};

class RRL
{
public:
    RRL()
    {
    }

public:
    bool add(const RR &route);
    bool remove(const QString &id);
    RR *find(const QString &id);

public:
    void load();
    void save() const;

public:
    QVector<RR> m_data;
};

enum
{
    NAVIGATE_MODE_DISTANCE,
    NAVIGATE_MODE_STATION,
    NAVIGATE_MODE_TRANSFER,
};

class NN
{
public:
    NN()
    {
    }

public:
    void build();

public:
    QString getRRID(const QString &from, const QString &to) const;
    QVector<tuple<QString, QString, double>> nD(const QString &start, const QString &end);
    QVector<tuple<QString, QString, double>> nS(const QString &start, const QString &end);
    QVector<tuple<QString, QString, double>> nT(const QString &start, const QString &end);

public:
    static QString makeKey(const QString &name1, const QString &name2);

private:
    QVector<tuple<QString, QString, double>> dD(const QString &start, const QString &end, int mode);

private:
    double getEdgeWeight(const QString &start, const QString &end, int mode);
    QVector<QString> getNeighbors(const QString &name);

private:
    QHash<QString, double> m_a;
    QVector<tuple<QString, QString, QString>> m_e;
    QSet<QString> m_v;
};

class My
{
public:
    My();

public:
    void update();
    double width() const;
    double height() const;

public:
    double XX(double lng) const;
    double YY(double lat) const;

public:
    QStringList NotSS() const;
    void CL();

public:
    bool hasSS(const QString &name) const;
    RR *getRR(const QString &from, const QString &to);
    bool sameRr(const QString &station1, const QString &station2);
    void nD(const QString &start, const QString &end);
    void nS(const QString &start, const QString &end);
    void nT(const QString &start, const QString &end);
    double countC(double km);

public:
    double m_R;
    double m_SC;
    double m_MLG;
    double m_MLT;
    double m_NLG;
    double m_NLT;
    int m_MAR;
    int m_offset_x;
    int m_offset_y;
    SSL m_SSL;
    RRL m_RRL;
    QVector<tuple<QString, QString, double>> m_AT;

private:
    NN m_nN;
};

extern My runtime;

#endif
