
#include <unistd.h>
#include "velacc_cal/Set.h"
#include "velacc_cal/Velo.h"
#include "velacc_cal/Mks.h"

class calculate
{
private:
public:
    velacc_cal::Mks fra1;
    velacc_cal::Mks fra2;
    velacc_cal::Mks fra3;
    velacc_cal::Mks fra4;
    velacc_cal::Mks fra5;
    velacc_cal::Velo velocity;
    velacc_cal::Velo acceleration;
    int skip;
    int pf;
    int count;
    int ffir;
    int fsec;
    int fthi;
    int ffor;
    int ffif;
    void clbk3(const velacc_cal::Set::ConstPtr& markers);
    void clbk5(const velacc_cal::Set::ConstPtr& markers);
    velacc_cal::Velo calcu3(velacc_cal::Mks &a, velacc_cal::Mks &b,velacc_cal::Mks &c,bool s_or_a);
    velacc_cal::Velo calcu5(velacc_cal::Mks &a, velacc_cal::Mks &b,velacc_cal::Mks &c,velacc_cal::Mks &d,velacc_cal::Mks &e,bool s_or_a);
};