#include <iostream>
#include <vector>
#include <map>

struct Time {
    int h = 0;
    int m = 0;

    Time() {}
    Time(int h_, int m_) : h(h_), m(m_) {}
    Time(std::string line);

    bool operator<(const Time& other) const;
    Time operator+(const Time& other);
    Time operator-(const Time& other);
    Time& operator+=(const Time& other);
};

std::ostream& operator<<(std::ostream& stream, const Time& time);

struct Event {
    Time time;
    int id;
    std::string name;
    int table_id = 0;
};

struct Shift_info {
    int tables;
    Time start;
    Time finish;
    int cost;
};

struct Table {
    std::string name = "";
    Time start;
};

class Computer_club {
public:
    void Work(std::string file);

private:
    void Outcoming_event(Time time, int id, std::string name);
    void Outcoming_event(Time time, int id, std::string name, int table_id);
    int Find_client_in_table(std::string client);
    std::vector<std::string>::iterator Find_waiting_client(std::string client);
    void Read(std::string file);

    std::vector<Event> incoming_events;
    Shift_info shift;
    std::vector<std::string> waiting_room;
    std::map<int, Table> tables;
    std::vector<int> table_revenue;
    std::vector<Time> table_usage_time;
};