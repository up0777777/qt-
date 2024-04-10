#include "my.h"
#include "res.rc"
#include <cstdio>
#include <cmath>

#define PI 3.1415926
#define EARTH_RADIUS 6378.137 // 地球近似半径

// 求弧度
static double radian(double d)
{
    return d * PI / 180.0; // 角度1˚ = π / 180
}

// 计算距离
static double countDistance(double lat1, double lng1, double lat2, double lng2)
{
    double radLat1 = radian(lat1);
    double radLat2 = radian(lat2);
    double a = radLat1 - radLat2;
    double b = radian(lng1) - radian(lng2);

    double dst = 2 * asin((sqrt(pow(sin(a / 2), 2) + cos(radLat1) * cos(radLat2) * pow(sin(b / 2), 2))));

    dst = dst * EARTH_RADIUS;
    dst = round(dst * 10000) / 10000;
    return dst;
}

My runtime;

double SS::distance(const SS &station) const
{
    return countDistance(m_lat, m_lng, station.m_lat, station.m_lng);
}

double SS::x() const
{
    return runtime.XX(m_lng);
}

double SS::y() const
{
    return runtime.YY(m_lat);
}

// 添加站点信息信息
bool SSL::add(const SS &station)
{
    // 添加新站点信息之前，先查询是否存在相同编号，若存在，则添加失败
    if (!find(station.m_name))
    {
        m_data.append(station);
        return true;
    }
    return false;
}

// 删除站点信息信息
bool SSL::remove(const QString &name)
{
    // 遍历整个列表，若找到编号匹配的站点信息信息，则从列表中删除
    for (auto iter = m_data.begin(); iter != m_data.end(); ++iter)
    {
        if (iter->m_name == name)
        {
            m_data.erase(iter);
            return true;
        }
    }
    return false;
}

// 查找站点信息信息
SS *SSL::find(const QString &name)
{
    // 遍历整个列表，若找到编号匹配的站点信息信息，则返回该站点信息信息
    for (auto &station : m_data)
    {
        if (station.m_name == name)
        {
            return &station;
        }
    }
    return nullptr;
}

// 查找站点信息信息
const SS *SSL::find(const QString &name) const
{
    return const_cast<SSL *>(this)->find(name);
}

// 清理途径线路计数
void SSL::clearLineCount()
{
    for (auto &station : m_data)
    {
        station.m_line_count = 0;
    }
}

// 从文件读取站点信息信息
void SSL::load()
{
    QFile file("station.txt");
    if (file.open(QFile::ReadOnly | QIODevice::Text))
    {
        QTextStream input(&file);
        while (true)
        {
            SS station;
            if (!station.load(input))
                break;
            add(station);
        }
    }
}

// 将站点信息信息保存到文件
void SSL::save() const
{
    QFile file("station.txt");
    if (file.open(QFile::WriteOnly | QIODevice::Text))
    {
        QTextStream output(&file);
        for (auto &station : m_data)
        {
            station.save(output);
        }
    }
}

// 添加线路信息
bool RRL::add(const RR &route)
{
    // 添加新线路之前，先查询是否存在相同编号，若存在，则添加失败
    if (!find(route.m_id))
    {
        m_data.append(route);
        return true;
    }
    return false;
}

// 删除线路信息
bool RRL::remove(const QString &id)
{
    // 遍历整个列表，若找到编号匹配的线路信息，则从列表中删除
    for (auto iter = m_data.begin(); iter != m_data.end(); ++iter)
    {
        if (iter->m_id == id)
        {
            m_data.erase(iter);
            return true;
        }
    }
    return false;
}

// 查找线路信息
RR *RRL::find(const QString &id)
{
    // 遍历整个列表，若找到编号匹配的线路信息，则返回该线路信息
    for (auto &route : m_data)
    {
        if (route.m_id == id)
        {
            return &route;
        }
    }
    return nullptr;
}

// 从文件读取线路信息
void RRL::load()
{
    QFile file("route.txt");
    if (file.open(QFile::ReadOnly | QIODevice::Text))
    {
        QTextStream input(&file);
        while (true)
        {
            RR route;
            if (!route.load(input))
                break;
            add(route);
        }
    }
}

// 将线路信息保存到文件
void RRL::save() const
{
    QFile file("route.txt");
    if (file.open(QFile::WriteOnly | QIODevice::Text))
    {
        QTextStream output(&file);
        for (auto &route : m_data)
        {
            route.save(output);
        }
    }
}

class QueueVertex
{
public:
    QueueVertex(double weight, const QString &name) : m_weight(weight), m_name(name)
    {
    }

public:
    bool operator<(const QueueVertex &vertex) const
    {
        return vertex.m_weight < m_weight;
    }

public:
    double m_weight;
    QString m_name;
};

void NN::build()
{
    m_a.clear();
    m_e.clear();
    for (auto &route : runtime.m_RRL.m_data)
    {
        if (route.m_enabled == "开启" && route.m_stations.size())
        {
            QStringList stations = route.m_stations;
            if (route.m_ring == "始终相同")
            {
                stations.append(stations[0]);
            }
            SS *start = runtime.m_SSL.find(stations[0]);
            for (int index = 1; index < stations.size(); ++index)
            {
                SS *end = runtime.m_SSL.find(stations[index]);
                if (start && end)
                {
                    m_a.insert(makeKey(start->m_name, end->m_name), start->distance(*end));
                    m_e.append(tuple<QString, QString, QString>{start->m_name, end->m_name, route.m_id});
                }
                start = end;
            }
        }
    }
    for (auto &station : runtime.m_SSL.m_data)
    {
        if (station.m_line_count > 0)
        {
            m_v.insert(station.m_name);
        }
    }
}

QString NN::getRRID(const QString &from, const QString &to) const
{
    for (auto &segment : m_e)
    {
        if ((std::get<0>(segment) == from && std::get<1>(segment) == to) || (std::get<1>(segment) == from && std::get<0>(segment) == to))
        {
            return std::get<2>(segment);
        }
    }
    return QString("");
}

QVector<tuple<QString, QString, double>> NN::nD(const QString &start, const QString &end)
{
    return dD(start, end, NAVIGATE_MODE_DISTANCE);
}

QVector<tuple<QString, QString, double>> NN::nS(const QString &start, const QString &end)
{
    return dD(start, end, NAVIGATE_MODE_STATION);
}

QVector<tuple<QString, QString, double>> NN::nT(const QString &start, const QString &end)
{
    return dD(start, end, NAVIGATE_MODE_TRANSFER);
}

QString NN::makeKey(const QString &name1, const QString &name2)
{
    if (name1 > name2)
    {
        return QString("%1_%2").arg(name1).arg(name2);
    }
    else
    {
        return QString("%1_%2").arg(name2).arg(name1);
    }
}

QVector<tuple<QString, QString, double>> NN::dD(const QString &start, const QString &end, int mode)
{
    QHash<QString, QString> trace;
    trace.insert(start, "");

    QHash<QString, double> dis;
    QSet<QString> mark;
    priority_queue<QueueVertex> queue;

    for (auto &v : m_v)
    {
        dis.insert(v, -1);
    }

    dis[start] = 0;

    queue.push({0, start});
    while (!queue.empty())
    {
        QueueVertex min = queue.top();
        queue.pop();

        if (mark.find(min.m_name) == mark.end())
        {
            mark.insert(min.m_name);
            for (auto &to : getNeighbors(min.m_name))
            {
                double weight = -1;
                if (mode == NAVIGATE_MODE_TRANSFER)
                {
                    QString lastname = trace[min.m_name];
                    if (lastname == "" || runtime.sameRr(lastname, to))
                    {
                        weight = 0;
                    }
                    else
                    {
                        weight = 1;
                    }
                }
                else
                {
                    weight = getEdgeWeight(min.m_name, to, mode);
                }
                if (weight != -1)
                {
                    auto ow = dis[min.m_name];
                    if (ow != -1)
                    {
                        if (dis[to] == -1 || ow + weight < dis[to])
                        {
                            dis[to] = ow + weight;
                            queue.push({ow + weight, to});
                            trace[to] = min.m_name;
                        }
                    }
                }
            }
        }
    }

    QHash<QString, QString>::iterator titer = trace.find(end);
    if (titer != trace.end())
    {
        QVector<tuple<QString, QString, double>> path;
        while (titer.value() != "")
        {
            QString key = titer.key();
            QString value = titer.value();
            path.insert(path.begin(), tuple<QString, QString, double>{value, key, getEdgeWeight(key, value, NAVIGATE_MODE_DISTANCE)});
            titer = trace.find(value);
        }
        return path;
    }
    return {};
}

double NN::getEdgeWeight(const QString &start, const QString &end, int mode)
{
    double weight = -1;
    QString key = makeKey(start, end);
    auto target = m_a.find(key);
    if (target != m_a.end())
    {
        switch (mode)
        {
        case NAVIGATE_MODE_DISTANCE:
            weight = target.value();
            break;
        case NAVIGATE_MODE_STATION:
            weight = 1;
            break;
        }
    }
    return weight;
}

QVector<QString> NN::getNeighbors(const QString &name)
{
    QVector<QString> list;
    //    for (auto iter = m_vertex_list.begin(); iter != m_vertex_list.end(); ++iter) {
    //        if (*iter != name) {
    //            if (getDdgeWeight(*iter, name) != -1) {
    //                list.push_back(*iter);
    //            }
    //        }
    //    }
    for (auto &segment : m_e)
    {
        if (std::get<0>(segment) == name)
        {
            list << std::get<1>(segment);
        }
        else if (std::get<1>(segment) == name)
        {
            list << std::get<0>(segment);
        }
    }
    return list;
}

My::My()
    : m_R(1), m_SC(6), m_MLG(-1), m_MLT(-1), m_NLG(-1), m_NLT(-1), m_MAR(80), m_offset_x(0), m_offset_y(0)
{
    // 从文件中读取站点信息
    m_SSL.load();
    // 从文件中读取线路信息
    m_RRL.load();
    update();
}

void My::update()
{
    for (const auto &station : m_SSL.m_data)
    {
        if (m_MLG == -1 || m_MLG < station.m_lng)
        {
            m_MLG = station.m_lng;
        }
        if (m_MLT == -1 || m_MLT < station.m_lat)
        {
            m_MLT = station.m_lat;
        }
        if (m_NLG == -1 || m_NLG > station.m_lng)
        {
            m_NLG = station.m_lng;
        }
        if (m_NLT == -1 || m_NLT > station.m_lat)
        {
            m_NLT = station.m_lat;
        }
    }
    CL();
    m_nN.build();
}

double My::width() const
{
    return (XX(m_MLG) - XX(m_NLG)) + (m_MAR * 2);
}

double My::height() const
{
    return (YY(m_NLT) - YY(m_MLT) + (m_MAR * 2));
}

double My::XX(double lng) const
{
    return ((lng - m_NLG) * 1000) * m_R + m_MAR + m_offset_x;
}

double My::YY(double lat) const
{
    return ((m_MLT - lat) * 1000) * m_R + m_MAR + m_offset_y;
}

QStringList My::NotSS() const
{
    QStringList list;
    for (auto &route : m_RRL.m_data)
    {
        for (auto &name : route.m_stations)
        {
            const SS *station = m_SSL.find(name);
            if (!station)
            {
                list << name;
            }
        }
    }
    return list;
}

void My::CL()
{
    m_SSL.clearLineCount();
    for (auto &route : m_RRL.m_data)
    {
        if (route.m_enabled == "开启")
        {
            for (auto &name : route.m_stations)
            {
                SS *station = m_SSL.find(name);
                if (station)
                {
                    ++station->m_line_count;
                }
            }
        }
    }
}

bool My::hasSS(const QString &name) const
{
    return (runtime.m_SSL.find(name) != nullptr);
}

RR *My::getRR(const QString &from, const QString &to)
{
    return m_RRL.find(m_nN.getRRID(from, to));
}

bool My::sameRr(const QString &station1, const QString &station2)
{
    for (auto &route : m_RRL.m_data)
    {
        if (route.m_stations.contains(station1) && route.m_stations.contains(station2))
        {
            return true;
        }
    }
    return false;
}

void My::nD(const QString &start, const QString &end)
{
    m_AT = m_nN.nD(start, end);
}

void My::nS(const QString &start, const QString &end)
{
    m_AT = m_nN.nS(start, end);
}

void My::nT(const QString &start, const QString &end)
{
    m_AT = m_nN.nT(start, end);
}

double My::countC(double km)
{
    if (km <= 6)
        return 3;
    if (km <= 12)
        return 4;
    if (km <= 22)
        return 5;
    if (km <= 32)
        return 6;
    if (km <= 52)
        return 7;
    if (km <= 72)
        return 8;
    return 9;
}
