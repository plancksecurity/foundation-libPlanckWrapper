#include "../src/PityUnit.hh"
#include <iostream>
#include <algorithm>

using namespace std;
using namespace pEp;
using namespace pEp::PityTest11;


class Car {
private:
    int gear_nr = 0;

public:
    void setGear(int nr)
    {
        gear_nr = nr;
    }

    void drive()
    {
        if (gear_nr > 0 && gear_nr <= 6) {
            cout << "cruising" << endl;
        } else {
            throw runtime_error{ "invalid gear" };
        }
    }
};

class CarTestModel {
public:
    CarTestModel(const string& name) : name{ name }, car{} {}

    string name{};
    Car car;
};

void test_setGear(PityUnit<CarTestModel>& node, CarTestModel* ctx)
{
    int gear = 1;
    node.log("Setting gear to: " + to_string(gear));
    ctx->car.setGear(gear);
}

void test_drive(PityUnit<CarTestModel>& node, CarTestModel* ctx)
{
    ctx->car.drive();
}

int main(int argc, char* argv[])
{
    // Linear Test
    CarTestModel model{ "CarTestModel" };
    PityUnit<CarTestModel> testnode_car{ nullptr, "test car", nullptr, &model };
    PityUnit<CarTestModel> testnode_setGear{ &testnode_car, "test car setGear()", test_setGear };
    PityUnit<CarTestModel> testnode_driv{ &testnode_setGear, "test car drive()", &test_drive };

    PityUnit<CarTestModel> testnode_driv_before_gear{ &testnode_car,
                                                      "testnode_driv_before_gear",
                                                      &test_drive };

    testnode_car.run();
}