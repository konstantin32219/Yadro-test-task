#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <math.h>
#include <algorithm>
#include "test_task.h"

Time::Time(std::string line) {
    std::string error_msg = "WRONG TIME.";
    if (line.size() != 5) {
        throw error_msg;
    } else {
        for (size_t i = 0; i < 5; ++i) {
            if (i != 2) {
                if (line[i] < '0' && line[i] > '9') {
                    throw error_msg;
                }
            } else if (line[i] != ':') {
                throw error_msg;
            }
        }
    }

    h = std::stoi(line.substr(0, 2));
    m = std::stoi(line.substr(3));

    if (h > 23 || m > 59) {
        throw error_msg;
    }
}

bool Time::operator<(const Time& other) const {
    if (this->h < other.h) {
        return true;
    } else if (this->h == other.h) {
        return this->m < other.m;
    }
    return false;
}

Time Time::operator+(const Time& other) {
    return Time(this->h + other.h + (this->m + other.m) / 60, 
        (this->m + other.m) % 60);
}

Time Time::operator-(const Time& other) {
    int h = this->h - other.h;
    int m = this->m - other.m;
    if (m < 0) {
        m = 60 + m;
        --h;
    }
    return Time(h, m);
}

Time& Time::operator+=(const Time& other) {
    *this = *this + other;
    return *this;
}


std::ostream& operator<<(std::ostream& stream, const Time& time) {
    if (time.h < 10)
        stream << '0';
    stream << time.h << ":";
    if (time.m < 10)
        stream << '0';
    stream << time.m;
    return stream;
}

void Computer_club::Read(std::string file) {
    std::vector<std::string> data;
    std::string line;

    std::ifstream in(file);

    while(std::getline(in, line)) {
        data.push_back(line);
    }

    if (data.size() < 3) {
        throw "NOT ENOUGH INFORMATION.";
    }

    if (data[0][0] < '1' || data[0][0] > '9') {
        std::cout << data[0] << std::endl;
        throw "WRONG TABLE NUMBER.";
    }
    if (data[0][0] < '1' || data[0][0] > '9') {
        std::cout << data[2] << std::endl;
        throw "WRONG COST.";
    }

    shift.tables = std::stoi(data[0]);
    try {
        shift.start = Time(data[1].substr(0, 5));
        shift.finish = Time(data[1].substr(6));
    } catch (...) {
        std::cout << data[1] << std::endl;
        throw "WRONG TIME.";
    }
    shift.cost = std::stoi(data[2]);

    for (size_t i = 3; i < data.size(); ++i) {
        Event ev;
        std::vector<std::string> words;

        line.clear();
        for (auto ch : data[i]) {
            if (ch != ' ') {
                line += ch; 
            } else {
                words.push_back(line);
                line.clear();
            }
        }
        words.push_back(line);

        if (words.size() < 3) {
            throw "NOT ENOUGH WORDS";
        }

        try {
            ev.time = Time(words[0]);
        } catch (...) {
            std::cout << data[i] << std::endl;
            throw "WRONG TIME.";
        }
        if (incoming_events.size() > 0 && ev.time < incoming_events.back().time) {
            std::cout << data[i] << std::endl;
            throw "WRONG ORDER.";
        }
        if (words[1][0] < '1' || words[1][0] > '4') {
            std::cout << data[i] << std::endl;
            throw "WRONG EVENT ID.";
        }
        ev.id = std::stoi(words[1]);
        if (ev.id == 2 && words.size() < 4) {
            throw "NOT ENOUGH WORDS";
        }

        for (auto ch : words[2]) {
            if ((ch < 'a' || ch > 'z') && (ch < '0' || ch > '9') && ch != '_' &&
                ch != '-') {
                std::cout << data[i] << std::endl;
                throw "WRONG CLIENT NAME.";
            }
        }
        ev.name = words[2];
        if (words.size() == 4) {
            if (words[3][0] < '1' || words[3][0] > '9') {
                std::cout << data[i] << std::endl;
                throw "WRONG TABLE ID.";
            }
            ev.table_id = std::stoi(words[3]);
            if (ev.table_id > shift.tables) {
                std::cout << data[i] << std::endl;
                throw "WRONG TABLE ID.";
            }
        }

        if (((ev.id == 2) && words.size() > 4) || words.size() > 3 && (ev.id != 2)) {
            std::cout << data[i] << std::endl;
            throw "TOO MANY ARGUMENTS.";
        }

        incoming_events.push_back(ev);
    }

    in.close();
}

int Computer_club::Find_client_in_table(std::string client) {
    for (auto [table_id, table] : tables) {
        if (table.name == client) {
            return table_id;
        }
    }
    return 0;
}

std::vector<std::string>::iterator Computer_club::Find_waiting_client(std::string client) {
    auto it = waiting_room.begin();
    for (; it != waiting_room.end(); ++it) {
        if ((*it) == client) {
            return it;
        }
    }
    return it;
}

void Computer_club::Outcoming_event(Time time, int id, std::string name) {
    std::cout << time << " " << id << " " << name << std::endl;
}

void Computer_club::Outcoming_event(Time time, int id, std::string name, int table_id) {
    std::cout << time << " " << id << " " << name << " " << table_id << std::endl;
}

void Computer_club::Work(std::string file) {
    try {
        Read(file);
    } catch (const char* err_msg) {
        return;
    }

    table_revenue = std::vector(shift.tables, 0);
    table_usage_time = std::vector<Time>(shift.tables);

    std::cout << shift.start << std::endl;
    for (auto ev : incoming_events) {
        std::cout << ev.time << " " << ev.id << " " << ev.name;
        if (ev.table_id != 0){
            std::cout << " " << ev.table_id;
        }
        std::cout << std::endl;

        switch(ev.id) {
            case 1:
                if (ev.time < shift.start || shift.finish < ev.time) {
                    Outcoming_event(ev.time, 13, "NotOpenYet");
                } else if (Find_client_in_table(ev.name) ||
                            Find_waiting_client(ev.name) != waiting_room.end()) {
                    Outcoming_event(ev.time, 13, "YouShallNotPass");
                } else {
                    waiting_room.push_back(ev.name);
                }
                break;
            case 2:
                if (!Find_client_in_table(ev.name) &&
                    Find_waiting_client(ev.name) == waiting_room.end()) {
                    Outcoming_event(ev.time, 13, "ClientUnknown");
                } else if (tables.count(ev.table_id)) {
                    Outcoming_event(ev.time, 13, "PlaceIsBusy");
                } else {
                    int pos = Find_client_in_table(ev.name);
                    if (pos) {
                        Time period = ev.time - tables[pos].start;
                        table_revenue[pos - 1] += shift.cost * (period.h + 
                            (period.m > 0 ? 1 : 0));
                        table_usage_time[pos - 1] += period;
                        tables.erase(pos);
                    } else {
                        waiting_room.erase(Find_waiting_client(ev.name));
                    }
                    tables[ev.table_id] = Table{ev.name, ev.time};
                }
                break;
            case 3:
                if (tables.size() < shift.tables) {
                    Outcoming_event(ev.time, 13, "ICanWaitNoLonger!");
                } else if (waiting_room.size() > shift.tables) {
                    Outcoming_event(ev.time, 11, ev.name);
                    waiting_room.erase(Find_waiting_client(ev.name));
                }
                break;
            case 4:
                if (!Find_client_in_table(ev.name) &&
                    Find_waiting_client(ev.name) == waiting_room.end()) {
                    Outcoming_event(ev.time, 13, "ClientUnknown");
                } else {
                    int pos = Find_client_in_table(ev.name);
                    if (pos) {
                        Time period = ev.time - tables[pos].start;
                        table_revenue[pos - 1] += shift.cost * (period.h + 
                            (period.m > 0 ? 1 : 0));
                        table_usage_time[pos - 1] += period;
                        tables.erase(pos);

                        if (!waiting_room.empty()) {
                            auto name = *waiting_room.begin();
                            tables[pos] = Table{name, ev.time};
                            waiting_room.erase(Find_waiting_client(name));
                            Outcoming_event(ev.time, 12, name, pos);
                        }
                    } else {
                        waiting_room.erase(Find_waiting_client(ev.name));
                    }
                }
                break;
            default:
                break;
        }
    }

    // waiting_room.clear();
    for (auto [table_id, table] : tables) {
        Time period = shift.finish - table.start;
        table_revenue[table_id - 1] += shift.cost * (period.h + 
            (period.m > 0 ? 1 : 0));
        table_usage_time[table_id - 1] += period;
        waiting_room.push_back(table.name);
    }
    std::sort(waiting_room.begin(), waiting_room.end());
    for (auto cl : waiting_room) {
        Outcoming_event(shift.finish, 11, cl);
    }

    std::cout << shift.finish << std::endl;
    for (size_t i = 0; i < shift.tables; ++i) {
        std::cout << i + 1 << " " << table_revenue[i] << " " << table_usage_time[i]
            << std::endl;
    }
}