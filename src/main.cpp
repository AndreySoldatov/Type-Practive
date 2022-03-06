#include <sstream>
#include <fstream>
#include <codecvt>
#include <ctime>
#include <iostream>
#include <filesystem>
#include <vector>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <numeric>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include <windows.h>
#include <lmcons.h>
#else

#endif

#include <SFML/Graphics.hpp>

std::wstring getRandomString(std::string const &filename, size_t sentence_num = 2) {
    std::wifstream wif(filename);
    if(!wif.good()) {
        return L"File is not found. Instead practice on this unnecesary long error message...";
    }
    wif.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));
    std::wstringstream wss;
    wss << wif.rdbuf();

    wif.close();

    std::wstring text = wss.str();
    std::wstring res;

    size_t sent_num{};

    srand(time(nullptr));
    size_t current_index = (rand() + rand()) % (text.length() - 20);

    while(!(text[current_index] >= 0x41 && text[current_index] <= 0x5A || text[current_index] >= 0x410 && text[current_index] <= 0x42F) && current_index > 0) {
        current_index--;
    }

    while(sent_num < sentence_num && current_index < text.length()) {
        if(text[current_index] != 0x0A) {
            res += text[current_index];
        } else {
            res += 0x20;
        }
        if(text[current_index] == 0x2E || text[current_index] == 0x3b || text[current_index] == 0x21 || text[current_index] == 0x3f) {
            sent_num++;
        }
        current_index++;
    }

    std::cout << "Loaded" << std::endl;

    return res;
}

class Info {
    std::vector<int> m_time_s{};
    std::vector<int> m_wpm_s{};
    std::vector<int> m_entries_s{};
    std::vector<int> m_errors_s{};
    int time{};
    tm m_date{};
    int entries{};
    int errors{};

public:
    Info() {
        updateTime();
    }

    bool loadFromFile(std::string const &path) {
        std::ifstream is(path);

        if(!is.good()) {
            return false;
        }
        
        try {
            std::string time_str;
            std::getline(is, time_str);
            time = std::stoi(time_str);

            std::string stats_str;
            std::getline(is, stats_str);

            m_time_s.clear();
            m_wpm_s.clear();
            m_entries_s.clear();
            m_errors_s.clear();

            size_t index{};
            while((index = stats_str.find(" ")) != std::string::npos) {
                std::string stat = stats_str.substr(0, index);
                stats_str.erase(0, index + 1);

                m_time_s.push_back(std::stoi(stat.substr(0, stat.find("|"))));
                stat.erase(0, stat.find("|") + 1);
                m_wpm_s.push_back(std::stoi(stat.substr(0, stat.find("|"))));
                stat.erase(0, stat.find("|") + 1);
                m_entries_s.push_back(std::stoi(stat.substr(0, stat.find("|"))));
                stat.erase(0, stat.find("|") + 1);
                m_errors_s.push_back(std::stoi(stat));
            }

            std::getline(is, time_str);
            m_date.tm_hour = std::stoi(time_str);

            std::getline(is, time_str);
            m_date.tm_isdst = std::stoi(time_str);

            std::getline(is, time_str);
            m_date.tm_mday = std::stoi(time_str);

            std::getline(is, time_str);
            m_date.tm_min = std::stoi(time_str);

            std::getline(is, time_str);
            m_date.tm_mon = std::stoi(time_str);

            std::getline(is, time_str);
            m_date.tm_sec = std::stoi(time_str);

            std::getline(is, time_str);
            m_date.tm_wday = std::stoi(time_str);

            std::getline(is, time_str);
            m_date.tm_yday = std::stoi(time_str);

            std::getline(is, time_str);
            m_date.tm_year = std::stoi(time_str);

            std::string add_stats;
            std::getline(is, add_stats);
            entries = std::stoi(add_stats);

            std::getline(is, add_stats);
            errors = std::stoi(add_stats);
        } catch(std::exception& ia) {
            return false;
        }

        is.close();

        return true;
    }

    void saveToFile(std::string const &path) const {
        std::stringstream ss;

        std::string user = "default";

        #if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
        char username[UNLEN + 1];
        DWORD username_len = UNLEN + 1;
        GetUserNameA(username, &username_len);
        user = username;
        #else

        #endif

        ss << path << "\\" << user << "_" << std::setw(2) << std::setfill('0') << 
            std::setw(2) << std::setfill('0') << m_date.tm_mday << "." << 
            std::setw(2) << std::setfill('0') << m_date.tm_mon + 1 << "." << 
            m_date.tm_year + 1900 << "_" << 
            std::setw(2) << std::setfill('0') << m_date.tm_hour << "UTC";

        if(!std::filesystem::exists(ss.str())) {
            std::filesystem::create_directory(ss.str());
        }

        ss << "\\" << 
            std::setw(2) << std::setfill('0') << m_date.tm_hour << "." <<
            std::setw(2) << std::setfill('0') << m_date.tm_min << "." <<
            std::setw(2) << std::setfill('0') << m_date.tm_sec << ".tpst";
        
        std::ofstream os(ss.str(), std::ios::trunc);

        os << time << "\n";
        
        for(size_t i = 0; i < m_time_s.size(); i++) {
            os << m_time_s[i] << "|" << m_wpm_s[i] << "|" << m_entries_s[i] << "|" << m_errors_s[i] << " ";
        }
        os << "\n";

        os << 
            m_date.tm_hour << "\n" << 
            m_date.tm_isdst << "\n" << 
            m_date.tm_mday << "\n" << 
            m_date.tm_min << "\n" << 
            m_date.tm_mon << "\n" << 
            m_date.tm_sec << "\n" <<
            m_date.tm_wday << "\n" << 
            m_date.tm_yday << "\n" << 
            m_date.tm_year << "\n";
        
        os << entries << "\n" << errors;
        
        os.close();
    }

    std::vector<int> const &getTimeStat() const {
        return m_time_s;
    }

    std::vector<int> const &getWPMStat() const {
        return m_wpm_s;
    }

    std::vector<int> const &getEntriesStat() const {
        return m_entries_s;
    }

    std::vector<int> const &getErrorsStat() const {
        return m_errors_s;
    }

    int getTime() const {
        return time;
    }

    tm const &get_date() const {
        return m_date;
    }

    void setTime(int time) {
        this->time = time;
    }

    void pushTimeStat(int stat) {
        m_time_s.push_back(stat);
    }

    void pushWPMStat(int stat) {
        m_wpm_s.push_back(stat);
    }

    void pushEntriesStat(int stat) {
        m_entries_s.push_back(stat);
    }

    void pushErrorsStat(int stat) {
        m_errors_s.push_back(stat);
    }

    void updateTime() {
        time_t raw_time;
        std::time(&raw_time);

        m_date = *std::gmtime(&raw_time);
    }

    void setEntries(int e) {
        entries = e;
    }

    void setErrors(int e) {
        errors = e;
    }

    int getEntries() {
        return entries;
    }

    int getErrors() {
        return errors;
    }
};

class UPlotter{
    std::vector<int> m_xdata;
    std::vector<int> m_ydata;
    size_t m_upper_value{100};
    size_t m_height{100};
    size_t m_offset{40};
    size_t m_h_offset{50};
    sf::Vector2f m_pos{};
    sf::Color m_prim_col{255, 69, 134};
    sf::Color m_sec_col{200, 200, 200};

    std::vector<sf::RectangleShape> m_guide_lines;
    std::vector<sf::Text> m_x_guides;
    std::vector<sf::Text> m_y_guides;
    std::vector<sf::Vertex> m_graph_lines;
    std::vector<sf::CircleShape> m_graph_circles;

    std::vector<sf::ConvexShape> m_fill;

public:
    UPlotter(sf::Vector2f const &pos, size_t height, size_t height_offset, size_t offset, std::vector<int> const &xdata, std::vector<int> const &ydata, sf::Font const &f, int text_size, sf::Color const &p_c = sf::Color(255, 69, 134), sf::Color const &s_c = sf::Color(200, 200, 200)) :
        m_pos(pos),
        m_height(height),
        m_h_offset(height_offset),
        m_offset(offset),
        m_xdata(xdata),
        m_ydata(ydata),
        m_prim_col(p_c),
        m_sec_col(s_c) 
    {
        int max_y_val{};
        for(auto &i : m_ydata) {
            max_y_val = i > max_y_val ? i : max_y_val;
        }
        m_upper_value = max_y_val - (max_y_val % height_offset) + ((max_y_val % height_offset == 0) ? 0 : height_offset);

        for(size_t i = 0; i <= m_upper_value; i+=height_offset) {
            sf::RectangleShape guide_line(sf::Vector2f(offset * (m_xdata.size() - 1), 1));
            guide_line.setPosition(pos + sf::Vector2f(35, ((float)i / (float)m_upper_value) * (float)height));
            guide_line.setFillColor(m_sec_col);
            m_guide_lines.push_back(guide_line);

            sf::Text y_text;
            y_text.setFont(f);
            y_text.setCharacterSize(text_size);
            y_text.setFillColor(m_sec_col);
            y_text.setPosition(pos + sf::Vector2f(0, ((float)i / (float)m_upper_value) * (float)height - height * 0.05));
            y_text.setString(std::to_string(m_upper_value - i));
            m_y_guides.push_back(y_text);
        }

        sf::Color trans = m_prim_col;
        trans.a = 40;

        for(size_t i = 0; i < m_xdata.size(); i++) {
            sf::Text x_text;
            x_text.setFont(f);
            x_text.setCharacterSize(text_size);
            x_text.setFillColor(m_sec_col);
            x_text.setPosition(pos + sf::Vector2f(m_offset * i + offset * 0.4, height * 1.03));

            std::stringstream ss;
            ss << std::fixed << std::setprecision(1) << (float)m_xdata[i] / 1000.0;
            x_text.setString(ss.str());
            m_x_guides.push_back(x_text);

            sf::Vertex v;
            v.color = m_prim_col;
            v.position = pos + sf::Vector2f(35 + m_offset * i, ((float)(m_upper_value - m_ydata[i]) / (float)m_upper_value) * height);
            m_graph_lines.push_back(v);

            sf::CircleShape circle(height * 0.01);
            circle.setPosition(pos + sf::Vector2f(35 + m_offset * i - height * 0.01, ((float)(m_upper_value - m_ydata[i]) / (float)m_upper_value) * height - height * 0.01));
            circle.setFillColor(m_prim_col);
            m_graph_circles.push_back(circle);

            if(i < m_xdata.size() - 1) {
                sf::ConvexShape cs;
                cs.setPointCount(4);

                cs.setPoint(0, pos + sf::Vector2f(35 + i * m_offset, height));
                cs.setPoint(1, pos + sf::Vector2f(35 + i * m_offset, ((float)(m_upper_value - m_ydata[i]) / (float)m_upper_value) * height));
                cs.setPoint(2, pos + sf::Vector2f(35 + (i + 1) * m_offset, ((float)(m_upper_value - m_ydata[i + 1]) / (float)m_upper_value) * height));
                cs.setPoint(3, pos + sf::Vector2f(35 + (i + 1) * m_offset, height));

                cs.setFillColor(trans);
                m_fill.push_back(cs);
            }
        }
    }

    void setPos(sf::Vector2f const &v) {
        sf::Vector2f dp = v - m_pos;
        m_pos = v;

        move(dp);
    }

    void move(sf::Vector2f const &dp) {
        m_pos += dp;
        for(auto &g_l : m_guide_lines) {
            g_l.move(dp);
        }

        // for(auto &y_text : m_y_guides) {
        //     y_text.move(dp);
        // }

        for(auto &x_text : m_x_guides) {
            x_text.move(dp);
        }

        for(auto &cs : m_fill) {
            cs.move(dp);
        }

        for(auto &circle : m_graph_circles) {
            circle.move(dp);
        }

        for(auto &v : m_graph_lines) {
            v.position += dp;
        }
    }

    sf::Vector2f const &getPos() {
        return m_pos;
    }

    void draw(sf::RenderWindow &window) {
        for(auto &g_l : m_guide_lines) {
            window.draw(g_l);
        }

        for(auto &y_text : m_y_guides) {
            window.draw(y_text);
        }

        for(auto &x_text : m_x_guides) {
            window.draw(x_text);
        }

        for(auto &cs : m_fill) {
            window.draw(cs);
        }

        for(auto &circle : m_graph_circles) {
            window.draw(circle);
        }

        window.draw(&m_graph_lines[0], m_graph_lines.size(), sf::LineStrip);
    }
};

int main(int argc, char** argv)
{
    srand(time(nullptr));
    sf::Font font;

    std::string exepath = std::string(argv[0]).substr(0, std::string(argv[0]).find_last_of("\\")) + "\\res\\font\\font.ttf";
    font.loadFromFile(exepath);

    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    if(argc != 2) {
        Info info;
        
        std::string path = "res\\text\\";
        std::vector<std::string> files;
        for(const auto &entry : std::filesystem::directory_iterator(path)) {
            files.push_back(entry.path().string());
        }
        size_t current_file{};

        std::wstring str = getRandomString(files[current_file]);
        std::wstring res;

        bool writing = false;

        sf::RectangleShape left(sf::Vector2f(5, 400));
        sf::RectangleShape right(sf::Vector2f(5, 400));
        sf::RectangleShape up(sf::Vector2f(1200, 5));
        sf::RectangleShape down(sf::Vector2f(1200, 5));

        left.setPosition(0, 0);
        right.setPosition(1200 - 5, 0);
        up.setPosition(0, 0);
        down.setPosition(0, 400 - 5);

        left.setFillColor(sf::Color(200, 200, 200));
        right.setFillColor(sf::Color(200, 200, 200));
        up.setFillColor(sf::Color(200, 200, 200));
        down.setFillColor(sf::Color(200, 200, 200));

        sf::Text text;
        text.setFont(font);
        text.setCharacterSize(96);
        text.setFillColor(sf::Color(200, 200, 200));
        text.setPosition(400, 140);
        text.setString(str);

        sf::Text res_text;
        res_text.setFont(font);
        res_text.setCharacterSize(96);
        res_text.setFillColor(sf::Color(70, 70, 70));
        res_text.setPosition(400, 140);
        res_text.setString(res);

        sf::Text clock;
        clock.setFont(font);
        clock.setCharacterSize(32);
        clock.setFillColor(sf::Color(70, 70, 70));
        clock.setPosition(25, 20);
        clock.setString("00:00.000");

        sf::Text wpm;
        wpm.setFont(font);
        wpm.setCharacterSize(32);
        wpm.setFillColor(sf::Color(70, 70, 70));
        wpm.setPosition(1000, 20);
        wpm.setString("WPM: 000");

        sf::Text stats;
        stats.setFont(font);
        stats.setCharacterSize(32);
        stats.setFillColor(sf::Color(70, 70, 70));
        stats.setPosition(25, 340);
        stats.setString("Errors: 00  Presses: 00");

        sf::Text accuracy;
        accuracy.setFont(font);
        accuracy.setCharacterSize(32);
        accuracy.setFillColor(sf::Color(70, 70, 70));
        accuracy.setPosition(850, 340);
        accuracy.setString("Accuracy: 00.00%");

        sf::Clock cl;

        sf::RenderWindow window(sf::VideoMode(1200, 400), "", sf::Style::None, settings);
        window.setFramerateLimit(60);

        int highest_wpm = 0;
        sf::Text highest_wpm_text;
        highest_wpm_text.setFont(font);
        highest_wpm_text.setCharacterSize(32);
        highest_wpm_text.setFillColor(sf::Color(255, 231, 159));
        highest_wpm_text.setPosition(470, 20);
        highest_wpm_text.setString("TOP WPM: 000");

        sf::Text tmp_text = text;
        tmp_text.setString("M");

        sf::Text current_letter;
        current_letter.setFont(font);
        current_letter.setCharacterSize(96);
        current_letter.setFillColor(sf::Color(255, 69, 134));
        current_letter.setPosition(400, 140);
        current_letter.setString(str[0]);
        current_letter.setStyle(sf::Text::Underlined);

        int time{};
        int wpmRate{};
        int error_count{};
        int entries_count{};
        int cursor_rate{};

        sf::Vector2i p_Mpos{};

        float splash = 55.0;

        while (window.isOpen())
        {
            sf::Event event;
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                    window.close();
                
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                    window.close();
                }

                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Tab) {
                    current_file++;
                    current_file %= files.size();

                    writing = false;
                    
                    str = getRandomString(files[current_file]);
                    text.setString(str);
                    res.clear();
                    res_text.setString(res);
                    
                    res_text.setPosition(400, 140);
                    text.setPosition(400, 140);

                    current_letter.setString(str[res.length()]);

                    info = Info();
                }

                if(event.type == sf::Event::TextEntered && event.text.unicode != 0x09) {
                    entries_count++;
                    if(!writing) {
                        writing = true;
                        cl.restart();
                        time = 0;
                        error_count = 0;
                        entries_count = 1;
                    }
                    if(str[res.length()] == event.text.unicode) {
                        res += event.text.unicode;
                        res_text.setString(res);
                        text.move(sf::Vector2f(400.0 - text.findCharacterPos(res.length()).x, 0.0));
                        res_text.move(sf::Vector2f(400.0 - res_text.findCharacterPos(res.length()).x, 0.0));
                    } else {
                        error_count++;
                        splash = 0.0;
                    }
                    if(str.length() == res.length()) {
                        writing = false;

                        str = getRandomString(files[current_file]);
                        text.setString(str);
                        res.clear();
                        res_text.setString(res);
                        
                        res_text.setPosition(400, 140);
                        text.setPosition(400, 140);

                        info.setTime(cl.getElapsedTime().asMilliseconds());
                        info.setEntries(entries_count);
                        info.setErrors(error_count);

                        info.updateTime();
                        info.saveToFile("res\\log");

                        info = Info();
                    }
                    std::stringstream stat;
                    stat << "Errors: " << std::setw(2) << std::setfill('0') << error_count << "  ";
                    stat << "Presses: " << std::setw(2) << std::setfill('0') << entries_count;
                    stats.setString(stat.str());

                    std::stringstream accuracy_str;
                    accuracy_str << "Accuracy: " << std::fixed << std::setw(2) << std::setprecision(2) << std::setfill('0') << ((float)(entries_count - error_count) / (float)(entries_count)) * 100.0 << "%";
                    accuracy.setString(accuracy_str.str());

                    current_letter.setString(str[res.length()]);
                }
            }

            if(sf::Mouse::isButtonPressed(sf::Mouse::Left) && window.hasFocus()) {
                window.setPosition(window.getPosition() + (sf::Mouse::getPosition() - p_Mpos));
            }

            p_Mpos = sf::Mouse::getPosition();

            int dt = cl.getElapsedTime().asMilliseconds() - time;
            time += dt;
            if(writing) {
                wpmRate += dt;
            }
            cursor_rate += dt;

            splash += (float)dt * 0.1;
            if (splash > 55.0) {
                splash = 55.0;
            }

            window.clear(sf::Color(255, 200 + (int)splash, 200 + (int)splash));

            window.draw(text);
            window.draw(res_text);
            window.draw(current_letter);

            if(writing) {
                int millis = cl.getElapsedTime().asMilliseconds() % 1000;
                int seconds = (cl.getElapsedTime().asMilliseconds() / 1000) % 60;
                int minutes = (cl.getElapsedTime().asMilliseconds() / 60000) % 60;
                std::stringstream clock_str;
                clock_str << std::setw(2) << std::setfill('0') << minutes << ":";
                clock_str << std::setw(2) << std::setfill('0') << seconds << ":";
                clock_str << std::setw(3) << std::setfill('0') << millis;
                clock.setString(clock_str.str());
            }
            window.draw(clock);

            if(writing && wpmRate >= 500) {
                wpmRate = 0;
                std::stringstream wpm_str;
                int wpm_num = (int)(((float)res.length() / 5.0) / ((float)cl.getElapsedTime().asMilliseconds() / 60000.0));
                wpm_str << "WPM: " << std::setw(3) << std::setfill('0') << (wpm_num > 0 ? wpm_num : 0);
                wpm.setString(wpm_str.str());

                if(cl.getElapsedTime().asMilliseconds() > 5000) {
                    highest_wpm = (wpm_num > highest_wpm) ? wpm_num : highest_wpm;
                    std::stringstream highest_wpm_str;
                    highest_wpm_str << "TOP WPM: " << std::setw(3) << std::setfill('0') << highest_wpm;
                    highest_wpm_text.setString(highest_wpm_str.str());
                }

                info.pushTimeStat(cl.getElapsedTime().asMilliseconds());
                info.pushWPMStat(wpm_num);
                info.pushEntriesStat(entries_count);
                info.pushErrorsStat(error_count);
            }

            // if(cursor_rate >= 500) {
            //     cursor_rate = 0;
            //     current_letter.setStyle(current_letter.getStyle() > 0 ? 0 : 4);
            // }

            window.draw(wpm);
            window.draw(highest_wpm_text);
            
            window.draw(stats);
            window.draw(accuracy);

            window.draw(left);
            window.draw(right);
            window.draw(up);
            window.draw(down);

            window.display();
        }
    } else {
        Info info;

        if(!info.loadFromFile(argv[1])) {
            sf::RenderWindow window(sf::VideoMode(900, 100), "", sf::Style::None, settings);
            window.setFramerateLimit(60);

            sf::Text text;
            text.setFont(font);
            text.setCharacterSize(48);
            text.setFillColor(sf::Color(200, 200, 200));
            text.setString("Unsupported or corrupted file");
            text.setPosition(20, 20);

            sf::RectangleShape bound(sf::Vector2f(890, 90));
            bound.setPosition(5, 5);
            bound.setFillColor(sf::Color(0));
            bound.setOutlineThickness(5);
            bound.setOutlineColor(sf::Color(200, 200, 200));

            while (window.isOpen())
            {
                sf::Event event;
                while (window.pollEvent(event))
                {
                    if (event.type == sf::Event::Closed)
                        window.close();
                    
                    if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                        window.close();
                    }
                }

                window.clear(sf::Color::White);
                window.draw(text);
                window.draw(bound);
                window.display();
            }
        } else {
            sf::RenderWindow window(sf::VideoMode(1200, 680), "", sf::Style::None, settings);
            window.setFramerateLimit(60);

            sf::Clock cl;
            int time = 0;
            float dt = 0.0;

            UPlotter pl_wpm(sf::Vector2f(40, 120), 200, 25, 50, info.getTimeStat(), info.getWPMStat(), font, 16);

            std::vector<int> acc;
            for(size_t i = 0; i < info.getEntriesStat().size(); i++) {
                acc.push_back((int)(((float)(info.getEntriesStat()[i] - info.getErrorsStat()[i]) / (float)(info.getEntriesStat()[i])) * 100.0));
            }

            UPlotter pl_acc(sf::Vector2f(40, 420), 200, 20, 50, info.getTimeStat(), acc, font, 16, sf::Color(255, 192, 43));

            sf::Text wpm;
            wpm.setFont(font);
            wpm.setCharacterSize(32);
            wpm.setFillColor(sf::Color(70, 70, 70));
            wpm.setPosition(40, 20);

            std::stringstream wpm_ss;
            wpm_ss << "MIN WPM: " << *std::min_element(std::begin(info.getWPMStat()), std::end(info.getWPMStat())) << "  ";
            wpm_ss << "MAX WPM: " << *std::max_element(std::begin(info.getWPMStat()), std::end(info.getWPMStat())) << "  ";
            wpm_ss << "AVR WPM: " << (int)((float)std::accumulate(info.getWPMStat().begin(), info.getWPMStat().end(), 0) / (float)info.getWPMStat().size());

            wpm.setString(wpm_ss.str());

            sf::Text acc_text;
            acc_text.setFont(font);
            acc_text.setCharacterSize(32);
            acc_text.setFillColor(sf::Color(70, 70, 70));
            acc_text.setPosition(850, 20);

            std::stringstream acc_ss;
            acc_ss << "Accuracy: " << std::fixed << std::setprecision(2) << 100.0 * (float)(info.getEntries() - info.getErrors()) / (float)(info.getEntries()) << "%";

            acc_text.setString(acc_ss.str());

            sf::Text labelWPM;
            labelWPM.setFont(font);
            labelWPM.setCharacterSize(24);
            labelWPM.setFillColor(sf::Color(200, 200, 200));
            labelWPM.setPosition(40, 70);
            labelWPM.setString("WPM:");

            sf::Text labelAcc;
            labelAcc.setFont(font);
            labelAcc.setCharacterSize(24);
            labelAcc.setFillColor(sf::Color(200, 200, 200));
            labelAcc.setPosition(40, 370);
            labelAcc.setString("Accuracy:");

            sf::RectangleShape bound(sf::Vector2f(1190, 670));
            bound.setPosition(5, 5);
            bound.setFillColor(sf::Color(0));
            bound.setOutlineThickness(5);
            bound.setOutlineColor(sf::Color(200, 200, 200));
               
            float speed = 0.0;
            sf::Vector2i p_Mpos{};

            while (window.isOpen())
            {
                sf::Event event;
                while (window.pollEvent(event))
                {
                    if (event.type == sf::Event::Closed) {
                        window.close();
                    }
                    
                    if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                        window.close();
                    }

                    if(event.type == sf::Event::MouseWheelScrolled) {
                        if(event.mouseWheelScroll.delta > 0) {
                            if(pl_wpm.getPos().x <= 40.0) {
                                speed += event.mouseWheelScroll.delta;
                            }
                        } else {
                            if(pl_wpm.getPos().x >= (120.0 - info.getTimeStat().size() * 50.0)) {
                                speed += event.mouseWheelScroll.delta;
                            }
                        }
                    }
                }

                if(sf::Mouse::isButtonPressed(sf::Mouse::Left) && window.hasFocus()) {
                    window.setPosition(window.getPosition() + (sf::Mouse::getPosition() - p_Mpos));
                }

                p_Mpos = sf::Mouse::getPosition();

                dt = (float)(cl.getElapsedTime().asMilliseconds() - time) * 0.001;
                time = cl.getElapsedTime().asMilliseconds();

                pl_wpm.move(sf::Vector2f(speed * dt * 1000, 0));
                pl_acc.move(sf::Vector2f(speed * dt * 1000, 0));

                speed *= 1.0 - 20.0 * dt;

                window.clear(sf::Color::White);
                pl_wpm.draw(window);
                pl_acc.draw(window);

                window.draw(wpm);
                window.draw(acc_text);

                window.draw(labelAcc);
                window.draw(labelWPM);

                window.draw(bound);
                window.display();
            }
        }
    }

    return 0;
}