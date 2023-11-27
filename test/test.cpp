#include "dollar.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <pugixml.hpp>

#include <filesystem>
#include <string>
using namespace dollar;
using namespace std;
TEST_CASE("1 instance")
{
    vector<Stroke> strokes{ Stroke({ { 0, 0 }, { 1, 1 }, { 1, 0 } }, Orientation::Insensitive) };
    auto [it, score] = recognize(strokes[0], strokes.begin(), strokes.end());
    CHECK(it == strokes.begin());
    CHECK_THAT(score, Catch::Matchers::WithinAbs(0.63662f, 0.001f));
}

TEST_CASE("Regression of official data")
{
    namespace fs = std::filesystem;
    const fs::path testdata{ "test/testdata" };

    struct Record : dollar::Stroke
    {
        string name;
        Record(const string& _name, const Stroke& _stroke)
          : name(_name)
          , Stroke(_stroke)
        {
        }
    };
    REQUIRE(fs::exists(testdata));
    for (const auto& l1 : fs::directory_iterator{ testdata }) {
        if (!l1.is_directory()) {
            continue;
        }
        for (const auto& l2 : fs::directory_iterator{ l1 }) {
            if (!l2.is_directory()) {
                continue;
            }
            DYNAMIC_SECTION(l1.path().filename().string() + "/" + l2.path().filename().string())
            {
                vector<Record> records;
                for (const auto& f : fs::directory_iterator{ l2 }) {
                    if (!f.is_regular_file()) {
                        continue;
                    }
                    const string stem = f.path().stem().string();
                    vector<Record> records;
                    string_view label = stem;
                    label.remove_suffix(2);
                    pugi::xml_document doc;
                    pugi::xml_parse_result result = doc.load_file(f.path().string().c_str());
                    REQUIRE(result);

                    vector<Point> points;
                    for (pugi::xml_node node : doc.child("Gesture").children("Point")) {
                        points.push_back({ node.attribute("X").as_double(), node.attribute("Y").as_double() });
                    }
                    records.push_back(Record(string{ label }, Stroke(points, Orientation::Sensitive)));
                }
                INFO("Loaded " << records.size() << " shapes");

                for (const Record& testedRec : records) {
                    vector<Point> points = testedRec.getRawPoints();
                    for (Point& p : points) {
                        p.first += (rand() % 3) - 1;
                        p.second += (rand() % 3) - 1;
                    }
                    Stroke stroke(points, Orientation::Sensitive);
                    auto [it, score] = recognize(stroke, records.begin(), records.end());
                    CHECK(it != records.end());
                    CHECK(it->name == testedRec.name);
                    CHECK(score > 0.5);
                }
            }
        }
    }
}
