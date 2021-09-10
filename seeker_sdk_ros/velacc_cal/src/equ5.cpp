#include <unistd.h>
#include "velacc_cal/Set.h"
#include "velacc_cal/Velo.h"
#include "velacc_cal/Mks.h"
#include "velacc_cal/velacc_cal.h"

velacc_cal::Velo calculate::calcu5(velacc_cal::Mks &a, velacc_cal::Mks &b,velacc_cal::Mks &c,velacc_cal::Mks &d,velacc_cal::Mks &e,bool s_or_a){
	velacc_cal::Velo output;
	for (int i=0;i<a.n_mk;++i){
		velacc_cal::Vec vec;
		for(int j =0;j<3;++j){
			// 3 marker
			if (s_or_a) vec.xyzM.push_back((16*d.mkset[i].coor[j]-e.mkset[i].coor[j]+16*b.mkset[i].coor[j]-a.mkset[i].coor[j]-30*c.mkset[i].coor[j])*60*5);
			else vec.xyzM.push_back((e.mkset[i].coor[j]-a.mkset[i].coor[j]+8*d.mkset[i].coor[j]-8*b.mkset[i].coor[j])*5);
		}
		output.velocity.push_back(vec);
	}
	output.n_frame = b.n_frame;
	return output;
}

void calculate::clbk5(const velacc_cal::Set::ConstPtr& markers){
    if (markers->n_frame == 0) count =0;
    if (markers->n_frame == pf) {
        skip ==1;
        return;
    }
	if (count==0){
		fra1.mkset = markers->mkset;
		fra1.n_mk = markers->n_mk;
		fra1.n_frame = ffir = markers->n_frame;
		++count;
	}
	else if (count ==1){
		fra2.mkset = markers->mkset;
		fra2.n_mk = markers->n_mk;
		fra2.n_frame = fsec = markers->n_frame;
		++count;
	}
	else if (count ==2){
		fra3.mkset = markers->mkset;
		fra3.n_mk = markers->n_mk;
		fra3.n_frame = fthi = markers->n_frame;
		++count;
	}
    else if (count ==3){
        fra4.mkset = markers->mkset;
		fra4.n_mk = markers->n_mk;
		fra4.n_frame = ffor = markers->n_frame;
		++count;
    }
    else if (count ==4){
        fra5.mkset = markers->mkset;
		fra5.n_mk = markers->n_mk;
		fra5.n_frame = ffif = markers->n_frame;
		++count;
        velocity = calcu5(fra1,fra2,fra3,fra4,fra5,0);
	    acceleration = calcu5(fra1,fra2,fra3,fra4,fra5,1);
    }
	else{
		fra1.mkset = fra2.mkset;
		fra2.mkset = fra3.mkset;
        fra3.mkset = fra4.mkset;
		fra4.mkset = fra5.mkset;
		fra5.mkset = markers->mkset;
		fra1.n_mk = fra2.n_mk;
		fra2.n_mk = fra3.n_mk;
        fra3.n_mk = fra4.n_mk;
		fra4.n_mk = fra5.n_mk;
		fra5.n_mk = markers->n_mk;
		fra1.n_frame = ffir = fsec;
		fra2.n_frame = fsec = fthi;
        fra3.n_frame = fthi = ffor;
		fra4.n_frame = ffor = ffif;
		fra5.n_frame = ffif = markers->n_frame;
        velocity = calcu5(fra1,fra2,fra3,fra4,fra5,0);
	    acceleration = calcu5(fra1,fra2,fra3,fra4,fra5,1);
	}
    pf = markers->n_frame;
}